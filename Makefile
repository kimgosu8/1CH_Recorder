#라이브러리를 지정할 때는 -l (소문자 L) 옵션을 사용하고, 라이브러리의 경로를 지정할 때는 -L 옵션을 사용한다. 


VERSION = 0.1.0_kim
CC=gcc
INC_DIR	:= -I. -I./include -I./include/directfb
WARNINGS = -g -Wall -O2
CFLAGS = -DLINUX -DVERSION=\"$(VERSION)\" $(WARNINGS) 
LIBS =   -ljpeg -lpthread -lavformat  -lavcodec -lavutil -lasound -lm -ldl -ldirectfb -ldirect -lfusion -lbz2 -lz
SRC = src


#CFLAGS = -g -Wall -O2 -MD `pkg-config --cflags directfb`
#LFLAGS = -g -Wall -O2 `pkg-config --libs directfb`
#DEFS = -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE=\"DirectFB-examples\" -DVERSION=\"1.2.0\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STDBOOL_H=1



OBJ_DIR = Obj
AppName = 1ch_dvr


#=============================================================================
# Compile rules
#=============================================================================

OBJECTS	= \
	$(OBJ_DIR)/main.o \
	$(OBJ_DIR)/v4l2_control.o \
	$(OBJ_DIR)/alsa_control.o \
	$(OBJ_DIR)/ffmpeg_api.o \
	$(OBJ_DIR)/key.o \
	$(OBJ_DIR)/directfb.o \
	$(OBJ_DIR)/ide.o \
	$(OBJ_DIR)/main_api_thread.o \
	$(OBJ_DIR)/mpeg_enc.o \
	$(OBJ_DIR)/timer.o






$(OBJ_DIR)/%.o : $(SRC)/%.c
	@echo Compiling: $<
	$(CC) -c -o $@ $(CFLAGS) $(INC_DIR) $(DEFS) $< 
	@echo



#=============================================================================
# Target builds
#=============================================================================

all:target1 target2

target1:
	@clear
	@echo
	@echo Building: $(AppName)
	@echo
	@mkdir -p $(OBJ_DIR)

target2: $(OBJECTS)
	@echo
	@echo Linking application: $(AppName) 
	@$(CC) -o $(AppName) $(OBJECTS) $(LIBS) $(LFLAGS)
	@echo
#	@$(NM) $(OUT_DIR)/$(AppName) | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)' | sort > $(OUT_DIR)/$(AppName).map		
	@echo Executable file \"$(AppName)\" built successfully
	@echo

clean:
	@clear
	@echo
	@echo Deleting: Intermediate build files...
	@rm  -v -f $(OBJ_DIR)/*
	@if [ -d $(OBJ_DIR) ]; then rmdir -v --ignore-fail-on-non-empty $(OBJ_DIR); fi
	@echo Deleting: ....finished!
	@echo
	@echo

install:
	install $(APP_BINARY) $(PREFIX)



