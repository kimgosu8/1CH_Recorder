/********************************************************************************
   INCLUDES
********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>

#define __IDE_C__
#include "ide.h"
#undef __IDE_C__

#include "env.h"

#include "typedef.h"
#include "svr_common.h"
#include "server.h"


/* for lseek64 
#define _LARGEFILE64_SOURCE
#include <sys/types.h>
#include <unistd.h>

ex)...	temp2=lseek64(0, temp1, &result, SEEK_SET);
*/

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

//	sys_llseek TYPE
//	int sys_llseek(unsigned int fd, unsigned long offset_high, unsigned long offset_low, loff_t * result, unsigned int origin)
//	int e = syscall(SYS__llseek, 1, temp1, temp2, &res, SEEK_SET);




/********************************************************************************
   DEFINES
********************************************************************************/


/********************************************************************************
   GLOBALS
********************************************************************************/


char 		dev_str[128];
BYTE  		at_boot_time;
BYTE		forced_head_change;
BYTE		num_disks;
BYTE		seq_disks;
WORD 		sataHDD_error;



/********************************************************************************
   LOCALS
********************************************************************************/


/********************************************************************************
   Function Prototype Declare
********************************************************************************/

void init_preBuffer(); // should be protected by mutex prior to calling this.
void clock_debug_message(time_t target);
void insert_freeblock_to_dataLGV(LOGICAL_VOLUME_GROUP* target_ptr, time_t target_time);





/********************************************************************************
   Function define
********************************************************************************/


void HDD_MAKE_COPY_ROOM() // make copy room
{
    	FILE 			*temp_fp1;    
    	FILE 			*temp_fp2;
    	char str[80], str1[80], str2[80], str3[80], str4[80];


	
    	system("rm -rf /tmp/text.txt");
//	system ("cat /proc/partitions | grep hda >> /tmp/text.txt");
	system ("cat /proc/partitions | grep sdb >> /tmp/text.txt");
//	system ("cat /proc/partitions | grep hdc >> /tmp/text.txt");
//	system ("cat /proc/partitions | grep hdd >> /tmp/text.txt");
	temp_fp1 = fopen("/tmp/text.txt","r");


	while (1)
	{
		fgets(str, 80, temp_fp1); 
printf("str =====>%s\n", str);
		if( feof(temp_fp1) )
		{
			fclose(temp_fp1);
			break;
		}
		sscanf (str, "%s %s %s %s", str1, str2,str3,str4);
		
		if ( !strcmp(str4, "sdb") )
		{
			system("rm -rf /tmp/text1.txt");
			system ("cat /proc/partitions | grep sdb1 > /tmp/text1.txt");  
			temp_fp2 = fopen("/tmp/text1.txt","r");
			fgets(str, 80, temp_fp2); 

			if( feof(temp_fp2) )
			{
printf("Make a copy room!!!\n");
				sprintf(str,"/user_2/HDD_init");
				system (str);
			}

			system("rm -rf /tmp/text.txt");
			system("rm -rf /tmp/text1.txt");     
			fclose(temp_fp1);
			fclose(temp_fp2);
			gNewVar.copy_room_exist = 1;
			break;
		}
	}

}




void ide_identify()
{
	int ii;
	char temp_str[128];


	at_boot_time = 1;
	forced_head_change = 0;
	num_disks = 0;
	seq_disks = 0;
	sataHDD_error = 0;
	gNewVar.ERROR_HDD_ID = 0xff;
	gNewVar.HDD_IO_ERROR = 0;

	memset( ide_devs_info, 0, sizeof(IDE_DEV_INFO) * MAX_HDD_NUM ); // sizeof(ide_devs_info)와 같다. 



	// SCAN HDD
	for(ii = 1; ii < MAX_IDE_NUM; ii++)
	{
		ide_devs_info[ii].tLVG = (LOGICAL_VOLUME_GROUP *)malloc( sizeof(LOGICAL_VOLUME_GROUP) * MAX_NUM_VOLUMES );
		sprintf (temp_str, "/dev/%s", make_device_string(ii));

		if ((ide_devs_info[ii].ide_fd = open( temp_str, O_RDWR)) >= 0)
		{
			ioctl( ide_devs_info[ii].ide_fd, HDIO_GET_IDENTITY, (BYTE*)&ide_devs_info[ii].info);

			if( (ide_devs_info[ii].info.command_set_2 & 0x0400) && (ide_devs_info[ii].info.cfs_enable_2 & 0x0400) )
			{
				ide_devs_info[ii].sector_size = ide_devs_info[ii].info.lba_capacity_2;
				printf("1. ide_devs_info[%d].sector_size =>%lld\n", ii, ide_devs_info[ii].sector_size); // 234441648 * 512 = 120034123776 (120G 기준)
			}
			else
			{
				ide_devs_info[ii].sector_size = (UINT64)ide_devs_info[ii].info.lba_capacity;
				printf("2. ide_devs_info[%d].sector_size =>%lld\n", ii, ide_devs_info[ii].sector_size); 
			}

			if(ide_devs_info[ii].sector_size > 0)
			{
				ide_devs_info[ii].exist = TRUE;
				ide_devs_info[ii].info.buf_type = 0;
				ide_devs_info[ii].info.max_multsect = 0;
				printd("[HDD %d] Sector size is %lld \n", ii, ide_devs_info[ii].sector_size);
			}

			num_disks++;
		}
		else
		{
			ide_devs_info[ii].ide_fd = 0;
			ide_devs_info[ii].sector_size = 0;
			ide_devs_info[ii].exist = FALSE;
		}

		// printf("ide_devs_info[%d].ide_fd ==>%d\n", ii, ide_devs_info[ii].ide_fd);
	}


	// SCAN SCSI HDD
//	sataHDD_scanning();

	probe_HDD_info(0);
	
	print_hdd_checking(-1);

	at_boot_time = 0;

	return ;

}



char* make_device_string(int number)
{
	sprintf( dev_str, "sd%c", 'a' + number);

	return dev_str;
}


void probe_HDD_info(int force)
{
	int loop;
	int result;
	int read_offset;

	BYTE pTemp[512];
	NEW_HDD_HEADER HDD_Header;
	LOGICAL_VOLUME_GROUP* pLVG;



//	pthread_mutex_lock( &gNewVar.hdd_network_lock );
	pthread_mutex_lock( &gNewVar.hdd_encode_lock );

	for ( loop = 1; loop < MAX_HDD_NUM; loop++)
		pthread_mutex_lock(&ide_devs_info[loop].ide_access_wait);

	gNewVar.Record.record_ready = OFF;
	data_LVG.num_LVs = 0;
	free_LVG.num_LVs = 0;
	memset( &data_LVG, 0, sizeof(struct tagLVG_list));
	memset( &free_LVG, 0, sizeof(struct tagLVG_list));
	memset( gLVG, 0, sizeof(gLVG));


	// loop 0는 sda이므로 안함!
	for ( loop = 1; loop < MAX_HDD_NUM; loop++)
	{
		if ( ide_devs_info[loop].exist != 0 )
		{
			memset( ide_devs_info[loop].tLVG, 0, sizeof(ide_devs_info[loop].tLVG));
			result = read_hdd_info( &HDD_Header, loop, (force) & ((int)ide_devs_info[loop].format) );

			if ( result > 0 )
				memcpy( &ide_devs_info[loop].hdd_header, &HDD_Header, sizeof(NEW_HDD_HEADER) );
			else
				ide_devs_info[loop].format = 1; // 구성되지 않은 하드 디스크가 있음.
		}
	}

	ide_devs_info[0].format = 0;




	if ( force == 0 ) // booting time
	{
		if ( ide_devs_info[1].exist == TRUE )
		{
			if ( ide_devs_info[1].format )
				result = format_hdd( 1, &ide_devs_info[1].hdd_header);
			else
				result = configure_LogicalVolumeGroup( 1, &ide_devs_info[1].hdd_header);
		}
	}

printf("data_LVG.num_LVs ==>%d\n", data_LVG.num_LVs);


	if ( data_LVG.num_LVs > 0 )
	{
		qsort( (void*)gLVG, data_LVG.num_LVs, sizeof(LOGICAL_VOLUME_GROUP*), &LVG_sort); //스타트 타임을 기준으로 정렬. status가 2(1)->0 이 되도록.

		for( loop = 0; loop < data_LVG.num_LVs; loop++)
		{
			pLVG = gLVG[loop];

			if ( data_LVG.LVG_ptr_head == NULL )
			{
				data_LVG.LVG_ptr_head = pLVG;
				pLVG->prev_pLVG = NULL;
				pLVG->next_pLVG = NULL;
				data_LVG.LVG_ptr_tail = pLVG;
			}
			else
			{
				data_LVG.LVG_ptr_tail->next_pLVG = pLVG;
				pLVG->prev_pLVG = data_LVG.LVG_ptr_tail;
				pLVG->next_pLVG = NULL;
				data_LVG.LVG_ptr_tail = pLVG;
			}
		}

		pLVG = find_proper_position(gNewVar.current_time);
		if ( pLVG == NULL ) // in this case, exact place does not matter.
			pLVG = data_LVG.LVG_ptr_tail;


		if ( pLVG->tLVH.new_offset > pLVG->tLVH.last_offset )
		{
			printf("[DEBUG] WEIRD! CODE = 154\n");
			read_offset = pLVG->start_offset + pLVG->tLVH.new_offset;
		}
		else
			read_offset = pLVG->start_offset + pLVG->tLVH.last_offset;

		result = ide_block_read( pLVG->hdd_id, read_offset, pTemp, 1);
		do {
			if ( result == 1 )
			{
				if ( pLVG->tLVH.start_time == ((NEW_GROUP_HEADER*)pTemp)->lvg_time && check_crc( pTemp, 511) == 1 )
					printf("[HDD] Full Verifying... success \n");
				else
				{
					result = 0;
					continue;
				}


				read_offset = pLVG->start_offset + LVG_TIMEINDEX_OFFSET;
				result = ide_block_read( pLVG->hdd_id, read_offset, (BYTE *)gNewVar.Record.pTimeIndex, LVG_TIMEINDEX_BLOCKS);
				if ( result != 1 )
					continue;

				read_offset = pLVG->start_offset + LVG_EVENTINDEX_OFFSET;
				result = ide_block_read( pLVG->hdd_id, read_offset, (BYTE *)gNewVar.Record.pEventIndex, LVG_EVENTINDEX_BLOCKS);
				if ( result != 1 )
					continue;


				gNewVar.Record.Rec_offset = ((NEW_GROUP_HEADER*)pTemp)->current_offset;
				gNewVar.Record.Prev_offset = ((NEW_GROUP_HEADER*)pTemp)->prev_offset;
				gNewVar.Record.last_blocks = gNewVar.Record.Rec_offset - gNewVar.Record.Prev_offset;
				gNewVar.Record.LVG_ptr = pLVG;

				pLVG->tLVH.status = 1;
				pLVG->tLVH.new_offset = gNewVar.Record.Rec_offset;
				pLVG->tLVH.last_offset = gNewVar.Record.Prev_offset;
				
				gNewVar.Record.repeat = 0;
				gNewVar.Record.record_ready = ON;
				break;
			}

			printf("[HDD] Full Verifying... FAIL \n");
			
			gNewVar.Record.LVG_ptr = pLVG;
			gNewVar.Record.Prev_offset = 0;
			gNewVar.Record.Rec_offset = LVG_DATA_OFFSET;
			gNewVar.Record.last_blocks = 0;
			memset( gNewVar.Record.pTimeIndex, 0, LVG_TIMEINDEX_BLOCKS<<9);
			memset( gNewVar.Record.pEventIndex, 0, LVG_EVENTINDEX_BLOCKS<<9);
			pLVG->tLVH.status = 0;
			pLVG->tLVH.new_offset = gNewVar.Record.Rec_offset;
			pLVG->tLVH.last_offset = gNewVar.Record.Prev_offset;
			gNewVar.Record.forced_flush = 1;
			printd("[HDD] Instead of continuing, New block %d - %d \n", pLVG->tLVH.volume_id, pLVG->tLVH.new_offset);
		
			gNewVar.Record.repeat = 0;
			gNewVar.Record.record_ready = ON;
			break;
		} while(1);

	}
	else
	{
		if ( free_LVG.LVG_ptr_head != NULL )
		{
			pLVG = (LOGICAL_VOLUME_GROUP*)pop_free_LVG();

			if ( data_LVG.LVG_ptr_head == NULL ) // 당연히 NULL
			{
				data_LVG.LVG_ptr_head = pLVG;
				pLVG->prev_pLVG = NULL;
				pLVG->next_pLVG = NULL;
				data_LVG.LVG_ptr_tail = pLVG;
			}
			else
			{
				printf("What's wrong with you?????????????????\n");
				data_LVG.LVG_ptr_tail->next_pLVG = pLVG;
				pLVG->prev_pLVG = data_LVG.LVG_ptr_tail;
				pLVG->next_pLVG = NULL;
				data_LVG.LVG_ptr_tail = pLVG;
			}

			gNewVar.Record.LVG_ptr = pLVG;
			gNewVar.Record.Prev_offset = 0;
			gNewVar.Record.Rec_offset = LVG_DATA_OFFSET;
			gNewVar.Record.last_blocks = 0;
			memset( gNewVar.Record.pTimeIndex, 0, LVG_TIMEINDEX_BLOCKS<<9);
			memset( gNewVar.Record.pEventIndex, 0, LVG_EVENTINDEX_BLOCKS<<9);
			pLVG->tLVH.status = 0;
			pLVG->tLVH.new_offset = gNewVar.Record.Rec_offset;
			pLVG->tLVH.last_offset = gNewVar.Record.Prev_offset;
			gNewVar.Record.forced_flush = 1;
			printf("[HDD] I will use new block %d - %d \n", pLVG->tLVH.volume_id, (int)pLVG->tLVH.new_offset);

			gNewVar.Record.repeat = 0;
			gNewVar.Record.record_ready = ON;
		}
		else
		{
			printf("[HDD] No HDD found!!!!!!!!!! \n"); // 발견된 HDD가 없으므로 경고해야 함.
			gNewVar.Record.LVG_ptr = &ide_devs_info[0].tLVG[0];
			gNewVar.Record.record_ready = OFF;
		}
	}


	printf("[HDD] DATA : %03d, FREE : %03d \n", data_LVG.num_LVs, free_LVG.num_LVs);

	calc_hdd_free();


	if (force)
	{
		init_preBuffer(); // buffer clear
//		disconnect_networkSearch(PLAY_STOP_BYFORMAT); // disconnect HDD_read network connection
	}


	for ( loop = 1; loop < MAX_HDD_NUM; loop++)
	{
		ide_devs_info[loop].format = 0;
		pthread_mutex_unlock(&ide_devs_info[loop].ide_access_wait);
	}

	pthread_mutex_unlock( &gNewVar.hdd_encode_lock );
//	pthread_mutex_unlock( &gNewVar.hdd_network_lock );

	return;
}



int read_hdd_info( NEW_HDD_HEADER* HDD_pHeader, int hdd_id, int force)
{
	int result;
	int read_result1, read_result2;
	char tSig[16]="HaHaHa"; // 고유한 string 을 집어너놓고 매 부팅시마다 확인?
	BYTE ptr[512];

	read_result1 = 0;
	read_result2 = 0;

// printf("force ==>%d\n", force);

	if ( force == 0 )
	{
		result = 0;
		read_result1 = ide_block_read( hdd_id, NEW_HDD_HEADER_OFFSET, (BYTE *)ptr, 1);

		if ( read_result1 == 1 )
			result = check_crc( ptr, sizeof(NEW_HDD_HEADER)); // sizeof : 60

		if( (result == 1) && (memcmp( ((NEW_HDD_HEADER*)ptr)->signature, tSig, 6) == 0) ) // TODO : Signature 바꿀것.
		{
			printf("1. [HDD /dev/%s] CRC Check OK!\n", make_device_string(hdd_id) );
			memcpy( HDD_pHeader, ptr, sizeof(NEW_HDD_HEADER) );

			if ( HDD_pHeader->time_signature != 0x20090218 )
			{
				HDD_pHeader->num_bad_volumes = 0;
				HDD_pHeader->time_signature = 0x20090218; // // time signiture로 bad block 기능이 있는지 없는지 check 했구나!!!
				memset( HDD_pHeader->reserved1, 0, sizeof(HDD_pHeader->reserved1) );
			}

			if ( HDD_pHeader->num_volumes > 0 )
			{
				// read_mirror_info(hdd_id);
			}


			return 1;
		}
		else
		{
			result = 0;
			read_result2 = ide_block_read( hdd_id, NEW_HDD_HEADER_BACKUP_OFFSET, (BYTE *)ptr, 1);

			if ( read_result2 == 1 )
				result = check_crc( ptr, sizeof(NEW_HDD_HEADER));

			if( (result == 1) && (memcmp( ((NEW_HDD_HEADER*)ptr)->signature, tSig, 6) == 0) ) // TODO : Signature 바꿀것.
			{
				printf("2. [HDD /dev/%s] CRC Check OK!\n", make_device_string(hdd_id) );
				memcpy( HDD_pHeader, ptr, sizeof(NEW_HDD_HEADER) );

				if ( HDD_pHeader->time_signature != 0x20090218 )
				{
					HDD_pHeader->num_bad_volumes = 0;
					HDD_pHeader->time_signature = 0x20090218;
					memset( HDD_pHeader->reserved1, 0, sizeof(HDD_pHeader->reserved1) );
				}

				if ( HDD_pHeader->num_volumes > 0 )
				{
					// read_mirror_info(hdd_id);
					printf("[HDD /dev/%s] !!!!!!! PRIMARY FAT RESTORED !!!!!!!!\n", make_device_string(hdd_id) );
					// ide_block_write( hdd_id, NEW_HDD_HEADER_OFFSET, ptr, 1, 0);
				}

				return 1;
			}
		}
	}


	if ( force )
	{
		printf("[HDD /dev/%s] Formatting... \n", make_device_string(hdd_id) );
	}
	else // DVR 헤더 정보가 제대로 안 들어가 있는 하드 디스크 이므로 format?한다.
	{
		printf("[HDD /dev/%s] CRC Check Failure! \n", make_device_string(hdd_id) );
		//return result; // 구성못한 HDD를 자동 포맷하고 싶지 않으면, return

		printf("read_result1 ==>%d\n", read_result1);
		printf("read_result2 ==>%d\n", read_result2);

		if ( read_result1 == 0 && read_result2 == 0 ) // 버뜨, 이 두 값이 모두 0이면 기본적인 HDD 능력을 상실했다고 봄. 2007 12 10
		{
			ide_devs_info[hdd_id].exist = 0;
		}
	}


	return 0; // 무조건 포맷?

}



int format_hdd(int hdd_id, NEW_HDD_HEADER *HDD_pHeader)
{	
	char tSig[16]="HaHaHa";
	BYTE ptr[512];
	int loop;
	int tNum;
	int probe_success=0;
	LOGICAL_VOLUME_GROUP* pLVG;
	
	print_hdd_checking(hdd_id);

printf("FORMAT HDD !!!!!!!!!\n");


	memset( ptr, 0, 512);


	tNum = ide_devs_info[hdd_id].sector_size >> 21; // 이미 sector_size는 9가 나누어져 있다. 따라서 2의 30승, 전체 HDD의 크기에서 1G 로 나눈 값이 된다. 

	if ( tNum < 10 ) // 10 기가 보다 작으면 NG
	{
		printf("[HDD] HDD size is not enough !!! \n");
		return 0;
	}

	if ( tNum > MAX_NUM_VOLUMES ) // 최대 2048, 2Tera
		tNum = MAX_NUM_VOLUMES;
	tNum = tNum - 5;


	ide_devs_info[hdd_id].num_FREEs = 0;


	for( loop = 0; loop < tNum; loop++) // 실제 HDD 의 용량에 따라 루프 횟수를 결정.
	{
		pLVG = &ide_devs_info[hdd_id].tLVG[loop];
		memset( pLVG, 0, sizeof(LOGICAL_VOLUME_GROUP));
		pLVG->hdd_id = hdd_id;
		pLVG->start_offset = (loop + 5) * LVG_BLOCKS;
		pLVG->tLVH.volume_id	= loop;
		pLVG->tLVH.start_time = 0x7ffffffe;
		pLVG->tLVH.end_time	= 0x7fffffff;


		if ( free_LVG.LVG_ptr_head == NULL )
		{
// printf("1. pLVG->tLVH.volume_id ==%d\n", pLVG->tLVH.volume_id);
			free_LVG.LVG_ptr_head = pLVG;
			pLVG->prev_pLVG = NULL;
			pLVG->next_pLVG = NULL;
			free_LVG.LVG_ptr_tail = pLVG;
		}
		else
		{
// printf("2. pLVG->tLVH.volume_id ==%d\n", pLVG->tLVH.volume_id);
			free_LVG.LVG_ptr_tail->next_pLVG = pLVG;
			pLVG->prev_pLVG = free_LVG.LVG_ptr_tail;
			pLVG->next_pLVG = NULL;
			free_LVG.LVG_ptr_tail = pLVG;
		}

		memcpy( ptr, &pLVG->tLVH, sizeof(LOGICAL_VOLUME_HEADER));
		ptr[sizeof(LOGICAL_VOLUME_HEADER)] = make_crc( ptr, sizeof(LOGICAL_VOLUME_HEADER));

		ide_block_write( hdd_id, pLVG->start_offset, ptr, 1, 0);

		free_LVG.num_LVs++;
		ide_devs_info[hdd_id].num_FREEs++;

		probe_success++;
	}


	memset( ptr, 0, 512);
	ide_block_write( hdd_id, 0, ptr, 1, 0); // MBR clear

	memset( HDD_pHeader, 0, sizeof(NEW_HDD_HEADER));
	memcpy( HDD_pHeader->signature , tSig, 6);
	HDD_pHeader->num_bad_volumes = 0;
	HDD_pHeader->num_volumes = loop;
	HDD_pHeader->time_signature = 0x20090218;
	memcpy( ptr, HDD_pHeader, sizeof(NEW_HDD_HEADER));

	ptr[sizeof(NEW_HDD_HEADER)] = make_crc( ptr, sizeof(NEW_HDD_HEADER));
	ide_block_write( hdd_id, NEW_HDD_HEADER_OFFSET, ptr, 1, 0);
	ide_block_write( hdd_id, NEW_HDD_HEADER_BACKUP_OFFSET, ptr, 1, 0);

	ide_devs_info[hdd_id].num_LVs = probe_success;

/*
	memset( ptr, 0, 512);
	ide_block_write( hdd_id, HDD_MIRROR_INFO_OFFSET, ptr, 1, 1); // flush


	ide_devs_info[hdd_id].mirror_configured = 0;
	ide_devs_info[hdd_id].make_mirror = 0;
*/
// printf("ide_devs_info[%d].num_LVs ==>%d\n", hdd_id, ide_devs_info[hdd_id].num_LVs);

	return probe_success;

}	



void calc_hdd_free(void)
{
	int sub_total;
	int sub_free_total;


	sub_total = 0;
	sub_free_total = 0;

	if ( ide_devs_info[1].exist != 0 )
	{
		sub_total += ide_devs_info[1].num_LVs; // + 왜 필요해??? 바보
		sub_free_total += ide_devs_info[1].num_FREEs; // + 왜 필요해??? 바보
	}

	if ( sub_total == 0 )
	{
		gNewVar.free_blocks = 0;
		gNewVar.hdd_free_space = 0;
		strcpy( gNewVar.HDDMsg, "[NONE]  ");
		gNewVar.HDD_free_space_warning = 0;
	}
	else
	{
		if ( gNewVar.Record.repeat == 1 )
		{
			gNewVar.free_blocks = (long long int)(LVG_BLOCKS - gNewVar.Record.Rec_offset);
			gNewVar.total_blocks = (long long int)sub_total * LVG_BLOCKS;
			gNewVar.hdd_free_space = (int)( (gNewVar.free_blocks * 10000) / gNewVar.total_blocks );
			sprintf ( gNewVar.HDDMsg, "[%d.%02d%%] ", gNewVar.hdd_free_space/100, gNewVar.hdd_free_space%100 );
		}
		else
		{
			if ( data_LVG.LVG_ptr_head == data_LVG.LVG_ptr_tail && gNewVar.Record.Rec_offset == LVG_DATA_OFFSET)
			{
				gNewVar.free_blocks = (long long int)sub_total * LVG_BLOCKS;
				gNewVar.total_blocks = gNewVar.free_blocks;
				gNewVar.hdd_free_space = 10000;
				strcpy( gNewVar.HDDMsg, "[100%]  ");
			}
			else
			{
				gNewVar.free_blocks = (long long int)(sub_free_total + 1) * LVG_BLOCKS - (long long int)gNewVar.Record.Rec_offset;
				gNewVar.total_blocks = (long long int)sub_total * LVG_BLOCKS;
				gNewVar.hdd_free_space = (int)( (gNewVar.free_blocks * 10000) / gNewVar.total_blocks );
				sprintf ( gNewVar.HDDMsg, "[%d.%02d%%] ", gNewVar.hdd_free_space/100, gNewVar.hdd_free_space%100 );
			}

// printf("gNewVar.HDDMsg ==>%s\n", gNewVar.HDDMsg);
		}

		if ( gEnv.recinfo.repeat_rec == 0 )
		{
			if ( gNewVar.free_blocks < 10240 )
				gNewVar.HDD_free_space_warning = 2;
			else if ( gNewVar.hdd_free_space < (gEnv.recinfo.warning_msg + 1)*100 )
				gNewVar.HDD_free_space_warning = 1;
			else
				gNewVar.HDD_free_space_warning = 0;
		}
		else
			gNewVar.HDD_free_space_warning = 0;
	}
	
	gNewVar.SCREEN_REFRESH.hdd_msg = 1;
}





int configure_LogicalVolumeGroup( int hdd_id, NEW_HDD_HEADER* HDD_pHeader)
{
	DWORD super_blocks;
	LOGICAL_VOLUME_GROUP* pLVG;
	LOGICAL_VOLUME_HEADER *pLVH;
	BYTE ptr[512];
	BYTE pTemp[512];
	int loop;
	int probe_success;
	int result;
	time_t cur_time;
	int hashing_diff;
	DWORD* pOld_blocks;
	int tNum;
	int max_blocks;

	print_hdd_checking(hdd_id);

	tNum = ide_devs_info[hdd_id].sector_size >> 21;
	if ( tNum < 10 )
	{
		printf("[HDD] HDD size is not enough !!! \n");
		return 0;
	}

	if ( tNum > MAX_NUM_VOLUMES ) 
		tNum = MAX_NUM_VOLUMES;
	tNum = tNum - 5;

	max_blocks = ( tNum > HDD_pHeader->num_volumes ) ? HDD_pHeader->num_volumes : tNum;


	pLVH = (LOGICAL_VOLUME_HEADER*)ptr;
	probe_success = 0;
	ide_devs_info[hdd_id].num_FREEs = 0;

	memset( bad_volume_table, 0, MAX_NUM_VOLUMES); // 64byte 변수를 사용하면 한방에 처리가능 할 텐데......
	if ( HDD_pHeader->num_bad_volumes != 0 ) // 2007 12 10
	{
		printf("[HDD] Bad block counts : %d \n", HDD_pHeader->num_bad_volumes);
		result = ide_block_read( hdd_id, BAD_LVGS_OFFSET, (BYTE *)bad_volume_table, MAX_NUM_VOLUMES>>9);
		if ( result != 1 )
		{
			memset( bad_volume_table, 0, MAX_NUM_VOLUMES);
			printf("[HDD] Bad block read fail \n");
		}
	}
	else
		printf("[HDD] Bad block not found \n");





	for( loop = 0; loop < max_blocks; loop++)
	{
		memset( ptr, 0, 512);
		super_blocks = (loop + 5) * LVG_BLOCKS;

		pLVG = &ide_devs_info[hdd_id].tLVG[loop]; // HDD에서 각 Volume 읽어서 ide_devs_info에 집어 넣는다.
		pLVG->hdd_id = hdd_id;
		pLVG->start_offset = super_blocks;
		pLVG->tLVH.volume_id = loop;
		pLVG->tLVH.status = 3;


		if ( bad_volume_table[loop + 5] == 0x03 ) // 2007 12 10 skip bad volume
			continue;

		result = ide_block_read( hdd_id, super_blocks, (BYTE *)ptr, 1); // 이렇게 함으로서 pLVH값이 대입된다.
		if ( result != 1 )
			continue;


		if ( check_crc( (BYTE *)ptr, sizeof(LOGICAL_VOLUME_HEADER)) != 0 )
		{
			if ( pLVH->event_count == 0 )
				;//Debugging.
			else if ( pLVH->event_count > 0 && pLVH->event_count < (LVG_EVENTINDEX_BLOCKS<<9) )
				; // printf("Imported Eventlist is %d for %03dth block \n", pLVH->event_count, gNewVar.num_LVs);
			else
				pLVH->event_count = 65532; // (LVG_EVENTINDEX_BLOCKS<<9 - 4)


			pLVG->event_former_record_length = pLVH->event_count;
			pLVG->event_offset_start = NULL;
			pLVG->event_offset_end = NULL;






// LV_STATUS_EMPTY
			if (pLVH->status == 0)
			{
				pLVH->start_time = 0x7ffffffe;
				pLVH->end_time   = 0x7fffffff;

				if ( free_LVG.LVG_ptr_head == NULL )
				{
					free_LVG.LVG_ptr_head = pLVG;
					pLVG->prev_pLVG = NULL;
					pLVG->next_pLVG = NULL;
					free_LVG.LVG_ptr_tail = pLVG;
				}
				else
				{
					free_LVG.LVG_ptr_tail->next_pLVG = pLVG;
					pLVG->prev_pLVG = free_LVG.LVG_ptr_tail;
					pLVG->next_pLVG = NULL;
					free_LVG.LVG_ptr_tail = pLVG;
				}

				free_LVG.num_LVs++;
				ide_devs_info[hdd_id].num_FREEs++;
			}

//  LV_STATUS_RECORDING
			else if ( pLVH->status == 1 )
			{
				printf("[HDD /dev/%s] I found last termination point %d - %d \n", make_device_string(hdd_id), loop,  pLVH->new_offset);
				result = ide_block_read( pLVG->hdd_id, pLVG->start_offset + LVG_TIMEINDEX_OFFSET, (BYTE *)gNewVar.Record.pTimeIndex, LVG_TIMEINDEX_BLOCKS);

				if ( result == 1 )
				{
					result = ide_block_read( pLVG->hdd_id, pLVG->start_offset + pLVH->new_offset, pTemp, 1);

					if ( result != 1 )
						goto skip_this_volume;
				}
				else
					goto skip_this_volume;


				if ( pLVH->start_time == ((NEW_GROUP_HEADER*)pTemp)->lvg_time && check_crc( pTemp, 511) == 1 )
					printf("[HDD] Verifying... success \n");
				else
				{
					int fail_count = 0;
					printf("[HDD] Verifying... NG \n");

					cur_time = pLVH->end_time;

					while (1)
					{
						hashing_diff = (int)cur_time - (int)pLVH->start_time;
						if ( hashing_diff < 0 )
						{
							pLVH->new_offset = LVG_DATA_OFFSET;
							pLVH->last_offset = LVG_DATA_OFFSET; // 일부러 freeblock 되게끔 함. (값을 같게 함)
							printf("[HDD] Recording from the first block after %d NGs\n", fail_count);
							// 실제 이러한 블럭이 있으면 free block에 추가해야함
							break;
						}

						pOld_blocks = (DWORD *)( gNewVar.Record.pTimeIndex + (hashing_diff * HASHING_INFO_SIZE) );
						if ( *pOld_blocks == 0 )
						{
							cur_time--;
							continue;
						}
						else
						{
							DWORD read_offset;

							printf("[HDD] Adjusting 1 second... \n");
							read_offset = pLVG->start_offset + *pOld_blocks;
							result = ide_block_read( pLVG->hdd_id, read_offset, pTemp, 1);
							if ( result != 1 )
								goto skip_this_volume;

							if ( pLVH->start_time == ((NEW_GROUP_HEADER*)pTemp)->lvg_time && check_crc( pTemp, 511) == 1 )
							{
								printf("[HDD] Verifying OK after %d NGs ==> ID : %d\n", fail_count, pLVH->start_time);
								do {
									printf("[HDD] Scanned Offset : %d\n", ((NEW_GROUP_HEADER*)pTemp)->current_offset);

									pLVH->new_offset = ((NEW_GROUP_HEADER*)pTemp)->current_offset;
									pLVH->last_offset = ((NEW_GROUP_HEADER*)pTemp)->prev_offset;
	
									read_offset = pLVG->start_offset + ((NEW_GROUP_HEADER*)pTemp)->next_offset;
									result = ide_block_read( pLVG->hdd_id, read_offset, pTemp, 1);
									if ( result != 1 )
										goto skip_this_volume;
								} while ( pLVH->start_time == ((NEW_GROUP_HEADER*)pTemp)->lvg_time && check_crc( pTemp, 511) == 1 );

								pLVH->end_time = cur_time;
								ide_block_write( pLVG->hdd_id, pLVG->start_offset + LVG_TIMEINDEX_OFFSET, gNewVar.Record.pTimeIndex, LVG_TIMEINDEX_BLOCKS, 0);

								break;
							}
							else
							{
								printf("[HDD] fail \n");
								fail_count++;
								*pOld_blocks = 0;
								*(pOld_blocks + 1)= 0;
								cur_time--;
								continue;
							}
						}
					} // while
				}


				if ( pLVH->new_offset > pLVH->last_offset ) // 당연?
				{
					pLVH->status = 2; // 이 함수를 나가면 이것을 다시 1로 고친다.
					pLVH->last_offset = pLVH->new_offset;
					pLVH->new_offset = LVG_DATA_OFFSET; // 이 함수를 나가서 다른 작업을 하기 위해 offset 값을 default로 처리함.

					ptr[sizeof(LOGICAL_VOLUME_HEADER)] = make_crc( ptr, sizeof(LOGICAL_VOLUME_HEADER));
					ide_block_write( pLVG->hdd_id, super_blocks, (BYTE *)ptr, 1, 0); // == pLVH

					gLVG[data_LVG.num_LVs] = &ide_devs_info[hdd_id].tLVG[loop];
					data_LVG.num_LVs++;
				}
				else
				{
					pLVH->status = 0; // 이상블럭. free block 으로 추가

					pLVH->start_time = 0x7ffffffe;
					pLVH->end_time	= 0x7fffffff;
					
					//ide_devs_info[hdd_id].tLVG[probe_success].event_offset_start= NULL;
					//ide_devs_info[hdd_id].tLVG[probe_success].event_offset_end	= NULL;
					if ( free_LVG.LVG_ptr_head == NULL )
					{
						free_LVG.LVG_ptr_head = pLVG;
						pLVG->prev_pLVG = NULL;
						pLVG->next_pLVG = NULL;
						free_LVG.LVG_ptr_tail = pLVG;
					}
					else
					{
						free_LVG.LVG_ptr_tail->next_pLVG = pLVG;
						pLVG->prev_pLVG = free_LVG.LVG_ptr_tail;
						pLVG->next_pLVG = NULL;
						free_LVG.LVG_ptr_tail = pLVG;
					}

					ptr[sizeof(LOGICAL_VOLUME_HEADER)] = make_crc( ptr, sizeof(LOGICAL_VOLUME_HEADER));
					ide_block_write( pLVG->hdd_id, super_blocks, ptr, 1, 0);

					free_LVG.num_LVs++;
					ide_devs_info[hdd_id].num_FREEs++;
				}
			}

// LV_STATUS_FULL
			else if ( pLVH->status == 2 )
			{
				gLVG[data_LVG.num_LVs] = &ide_devs_info[hdd_id].tLVG[loop];
				data_LVG.num_LVs++;
			}
// LV_ELSE
			else
			{
				pLVH->status = 3; // TODO : free block 처리 할 것.
			}


			memcpy( &pLVG->tLVH, pLVH, sizeof(LOGICAL_VOLUME_HEADER));
			probe_success++;

			skip_this_volume:

			continue; // kimyw 090217

		}
		else
		{
			printf("[HDD] %3d Volume : CRC Fail \n", loop);
		}
	}



	printf("[HDD] %d Volumes found...\n", probe_success);
	ide_devs_info[hdd_id].num_LVs = probe_success;


/*
	if ( ide_devs_info[hdd_id].mirror_configured == 1 )
	{
		memset( ptr, 0, 512);
		ide_block_write( hdd_id, HDD_MIRROR_INFO_OFFSET, ptr, 1, 0);
	}
	ide_devs_info[hdd_id].mirror_configured = 0;
	ide_devs_info[hdd_id].make_mirror = 0;	
*/

	return probe_success;
}




int ide_block_write( BYTE hddid, DWORD offset, BYTE* buf, DWORD size, BYTE sync)
{
	UINT64 result;
	UINT64 offset64;
	int error;
	int count;
	
	count = 0;

	offset64 = (UINT64)offset<<9;

//	error = _llseek( ide_devs_info[hddid].ide_fd, offset >> 23, (offset & 0x007fffff) << 9, &result, SEEK_SET);
	error = syscall(SYS__llseek, ide_devs_info[hddid].ide_fd, offset >> 23,  (offset & 0x007fffff) << 9, &result, SEEK_SET);



	if(error)
	{
		printf("ide_block_write seek error to write!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		return -1;
	}

	if(result != offset64)
	{
		printf("ide_block_write result %lld\n",result);
		return -1;
	}

	size = size<<9;
	count = write( (int)ide_devs_info[hddid].ide_fd, buf, size);

	if(count == size)
	{
		if(sync == TRUE)
		{
			fsync(ide_devs_info[hddid].ide_fd);
			if (ioctl(ide_devs_info[hddid].ide_fd, BLKFLSBUF, NULL))		/* do it again, big time */
				perror("BLKFLSBUF failed");
			if (ioctl(ide_devs_info[hddid].ide_fd, HDIO_DRIVE_CMD, NULL) && errno != EINVAL) /* await completion */
				perror("HDIO_DRIVE_CMD(null) (wait for flush complete) failed");
		}
	}
	else
	{
		DWORD index;

		hdd_error_handle(hddid);
		printf("All data NOT successfully written.\n");

	#if 1 // 2007 12 10 READ ERROR 인 경우 BAD BLOCK 처리
		index = offset>>21;
		if ( index >= 5 )
		{
			ide_devs_info[hddid].tLVG[index - 5].tLVH.status = 0x03; // 5GB reserved for NON-DATA
		}
		
		memset( bad_volume_table, 0, MAX_NUM_VOLUMES);
		if ( ide_devs_info[hddid].hdd_header.num_bad_volumes != 0 )
		{
			result = ide_block_read( hddid, BAD_LVGS_OFFSET, (BYTE *)bad_volume_table, MAX_NUM_VOLUMES>>9);
			if ( result != 1 )
			{
				memset( bad_volume_table, 0, MAX_NUM_VOLUMES);
			}
		}
		
		if ( bad_volume_table[index] != 0x03 )
		{
			BYTE ptr[512];
			
			bad_volume_table[index] = 0x03;
			ide_devs_info[hddid].hdd_header.num_bad_volumes++;
			memset( ptr, 0, 512);
			memcpy( ptr, &ide_devs_info[hddid].hdd_header, sizeof(NEW_HDD_HEADER));
			ptr[sizeof(NEW_HDD_HEADER)] = make_crc( ptr, sizeof(NEW_HDD_HEADER));
			ide_block_write( hddid, NEW_HDD_HEADER_OFFSET, ptr, 1, 0);
			ide_block_write( hddid, NEW_HDD_HEADER_BACKUP_OFFSET, ptr, 1, 0);
			ide_block_write( hddid, BAD_LVGS_OFFSET, (BYTE *)bad_volume_table, MAX_NUM_VOLUMES>>9, 1);
			printf("[DEBUG] Bad Volume Table updated : %ld \n", index);
		}
	#endif
		return -1;
	}	
	
	return 1;
}
int ide_read(BYTE hddid,UINT64 offset,BYTE * buf,DWORD size)
{
	UINT64 result,result2;
	//UINT64 offset64;
	int error;

	// error = _llseek (ide_devs_info[hddid].ide_fd, ((UINT64) offset) >> 32, ((UINT64) offset) & 0xffffffff, &result, SEEK_SET);
	error = syscall(SYS__llseek, ide_devs_info[hddid].ide_fd, ((UINT64) offset) >> 32,  ((UINT64) offset) & 0xffffffff, &result, SEEK_SET);

	if(error)
	{
		printd("seek error\n");	
		return -2;
	}		

	result2 = read((int)ide_devs_info[hddid].ide_fd,buf,size);		

	if(result2 != size) 
	{
		printd("All data NOT successfully read.\n");
		return -1;
	}

	return 1;
}

int ide_block_read( BYTE hddid, DWORD offset, BYTE* buf, DWORD size)
{
	UINT64 result=0;
	int error=0;
	int count=0;


//	error = _llseek( ide_devs_info[hddid].ide_fd, offset >> 23, (offset & 0x007fffff) << 9, &result, SEEK_SET);
	error = syscall(SYS__llseek, ide_devs_info[hddid].ide_fd, offset >> 23,  (offset & 0x007fffff) << 9, &result, SEEK_SET);

	if(error)
	{
		printf("seek error to read\n");	
		return -2;
	}

	size = size<<9;
	count = read( (int)ide_devs_info[hddid].ide_fd, buf, size);

// printf("buf ==>%s\n", buf);


	if(count != size) 
	{
		DWORD index;

		hdd_error_handle(hddid);
		printf("All data NOT successfully read.\n");
		
	#if 1 // 2007 12 10 READ ERROR 인 경우 BAD BLOCK 처리
		index = offset>>21;
		if ( index >= 5 )
		{
			ide_devs_info[hddid].tLVG[index - 5].tLVH.status = 0x03; // 5GB reserved for NON-DATA
		}
		
		memset( bad_volume_table, 0, MAX_NUM_VOLUMES);
		if ( ide_devs_info[hddid].hdd_header.num_bad_volumes != 0 )
		{
			result = ide_block_read( hddid, BAD_LVGS_OFFSET, (BYTE *)bad_volume_table, MAX_NUM_VOLUMES>>9);
			if ( result != 1 )
			{
				memset( bad_volume_table, 0, MAX_NUM_VOLUMES);
			}
		}

		if ( bad_volume_table[index] != 0x03 )
		{
			BYTE ptr[512];
			
			bad_volume_table[index] = 0x03;
			ide_devs_info[hddid].hdd_header.num_bad_volumes++;
			memset( ptr, 0, 512);
			memcpy( ptr, &ide_devs_info[hddid].hdd_header, sizeof(NEW_HDD_HEADER));
			ptr[sizeof(NEW_HDD_HEADER)] = make_crc( ptr, sizeof(NEW_HDD_HEADER));
			ide_block_write( hddid, NEW_HDD_HEADER_OFFSET, ptr, 1, 0);
			ide_block_write( hddid, NEW_HDD_HEADER_BACKUP_OFFSET, ptr, 1, 0);
			ide_block_write( hddid, BAD_LVGS_OFFSET, (BYTE *)bad_volume_table, MAX_NUM_VOLUMES>>9, 1);
			printf("[DEBUG] Bad Volume Table updated : %ld \n", index);
		}
	#endif
		return -1;
	}


	return 1;
}


#define BLOCK_TEST_SIZE (16*1024)
int ide_block_scan( BYTE hddid)
{
	UINT64 result;
	int error;
	int count;
	DWORD offset;
	DWORD size;
	//BYTE buf[BLOCK_TEST_SIZE];
	DWORD loop;
	DWORD scan_range;
	DWORD temp1=0, temp2=10;
//	int aa=0;


	pthread_mutex_lock(&ide_devs_info[ hddid ].ide_access_wait);

	scan_range = ide_devs_info[hddid].sector_size - BLOCK_TEST_SIZE;


	calc_each_disk(hddid);


	for ( loop = 0; loop < scan_range; loop += BLOCK_TEST_SIZE) // 0xffffffff
	{
		offset = loop;
//		error = _llseek( ide_devs_info[hddid].ide_fd, offset >> 23, (offset & 0x007fffff) << 9, &result, SEEK_SET);
		error = syscall(SYS__llseek, ide_devs_info[hddid].ide_fd, offset >> 23,  (offset & 0x007fffff) << 9, &result, SEEK_SET);

		if(error)
		{
			printd("[DEBUG] SEEK ERROR \n");	
			continue;
		}
	
		size = (BLOCK_TEST_SIZE<<9);
		count = read( (int)ide_devs_info[hddid].ide_fd, MP_Buffer, size);		
	
		if(count != size) 
		{
			DWORD index;
			printd("[DEBUG] BLOCK SCAN ERROR %dGB - %d\n", offset>>21, offset&(0x01ff));
			
		#if 1 // 2007 12 10 READ ERROR 인 경우 BAD BLOCK 처리
			index = offset>>21;
			if ( index >= 5 )
			{
				ide_devs_info[hddid].tLVG[index - 5].tLVH.status = 0x03; // 5GB reserved for NON-DATA
				printf("[DEBUG] ERROR BLOCK IN DATA AREA \n");
			}
			else
			{
				printf("[DEBUG] ERROR BLOCK IN NON-DATA AREA \n");
			}
			
			memset( bad_volume_table, 0, MAX_NUM_VOLUMES);
			if ( ide_devs_info[hddid].hdd_header.num_bad_volumes != 0 )
			{
				result = ide_block_read( hddid, BAD_LVGS_OFFSET, (BYTE *)bad_volume_table, MAX_NUM_VOLUMES>>9);
				if ( result != 1 )
				{
					memset( bad_volume_table, 0, MAX_NUM_VOLUMES);
				}
			}
			
			if ( bad_volume_table[index] != 0x03 )
			{
				BYTE ptr[512];
				
				bad_volume_table[index] = 0x03;
				ide_devs_info[hddid].hdd_header.num_bad_volumes++;
				memset( ptr, 0, 512);
				memcpy( ptr, &ide_devs_info[hddid].hdd_header, sizeof(NEW_HDD_HEADER));
				ptr[sizeof(NEW_HDD_HEADER)] = make_crc( ptr, sizeof(NEW_HDD_HEADER));
				ide_block_write( hddid, NEW_HDD_HEADER_OFFSET, ptr, 1, 0);
				ide_block_write( hddid, NEW_HDD_HEADER_BACKUP_OFFSET, ptr, 1, 0);
				ide_block_write( hddid, BAD_LVGS_OFFSET, (BYTE *)bad_volume_table, MAX_NUM_VOLUMES>>9, 1);
				printd("[DEBUG] Bad Volume Table updated : %d \n", index);
			}
		#endif
			loop = ( (index + 1)<<9 ) - 1;
		}

		temp1 = (DWORD)(((unsigned long long)loop * 100)/(unsigned long long)ide_devs_info[hddid].sector_size);

		if( temp1 != temp2 )
		{
//			Draw_scanning_table(hddid, temp1, 0);
//			Draw_scanning_table(hddid, temp1, 1);
		}

		temp2 =  temp1;
		
		// percentage = ( (unsigned long long)loop * 100 )/(unsigned long long)ide_devs_info[hdd_id].sector_size;
		
		// Check input. if ESC, just "return 0";
	}

	pthread_mutex_unlock(&ide_devs_info[ hddid ].ide_access_wait);
	return 1;
}


void* pop_free_LVG(void)
{
	void* temp;
	
	temp = free_LVG.LVG_ptr_head;

	if ( free_LVG.LVG_ptr_head == free_LVG.LVG_ptr_tail )
	{
		free_LVG.LVG_ptr_head = NULL;
		free_LVG.LVG_ptr_tail = NULL;
	}
	else
	{
		free_LVG.LVG_ptr_head = free_LVG.LVG_ptr_head->next_pLVG;
		free_LVG.LVG_ptr_head->prev_pLVG = NULL;
	}

	free_LVG.num_LVs--;
	ide_devs_info[((LOGICAL_VOLUME_GROUP*)temp)->hdd_id].num_FREEs--;
	data_LVG.num_LVs++;

// printf("pop_free_LVG() : data_LVG.num_LVs ==>%d\n", data_LVG.num_LVs);

	return temp;
}


LOGICAL_VOLUME_GROUP* find_proper_position( time_t target_time)
{
	LOGICAL_VOLUME_GROUP* pLVG;
	
	pLVG = data_LVG.LVG_ptr_tail;

	while(1)
	{
		if ( pLVG->tLVH.start_time < target_time )
			break;

		if ( pLVG->prev_pLVG == NULL )
			return NULL;

		pLVG = pLVG->prev_pLVG;
	}

	return pLVG;
}


void insert_freeblock_to_dataLGV(LOGICAL_VOLUME_GROUP* target_ptr, time_t target_time)
{
	LOGICAL_VOLUME_GROUP* pLVG;

	pLVG = find_proper_position(target_time);

	if ( pLVG == NULL )
	{
		data_LVG.LVG_ptr_head->prev_pLVG = target_ptr;
		target_ptr->prev_pLVG = NULL;
		target_ptr->next_pLVG = data_LVG.LVG_ptr_head;
		data_LVG.LVG_ptr_head = target_ptr;
	}
	else if ( pLVG == data_LVG.LVG_ptr_tail )
	{
		data_LVG.LVG_ptr_tail->next_pLVG = target_ptr;
		target_ptr->prev_pLVG = data_LVG.LVG_ptr_tail;
		target_ptr->next_pLVG = NULL;
		data_LVG.LVG_ptr_tail = target_ptr;
	}
	else
	{
		pLVG->next_pLVG->prev_pLVG = target_ptr;
		target_ptr->next_pLVG = pLVG->next_pLVG;
		target_ptr->prev_pLVG = pLVG;
		pLVG->next_pLVG = target_ptr;
	}
}


void print_hdd_checking(int number)
{
	char disp_str[128];

// printf("number ==>%d\n", number);

	if ( at_boot_time != 0 && num_disks != 0 )
	{
		if ( number == -1 )
		{
			seq_disks = 0;
			// printf("Checking HDD ...... [NONE]\n");
		}
		else if ( number < 4 )
		{
			seq_disks++;
			sprintf( disp_str, "%d%% Checking HDD%d...", (((UNS32)seq_disks)*100)/num_disks, number );
			// printf("%s\n", disp_str);
		}
		else
		{
			seq_disks++;
			sprintf( disp_str, "%d%% Checking SATA HDD %d-%d...", (((UNS32)seq_disks)*100)/num_disks, number/4, (number%4)+1);
		}
	}
}


void hdd_error_handle(int hddid)
{
	if ( hddid < 4 ) // MAX_IDE_NUM
	{
		// 20061215 HDD IO ERROR
		sprintf( gNewVar.WarningMsg, "        HDD%d ERROR        ", hddid + 1); // flag 추가할 것.
		gNewVar.SCREEN_REFRESH.warning_msg = 0;
		gNewVar.WarningTimeout = 5 * EVENT_COUNTDOWN_FACTOR;
		if ( gNewVar.ERROR_HDD_ID == hddid )
		{
			// skip
		}
		else
		{
//			add_logfile(LOGFILE_HDD_ERROR,gNewVar.current_time, 0, 0);
		}
		// 20061215 HDD IO ERROR

/*		
		D_IO_output( 0, 1);
		D_IO_output( 1, 1);
		D_IO_output( 2, 1);
*/
		gNewVar.ERROR_HDD_ID = hddid;
	}
	else
	{
		if ( gNewVar.ERROR_HDD_ID == hddid )
		{
			// skip
		}
		else
		{
			printd("[HDD %d] Ext. HDD Error recorded.. \n", hddid);
//			add_logfile(LOGFILE_EXT_HDD_ERROR,gNewVar.current_time, 0, 0);
		}

/*
		D_IO_output( 0, 1);
		D_IO_output( 1, 1);
		D_IO_output( 2, 1);
*/
		gNewVar.ERROR_HDD_ID = hddid;
		gNewVar.SCREEN_REFRESH.warning_msg = 0;
		gNewVar.HDD_IO_ERROR = 1;
	}

	ide_devs_info[hddid].error_count++;
	if ( ide_devs_info[hddid].error_count > 5 )
	{
		if ( hddid < 4 ) // MAX_IDE_NUM
		{
			//ide_devs_info[hddid].num_LVs = 0; // 2007 12 10 내장 HDD 에 대해서는 IO ERROR가 
			//ide_devs_info[hddid].exist = 0;   // 5 회 이상 발생해도 NONE 처리하지 않음
		}
		else
		{
			int temp;
			
			temp = (hddid>>2)<<2;
			ide_devs_info[ temp + 0 ].num_LVs = 0;
			ide_devs_info[ temp + 0 ].exist = 0;
			ide_devs_info[ temp + 1 ].num_LVs = 0;
			ide_devs_info[ temp + 1 ].exist = 0;
			ide_devs_info[ temp + 2 ].num_LVs = 0;
			ide_devs_info[ temp + 2 ].exist = 0;
			ide_devs_info[ temp + 3 ].num_LVs = 0;
			ide_devs_info[ temp + 3 ].exist = 0;
		}
	}
}


LOGICAL_VOLUME_GROUP *disk_mirror[MAX_NUM_VOLUMES];

int calc_each_disk(int hdd_id)
{
	int loop;
	int data_count;
	int free_count;

	/*
	if ( ide_devs_info[hdd_id].mirror_configured == 1 && hdd_id > 1 )
	{
		ide_devs_info[hdd_id].total_blocks = 0;
		ide_devs_info[hdd_id].free_blocks = 0;
		ide_devs_info[hdd_id].rec_start = 0;
		ide_devs_info[hdd_id].rec_end = 0;
		
		return 0;
	}
	*/
	data_count = 0;
	free_count = 0;

	if ( ide_devs_info[hdd_id].exist != 0 && ide_devs_info[hdd_id].num_LVs > 0 && hdd_id < MAX_HDD_NUM )
	{
		for ( loop = 0; loop < ide_devs_info[hdd_id].hdd_header.num_volumes; loop++ )
		{
			switch ( ide_devs_info[hdd_id].tLVG[loop].tLVH.status )
			{
			case 0 :
				free_count++;
				break;
			case 1 :
			case 2 :
				disk_mirror[data_count] = &ide_devs_info[hdd_id].tLVG[loop];
				data_count++;
				break;
			case 3 :
			default:
				break;
			}
		}
		
		printf("[HDD] %dth - MODEL: %s SERIAL:%s SIZE: %d KB \n", hdd_id, ide_devs_info[hdd_id].info.model, ide_devs_info[hdd_id].info.serial_no, ide_devs_info[hdd_id].sector_size>>1);
		if ( data_count > 0 )
		{
			qsort( (void*)disk_mirror, data_count, sizeof(LOGICAL_VOLUME_GROUP*), &LVG_sort);
			ide_devs_info[hdd_id].total_blocks = (DWORD)( (data_count + free_count) * LVG_BLOCKS)>>11;

			if ( disk_mirror[data_count - 1]->tLVH.status == 2 )
			{
				ide_devs_info[hdd_id].free_blocks = 0;
			}
			else if ( disk_mirror[data_count - 1]->tLVH.status == 1 )
			{
				ide_devs_info[hdd_id].free_blocks = ( (DWORD)(free_count + 1) * LVG_BLOCKS - disk_mirror[data_count - 1]->tLVH.new_offset )>>11;
			}
			else
			{
				ide_devs_info[hdd_id].free_blocks = ide_devs_info[hdd_id].total_blocks;
				printd("[HDD] Really? It can't be...\n");
			}
			printd("[HDD] total : %d MB , remain : %d MB \n", ide_devs_info[hdd_id].total_blocks, ide_devs_info[hdd_id].free_blocks);

			ide_devs_info[hdd_id].rec_start = disk_mirror[0]->tLVH.start_time;
			printd("[HDD] REC START : %d :", ide_devs_info[hdd_id].rec_start);
//			clock_debug_message(ide_devs_info[hdd_id].rec_start);
			ide_devs_info[hdd_id].rec_end = disk_mirror[data_count - 1]->tLVH.end_time;
			printd("[HDD] REC END   : %d :", ide_devs_info[hdd_id].rec_end);
//			clock_debug_message(ide_devs_info[hdd_id].rec_end);
		}
		else
		{
			ide_devs_info[hdd_id].total_blocks = (DWORD)(free_count * LVG_BLOCKS)>>11;
			ide_devs_info[hdd_id].free_blocks = ide_devs_info[hdd_id].total_blocks;
			printd("[HDD] total : %d MB , remain : %d MB \n", ide_devs_info[hdd_id].total_blocks, ide_devs_info[hdd_id].free_blocks);
			printd("[HDD] NO REC DATA\n");
			ide_devs_info[hdd_id].rec_start = 0; // time_t
			ide_devs_info[hdd_id].rec_end = 0;
		}

		return 1;
	}
	else
	{
		//ide_devs_info[hdd_id].exist // 1: 있음 0:없음
		//ide_devs_info[hdd_id].format // 1: 포맷 0: 포맷하지않음
		
		//ide_devs_info[hdd_id].info.model // 문자열 40byte
		ide_devs_info[hdd_id].total_blocks = 0; // DWORD
		ide_devs_info[hdd_id].free_blocks = 0; // DWORD
		ide_devs_info[hdd_id].rec_start = 0; // time_t
		ide_devs_info[hdd_id].rec_end = 0; // time_t
		
		return 0;
	}
}

int LVG_sort( const void * mFormer, const void * mLatter)
{
	// LOGICAL_VOLUME_GROUP *[]이니까 **와 동일하다. 
	// mFormer, mLatter 모두 void * 타입이니까 (LOGICAL_VOLUME_GROUP **) cast가 필요한 것이다. 
	// 또한 tLVH.start_time은 time_t형 즉, 포인터가 아니기 때문에 
	// (LOGICAL_VOLUME_GROUP **)앞에 *를 붙여 주소가 아닌 값을 넣어주도록 한다. 

	time_t tFirst = (*(LOGICAL_VOLUME_GROUP **)mFormer)->tLVH.start_time; 
	time_t tSecond = (*(LOGICAL_VOLUME_GROUP **)mLatter)->tLVH.start_time;

	if ( tFirst == tSecond )
		return 0;
	else if ( tFirst < tSecond )
		return -1;
	else
		return 1;
}

void read_mirror_info(int hdd_id)
{
	int result;
	DWORD tSig = 'M'; // 고유한 string 을 집어너놓고 매 부팅시마다 확인?
	BYTE ptr[512];
	MIRROR_INFO_HEADER* pMIH;

	if ( hdd_id < MAX_IDE_NUM )
	{
		result = ide_block_read( hdd_id, HDD_MIRROR_INFO_OFFSET, (BYTE *)ptr, 1);
		if ( result == 1 )
		{
			pMIH = (MIRROR_INFO_HEADER*)ptr;
			if ( pMIH->mirror_signature1 == tSig && pMIH->mirror_signature1 == ~pMIH->mirror_signature2)
			{
				ide_devs_info[hdd_id].mirror_configured = 1;
				ide_devs_info[hdd_id].mirrored_time_signature = pMIH->mirrored_time_signature;
				printd("[HDD /dev/%s] MIRROR INFO FOUND! \n", make_device_string(hdd_id) );
				return;
			}
		}
	}
	ide_devs_info[hdd_id].mirror_configured = 0;
	//printf("[HDD /dev/%s] MIRROR INFO NOT FOUND. \n", make_device_string(hdd_id) );
}



void flush_time_index(void)
{
	LOGICAL_VOLUME_GROUP* pLVG;
	LOGICAL_VOLUME_HEADER* pLVH;
	BYTE ptr[512];
	unsigned event_length;
	DWORD write_offset;
	int write_ok = 0;
	int result;
	
	if ( gNewVar.Record.record_ready == ON )
	{
		result = 1;

		pLVG = gNewVar.Record.LVG_ptr;
		pLVH = (LOGICAL_VOLUME_HEADER*)&pLVG->tLVH;

		if ( gNewVar.Record.forced_flush == 1 )
		{
			memset( ptr, 0, 512);
			memcpy( ptr, pLVH, sizeof(LOGICAL_VOLUME_HEADER));
			ptr[sizeof(LOGICAL_VOLUME_HEADER)] = make_crc( ptr, sizeof(LOGICAL_VOLUME_HEADER));

			result = ide_block_write( pLVG->hdd_id, pLVG->start_offset + LVG_TIMEINDEX_OFFSET, (BYTE*)gNewVar.Record.pTimeIndex, LVG_TIMEINDEX_BLOCKS, 0);
			if ( result > 0 )
				result = ide_block_write( pLVG->hdd_id, pLVG->start_offset + LVG_EVENTINDEX_OFFSET, (BYTE*)gNewVar.Record.pEventIndex, LVG_EVENTINDEX_BLOCKS, 0);
			if ( result > 0 )
				result = ide_block_write( pLVG->hdd_id, pLVG->start_offset, ptr, 1, 1);
		}
		else // 0
		{
			if ( gNewVar.Record.index_dirty == 1 )
			{
				write_offset = gNewVar.Record.last_modified>>9;
				result = ide_block_write( pLVG->hdd_id, pLVG->start_offset + LVG_TIMEINDEX_OFFSET + write_offset, gNewVar.Record.pTimeIndex + (write_offset<<9), 1, 0);
				#if defined CODENAME_SVR16X0 || defined CODENAME_SVR164X || defined CODENAME_SVR9X0
				if ( ide_devs_info[pLVG->hdd_id].mirror_configured == 1 )
				{
					ide_block_write( ide_devs_info[pLVG->hdd_id].pair_id, pLVG->start_offset + LVG_TIMEINDEX_OFFSET + write_offset, gNewVar.Record.pTimeIndex + (write_offset<<9), 1, 0);
				}
				#endif
				write_ok = 1;
			}
			if ( gNewVar.Event.index_dirty == 1 )
			{
				event_length = 0;
				if ( pLVG->event_offset_start != pLVG->event_offset_end ) // event index 가 매번 클리어 되었는지 확인..
				{
					if ( pLVG->event_offset_start > pLVG->event_offset_end )
					{
						unsigned buffer_length1;
						unsigned buffer_length2;
						
						buffer_length1 = (unsigned)gNewVar.Event.buffer_addr_end - (unsigned)pLVG->event_offset_start;
						buffer_length2 = (unsigned)pLVG->event_offset_end - (unsigned)gNewVar.Event.buffer_addr_start;
						event_length = buffer_length1 + buffer_length2;
						pLVH->event_count = event_length + pLVG->event_former_record_length;
						if ( pLVH->event_count <= LVG_EVENTINDEX_BLOCKS<<9 )
						{
							memcpy( (void*)(gNewVar.Record.pEventIndex + pLVG->event_former_record_length), pLVG->event_offset_start, buffer_length1);
							memcpy( gNewVar.Record.pEventIndex + pLVG->event_former_record_length + buffer_length1, gNewVar.Event.buffer_addr_start, buffer_length2);
						}
						else
						{
							pLVH->event_count = 65532; // (LVG_EVENTINDEX_BLOCKS<<9 - 4)
						}
						
					}
					else
					{
						event_length = (unsigned)pLVG->event_offset_end - (unsigned)pLVG->event_offset_start;
						pLVH->event_count = event_length + pLVG->event_former_record_length;
						if ( pLVH->event_count <= LVG_EVENTINDEX_BLOCKS<<9 )
						{
							memcpy( gNewVar.Record.pEventIndex + pLVG->event_former_record_length, pLVG->event_offset_start, event_length);
						}
						else
						{
							pLVH->event_count = 65532; // (LVG_EVENTINDEX_BLOCKS<<9 - 4)
						}
					}
					
					
					//if (gNewVar.Event.index_dirty == 1)
					//{
						//printd("[EVENT] S:%d E:%d L:%d \n", (unsigned)pLVH->event_offset_start, (unsigned)pLVH->event_offset_end, pLVH->event_count);
					//}
				}
				
				if ( result > 0 )
					result = ide_block_write( pLVG->hdd_id, pLVG->start_offset + LVG_EVENTINDEX_OFFSET, gNewVar.Record.pEventIndex, LVG_EVENTINDEX_BLOCKS, 0);
				#if defined CODENAME_SVR16X0 || defined CODENAME_SVR164X || defined CODENAME_SVR9X0
				if ( ide_devs_info[pLVG->hdd_id].mirror_configured == 1 )
				{
					ide_block_write( ide_devs_info[pLVG->hdd_id].pair_id, pLVG->start_offset + LVG_EVENTINDEX_OFFSET, gNewVar.Record.pEventIndex, LVG_EVENTINDEX_BLOCKS, 0);
				}
				#endif
				write_ok = 1;
			}
			
			if ( write_ok == 1 )
			{
				memset( ptr, 0, 512);
				memcpy( ptr, pLVH, sizeof(LOGICAL_VOLUME_HEADER));
				ptr[sizeof(LOGICAL_VOLUME_HEADER)] = make_crc( ptr, sizeof(LOGICAL_VOLUME_HEADER));
		
				if ( result > 0 )
					result = ide_block_write( pLVG->hdd_id, pLVG->start_offset, ptr, 1, 0);
				#if defined CODENAME_SVR16X0 || defined CODENAME_SVR164X || defined CODENAME_SVR9X0
				if ( ide_devs_info[pLVG->hdd_id].mirror_configured == 1 )
				{
					ide_block_write( ide_devs_info[pLVG->hdd_id].pair_id, pLVG->start_offset, ptr, 1, 0);
				}
				#endif
				//printd("======================\n");
				//calc_hdd_free();
			}
		}
		
		if ( result <= 0 )
		{
			// EXT HDD ERROR
			forced_head_change = 1;	
		}

	}
	gNewVar.Event.index_dirty = 0;
	gNewVar.Record.index_dirty = 0;
	gNewVar.Record.forced_flush = 0;
}







time_t temp_time=0;

void write_cached_buffer()
{
	DWORD* pOld_blocks;
	DWORD exist_info;
	int hashing_diff;
	int nBlocks;
	int nBytes;
	int loop;
	int result;

	BYTE* dst;
	BYTE* pTemp;
	NEW_GROUP_HEADER* pGH;
	NEW_GROUP_HEADER* pNextGroupHeader;
	LOGICAL_VOLUME_HEADER* pLVH;
	LOGICAL_VOLUME_GROUP* pLVG;
	unsigned long * pEvent_ptrs;





	dst = gNewVar.Record.pData;
	pGH = (NEW_GROUP_HEADER*)dst;
	nBlocks = pGH->block_size;

	if ( gNewVar.Record.record_ready == OFF )
	{
		printf("Data occured but discarded because of absence of HDD! \n");
		//usleep(100000);
		return;
	}

	pLVG = (LOGICAL_VOLUME_GROUP*)gNewVar.Record.LVG_ptr;
	pLVH = (LOGICAL_VOLUME_HEADER*)&pLVG->tLVH;



	if ( pLVH->status == 0 ) // REC_TIME_INITIALIZER
	{
		pLVH->start_time = pGH->last_time; // do not need any more?
	}
	else if ( gNewVar.Record.Rec_offset + nBlocks >= LVG_BLOCKS ||    // HDD FULL!! or 시간 초과(8시간을 벗어나지 않게. 1초당 8byte? 8배 할것.)
		      (pGH->last_time - pLVH->start_time) >= MAX_HOURS || 
		      (pGH->last_time - pLVH->start_time) < 0 || 
		      forced_head_change == 1 ) 
	{
		LOGICAL_VOLUME_GROUP* newLVG;


		find_again:

		if ( free_LVG.LVG_ptr_tail == NULL )
		{
			if ( gEnv.recinfo.repeat_rec == 1 && data_LVG.LVG_ptr_head->next_pLVG != NULL )
			{
				gNewVar.Record.repeat = 1;
				newLVG = data_LVG.LVG_ptr_head;
				data_LVG.LVG_ptr_head->next_pLVG->prev_pLVG = NULL;
				data_LVG.LVG_ptr_head = data_LVG.LVG_ptr_head->next_pLVG;
			}
			else
			{
				if ( forced_head_change == 0 )
				{
					gNewVar.Record.forced_flush = 1;
					flush_time_index();
				}
				else
					forced_head_change = 0;

				gNewVar.Record.record_ready = OFF;
				return;
			}
		}
		else
		{
			newLVG = pop_free_LVG();
		}

		if ( ide_devs_info[ newLVG->hdd_id ].exist == 0 || newLVG->tLVH.status == 3 )
		{
			// newLVG will be orphan..
			goto find_again;
		}

		insert_freeblock_to_dataLGV( newLVG, pGH->last_time);

		pLVH->last_offset = gNewVar.Record.Rec_offset;
		pLVH->new_offset = LVG_DATA_OFFSET;
		pLVH->status = 2;


		if ( forced_head_change == 0 )
		{
			gNewVar.Record.forced_flush = 1;
			flush_time_index(); // 현재 볼륨의 타임 인덱스를 write한다.
		}
		else
		{
			forced_head_change = 0;
		}

		pthread_mutex_unlock(&ide_devs_info[ pLVG->hdd_id ].ide_access_wait);


		gNewVar.Record.LVG_ptr = newLVG;
		pLVG = (LOGICAL_VOLUME_GROUP*)gNewVar.Record.LVG_ptr;
		pthread_mutex_lock(&ide_devs_info[ pLVG->hdd_id ].ide_access_wait);

		pLVH = (LOGICAL_VOLUME_HEADER*)&pLVG->tLVH;
		pLVH->status = 0;

		memset( gNewVar.Record.pTimeIndex, 0, LVG_TIMEINDEX_BLOCKS<<9);
		memset( gNewVar.Record.pEventIndex, 0, LVG_EVENTINDEX_BLOCKS<<9);
		pLVH->start_time = pGH->last_time;
		pLVH->end_time   = pGH->last_time;
		pLVH->event_count = 0;

		pLVG->event_former_record_length = 0;
		pLVG->event_offset_start = NULL;
		pLVG->event_offset_end = NULL;

		gNewVar.Record.Prev_offset = 0;
		gNewVar.Record.Rec_offset = LVG_DATA_OFFSET;
		gNewVar.Record.last_blocks = 0;
		gNewVar.Record.last_modified = 0;
		gNewVar.Record.forced_flush = 1;

		printf ("[HDD] New Logical Volume prepared... : %d-%03d \n", pLVG->hdd_id, pLVH->volume_id);
	}




	if ( pGH->event_offset_start != pGH->event_offset_end ) // 같지 않으면, 이 group 안에 이벤트 정보가 존재한다.
	{
		if ( pLVG->event_offset_start == NULL )
			pLVG->event_offset_start = pGH->event_offset_start;

		pLVG->event_offset_end = pGH->event_offset_end;
		gNewVar.Event.index_dirty = 1;
	}



	if ( nBlocks > 3 )
	{
		result = 1;
		pEvent_ptrs = (unsigned long *)((BYTE*)dst + MAX_GROUP_SIZE - ((GROUP_HEADER_SIZE + GROUP_TAIL_SIZE)<<9));

		for ( loop = 0; loop < pGH->nr_events; loop++)
		{
			printf(" _%p_ 's REC offset changed \n", (struct tagEventInfo *)(pEvent_ptrs[loop]) );
			((struct tagEventInfo *)(pEvent_ptrs[loop]))->group_offset = gNewVar.Record.Rec_offset;
			gNewVar.Event.index_dirty = 1;
		}


		pGH->prev_offset = gNewVar.Record.Prev_offset;
		pGH->current_offset = gNewVar.Record.Rec_offset;
		pGH->prev_blocks = gNewVar.Record.last_blocks;
// printf("pGH->prev_offset ==>%d\n", pGH->prev_offset);
// printf("pGH->current_offset ==>%d\n", pGH->current_offset);
// printf("pGH->prev_blocks ==>%d\n", pGH->prev_blocks);

		gNewVar.Record.Prev_offset = gNewVar.Record.Rec_offset;
		gNewVar.Record.Rec_offset = gNewVar.Record.Rec_offset + nBlocks;
		pGH->next_offset = gNewVar.Record.Rec_offset;
		pGH->lvg_time = pLVH->start_time;
		gNewVar.Record.last_blocks = nBlocks;


/* DEBUG */
		if( temp_time != pGH->lvg_time )
		{
			temp_time = pGH->lvg_time; 
			clock_debug_message(pGH->lvg_time);
		}
/* DEBUG END */


		nBytes = nBlocks << 9;

		pNextGroupHeader = (NEW_GROUP_HEADER*)((BYTE*)dst + MAX_GROUP_SIZE - (GROUP_HEADER_SIZE << 9));
		memset( pNextGroupHeader, 0, 512);
		pNextGroupHeader->prev_offset 		= gNewVar.Record.Prev_offset;
		pNextGroupHeader->current_offset 	= gNewVar.Record.Rec_offset;
		pNextGroupHeader->last_time 		= pGH->last_time;
		pNextGroupHeader->block_size 		= 1;
		pNextGroupHeader->prev_blocks 	= gNewVar.Record.last_blocks;
		pNextGroupHeader->lvg_time 		= pLVH->start_time;

		pTemp = (BYTE*)pNextGroupHeader;

		dst[511] = make_crc( dst, 512 - 1);
		pTemp[511] = make_crc( pTemp, 512 - 1);



		// ---------------------
		// Izone WRITE
		// ---------------------
		nBlocks = (pGH->Izone + 511) >> 9;
		nBytes = nBlocks << 9;
		*(WORD*)((BYTE*)pGH + nBytes) = (WORD)0x504A;
		result = ide_block_write( pLVG->hdd_id, pLVG->start_offset + gNewVar.Record.Prev_offset, dst, nBlocks + 1, 0); // GROUP_HEADER_SIZE = 1블럭



		// ---------------------
		// non_Izone WRITE
		// ---------------------
		nBlocks = (pGH->non_Izone + 511)>>9;
		if ( result > 0 )
			result = ide_block_write( pLVG->hdd_id, pLVG->start_offset + gNewVar.Record.Rec_offset - nBlocks, dst + MAX_GROUP_SIZE - ((nBlocks + GROUP_HEADER_SIZE)<<9), nBlocks + GROUP_HEADER_SIZE, 0);




		hashing_diff = (int)(pGH->last_time) - (int)(pLVH->start_time);

		gNewVar.Record.last_modified = (hashing_diff * HASHING_INFO_SIZE);
		pOld_blocks = (DWORD *)(gNewVar.Record.pTimeIndex + gNewVar.Record.last_modified);
		if ( *pOld_blocks == 0 )
		{
			*pOld_blocks = pGH->current_offset;
			if ( gNewVar.Record.verbose == 1 )
				fprintf( stderr, "[HDD] New Time Index added : %d : %d : %ld!!! \n", hashing_diff, (int)(pGH->last_time), *pOld_blocks);

			gNewVar.Record.index_dirty = 1; // 결과적으로 1초마다 한번씩?
		}

		if ( gNewVar.Record.verbose == 1 )
			fprintf( stderr, "Seq. %04d | Image %03d | Size %7d:%7d | Index %d - %4d\n", (unsigned int)pGH->seq, (unsigned short)pGH->num_picture, (unsigned int)pGH->Izone, (unsigned int)pGH->non_Izone, pLVH->volume_id, (unsigned int)gNewVar.Record.Prev_offset);

	
		exist_info = (((DWORD)pGH->CH_exist)<<16) | (((DWORD)pGH->EVENT_exist)&0x0000ffff);
		*(pOld_blocks + 1) |= exist_info;
		
		if ( result <= 0 )
		{
			// EXT HDD ERROR
			forced_head_change = 1;	
		}
	}



	pLVH->end_time    	= pGH->last_time;
	pLVH->last_offset 		= gNewVar.Record.Prev_offset;
	pLVH->new_offset  	= gNewVar.Record.Rec_offset;
	pLVH->status 			= 1;
}











// --------> env.c file

static BYTE crc_table[256] = {
	0,	94,	188,	226,	97,	63,	221,	131,
	194,	156,	126,	32,	163,	253,	31,	65,
	157,	195,	33,	127,	252,	162,	64,	30,
	95,	1,	227,	189,	62,	96,	130,	220,
	35,	125,	159,	193,	66,	28,	254,	160,
	225,	191,	93,	3,	128,	222,	60,	98,
	190,	224,	2,	92,	223,	129,	99,	61,
	124,	34,	192,	158,	29,	67,	161,	255,
	70,	24,	250,	164,	39,	121,	155,	197,
	132,	218,	56,	102,	229,	187,	89,	7,
	219,	133,	103,	57,	186,	228,	6,	88,
	25,	71,	165,	251,	120,	38,	196,	154,
	101,	59,	217,	135,	4,	90,	184,	230,
	167,	249,	27,	69,	198,	152,	122,	36,
	248,	166,	68,	26,	153,	199,	37,	123,
	58,	100,	134,	216,	91,	5,	231,	185,
	140,	210,	48,	110,	237,	179,	81,	15,
	78,	16,	242,	172,	47,	113,	147,	205,
	17,	79,	173,	243,	112,	46,	204,	146,
	211,	141,	111,	49,	178,	236,	14,	80,
	175,	241,	19,	77,	206,	144,	114,	44,
	109,	51,	209,	143,	12,	82,	176,	238,
	50,	108,	142,	208,	83,	13,	239,	177,
	240,	174,	76,	18,	145,	207,	45,	115,
	202,	148,	118,	40,	171,	245,	23,	73,
	8,	86,	180,	234,	105,	55,	213,	139,
	87,	9,	235,	181,	54,	104,	138,	212,
	149,	203,	41,	119,	244,	170,	72,	22,
	233,	183,	85,	11,	136,	214,	52,	106,
	43,	117,	151,	201,	74,	20,	246,	168,
	116,	42,	200,	150,	21,	75,	169,	247,
	182,	232,	10,	84,	215,	137,	107,	53
};
 

BYTE make_crc(BYTE *buf,int len)
{
	BYTE curr_crc=0;
	int i;
	BYTE *curr_byte = buf;

	for(i=0;i<len;i++)
	{
		curr_crc = crc_table[ curr_crc ^ (*curr_byte) ]; // XOR operation
		//printd("run : %04d   data : %3d  crc : %3d\n",i,*curr_byte,curr_crc);
		curr_byte++;

	}
	return curr_crc;
}

int check_crc( BYTE *buf, int len)
{
	if ( make_crc(buf,len) == buf[len] )
	{
// printf("check_crc return 1\n");
		return 1;
	}
	else
	{
// printf("check_crc return 0\n");
		return 0;
	}
}







