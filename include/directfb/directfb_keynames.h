#ifndef __DIRECTFB_KEYNAMES_H__
#define __DIRECTFB_KEYNAMES_H__


struct DFBKeySymbolName {
     DFBInputDeviceKeySymbol symbol;
     const char *name;
};

#define DirectFBKeySymbolNames(Identifier) struct DFBKeySymbolName Identifier[] = { \
     { DIKS_BACKSPACE, "BACKSPACE" }, \
     { DIKS_TAB, "TAB" }, \
     { DIKS_RETURN, "RETURN" }, \
     { DIKS_CANCEL, "CANCEL" }, \
     { DIKS_ESCAPE, "ESCAPE" }, \
     { DIKS_SPACE, "SPACE" }, \
     { DIKS_EXCLAMATION_MARK, "EXCLAMATION_MARK" }, \
     { DIKS_QUOTATION, "QUOTATION" }, \
     { DIKS_NUMBER_SIGN, "NUMBER_SIGN" }, \
     { DIKS_DOLLAR_SIGN, "DOLLAR_SIGN" }, \
     { DIKS_PERCENT_SIGN, "PERCENT_SIGN" }, \
     { DIKS_AMPERSAND, "AMPERSAND" }, \
     { DIKS_APOSTROPHE, "APOSTROPHE" }, \
     { DIKS_PARENTHESIS_LEFT, "PARENTHESIS_LEFT" }, \
     { DIKS_PARENTHESIS_RIGHT, "PARENTHESIS_RIGHT" }, \
     { DIKS_ASTERISK, "ASTERISK" }, \
     { DIKS_PLUS_SIGN, "PLUS_SIGN" }, \
     { DIKS_COMMA, "COMMA" }, \
     { DIKS_MINUS_SIGN, "MINUS_SIGN" }, \
     { DIKS_PERIOD, "PERIOD" }, \
     { DIKS_SLASH, "SLASH" }, \
     { DIKS_0, "0" }, \
     { DIKS_1, "1" }, \
     { DIKS_2, "2" }, \
     { DIKS_3, "3" }, \
     { DIKS_4, "4" }, \
     { DIKS_5, "5" }, \
     { DIKS_6, "6" }, \
     { DIKS_7, "7" }, \
     { DIKS_8, "8" }, \
     { DIKS_9, "9" }, \
     { DIKS_COLON, "COLON" }, \
     { DIKS_SEMICOLON, "SEMICOLON" }, \
     { DIKS_LESS_THAN_SIGN, "LESS_THAN_SIGN" }, \
     { DIKS_EQUALS_SIGN, "EQUALS_SIGN" }, \
     { DIKS_GREATER_THAN_SIGN, "GREATER_THAN_SIGN" }, \
     { DIKS_QUESTION_MARK, "QUESTION_MARK" }, \
     { DIKS_AT, "AT" }, \
     { DIKS_CAPITAL_A, "CAPITAL_A" }, \
     { DIKS_CAPITAL_B, "CAPITAL_B" }, \
     { DIKS_CAPITAL_C, "CAPITAL_C" }, \
     { DIKS_CAPITAL_D, "CAPITAL_D" }, \
     { DIKS_CAPITAL_E, "CAPITAL_E" }, \
     { DIKS_CAPITAL_F, "CAPITAL_F" }, \
     { DIKS_CAPITAL_G, "CAPITAL_G" }, \
     { DIKS_CAPITAL_H, "CAPITAL_H" }, \
     { DIKS_CAPITAL_I, "CAPITAL_I" }, \
     { DIKS_CAPITAL_J, "CAPITAL_J" }, \
     { DIKS_CAPITAL_K, "CAPITAL_K" }, \
     { DIKS_CAPITAL_L, "CAPITAL_L" }, \
     { DIKS_CAPITAL_M, "CAPITAL_M" }, \
     { DIKS_CAPITAL_N, "CAPITAL_N" }, \
     { DIKS_CAPITAL_O, "CAPITAL_O" }, \
     { DIKS_CAPITAL_P, "CAPITAL_P" }, \
     { DIKS_CAPITAL_Q, "CAPITAL_Q" }, \
     { DIKS_CAPITAL_R, "CAPITAL_R" }, \
     { DIKS_CAPITAL_S, "CAPITAL_S" }, \
     { DIKS_CAPITAL_T, "CAPITAL_T" }, \
     { DIKS_CAPITAL_U, "CAPITAL_U" }, \
     { DIKS_CAPITAL_V, "CAPITAL_V" }, \
     { DIKS_CAPITAL_W, "CAPITAL_W" }, \
     { DIKS_CAPITAL_X, "CAPITAL_X" }, \
     { DIKS_CAPITAL_Y, "CAPITAL_Y" }, \
     { DIKS_CAPITAL_Z, "CAPITAL_Z" }, \
     { DIKS_SQUARE_BRACKET_LEFT, "SQUARE_BRACKET_LEFT" }, \
     { DIKS_BACKSLASH, "BACKSLASH" }, \
     { DIKS_SQUARE_BRACKET_RIGHT, "SQUARE_BRACKET_RIGHT" }, \
     { DIKS_CIRCUMFLEX_ACCENT, "CIRCUMFLEX_ACCENT" }, \
     { DIKS_UNDERSCORE, "UNDERSCORE" }, \
     { DIKS_GRAVE_ACCENT, "GRAVE_ACCENT" }, \
     { DIKS_SMALL_A, "SMALL_A" }, \
     { DIKS_SMALL_B, "SMALL_B" }, \
     { DIKS_SMALL_C, "SMALL_C" }, \
     { DIKS_SMALL_D, "SMALL_D" }, \
     { DIKS_SMALL_E, "SMALL_E" }, \
     { DIKS_SMALL_F, "SMALL_F" }, \
     { DIKS_SMALL_G, "SMALL_G" }, \
     { DIKS_SMALL_H, "SMALL_H" }, \
     { DIKS_SMALL_I, "SMALL_I" }, \
     { DIKS_SMALL_J, "SMALL_J" }, \
     { DIKS_SMALL_K, "SMALL_K" }, \
     { DIKS_SMALL_L, "SMALL_L" }, \
     { DIKS_SMALL_M, "SMALL_M" }, \
     { DIKS_SMALL_N, "SMALL_N" }, \
     { DIKS_SMALL_O, "SMALL_O" }, \
     { DIKS_SMALL_P, "SMALL_P" }, \
     { DIKS_SMALL_Q, "SMALL_Q" }, \
     { DIKS_SMALL_R, "SMALL_R" }, \
     { DIKS_SMALL_S, "SMALL_S" }, \
     { DIKS_SMALL_T, "SMALL_T" }, \
     { DIKS_SMALL_U, "SMALL_U" }, \
     { DIKS_SMALL_V, "SMALL_V" }, \
     { DIKS_SMALL_W, "SMALL_W" }, \
     { DIKS_SMALL_X, "SMALL_X" }, \
     { DIKS_SMALL_Y, "SMALL_Y" }, \
     { DIKS_SMALL_Z, "SMALL_Z" }, \
     { DIKS_CURLY_BRACKET_LEFT, "CURLY_BRACKET_LEFT" }, \
     { DIKS_VERTICAL_BAR, "VERTICAL_BAR" }, \
     { DIKS_CURLY_BRACKET_RIGHT, "CURLY_BRACKET_RIGHT" }, \
     { DIKS_TILDE, "TILDE" }, \
     { DIKS_DELETE, "DELETE" }, \
     { DIKS_CURSOR_LEFT, "CURSOR_LEFT" }, \
     { DIKS_CURSOR_RIGHT, "CURSOR_RIGHT" }, \
     { DIKS_CURSOR_UP, "CURSOR_UP" }, \
     { DIKS_CURSOR_DOWN, "CURSOR_DOWN" }, \
     { DIKS_INSERT, "INSERT" }, \
     { DIKS_HOME, "HOME" }, \
     { DIKS_END, "END" }, \
     { DIKS_PAGE_UP, "PAGE_UP" }, \
     { DIKS_PAGE_DOWN, "PAGE_DOWN" }, \
     { DIKS_PRINT, "PRINT" }, \
     { DIKS_PAUSE, "PAUSE" }, \
     { DIKS_OK, "OK" }, \
     { DIKS_SELECT, "SELECT" }, \
     { DIKS_GOTO, "GOTO" }, \
     { DIKS_CLEAR, "CLEAR" }, \
     { DIKS_POWER, "POWER" }, \
     { DIKS_POWER2, "POWER2" }, \
     { DIKS_OPTION, "OPTION" }, \
     { DIKS_MENU, "MENU" }, \
     { DIKS_HELP, "HELP" }, \
     { DIKS_INFO, "INFO" }, \
     { DIKS_TIME, "TIME" }, \
     { DIKS_VENDOR, "VENDOR" }, \
     { DIKS_ARCHIVE, "ARCHIVE" }, \
     { DIKS_PROGRAM, "PROGRAM" }, \
     { DIKS_CHANNEL, "CHANNEL" }, \
     { DIKS_FAVORITES, "FAVORITES" }, \
     { DIKS_EPG, "EPG" }, \
     { DIKS_PVR, "PVR" }, \
     { DIKS_MHP, "MHP" }, \
     { DIKS_LANGUAGE, "LANGUAGE" }, \
     { DIKS_TITLE, "TITLE" }, \
     { DIKS_SUBTITLE, "SUBTITLE" }, \
     { DIKS_ANGLE, "ANGLE" }, \
     { DIKS_ZOOM, "ZOOM" }, \
     { DIKS_MODE, "MODE" }, \
     { DIKS_KEYBOARD, "KEYBOARD" }, \
     { DIKS_PC, "PC" }, \
     { DIKS_SCREEN, "SCREEN" }, \
     { DIKS_TV, "TV" }, \
     { DIKS_TV2, "TV2" }, \
     { DIKS_VCR, "VCR" }, \
     { DIKS_VCR2, "VCR2" }, \
     { DIKS_SAT, "SAT" }, \
     { DIKS_SAT2, "SAT2" }, \
     { DIKS_CD, "CD" }, \
     { DIKS_TAPE, "TAPE" }, \
     { DIKS_RADIO, "RADIO" }, \
     { DIKS_TUNER, "TUNER" }, \
     { DIKS_PLAYER, "PLAYER" }, \
     { DIKS_TEXT, "TEXT" }, \
     { DIKS_DVD, "DVD" }, \
     { DIKS_AUX, "AUX" }, \
     { DIKS_MP3, "MP3" }, \
     { DIKS_PHONE, "PHONE" }, \
     { DIKS_AUDIO, "AUDIO" }, \
     { DIKS_VIDEO, "VIDEO" }, \
     { DIKS_INTERNET, "INTERNET" }, \
     { DIKS_MAIL, "MAIL" }, \
     { DIKS_NEWS, "NEWS" }, \
     { DIKS_DIRECTORY, "DIRECTORY" }, \
     { DIKS_LIST, "LIST" }, \
     { DIKS_CALCULATOR, "CALCULATOR" }, \
     { DIKS_MEMO, "MEMO" }, \
     { DIKS_CALENDAR, "CALENDAR" }, \
     { DIKS_EDITOR, "EDITOR" }, \
     { DIKS_RED, "RED" }, \
     { DIKS_GREEN, "GREEN" }, \
     { DIKS_YELLOW, "YELLOW" }, \
     { DIKS_BLUE, "BLUE" }, \
     { DIKS_CHANNEL_UP, "CHANNEL_UP" }, \
     { DIKS_CHANNEL_DOWN, "CHANNEL_DOWN" }, \
     { DIKS_BACK, "BACK" }, \
     { DIKS_FORWARD, "FORWARD" }, \
     { DIKS_FIRST, "FIRST" }, \
     { DIKS_LAST, "LAST" }, \
     { DIKS_VOLUME_UP, "VOLUME_UP" }, \
     { DIKS_VOLUME_DOWN, "VOLUME_DOWN" }, \
     { DIKS_MUTE, "MUTE" }, \
     { DIKS_AB, "AB" }, \
     { DIKS_PLAYPAUSE, "PLAYPAUSE" }, \
     { DIKS_PLAY, "PLAY" }, \
     { DIKS_STOP, "STOP" }, \
     { DIKS_RESTART, "RESTART" }, \
     { DIKS_SLOW, "SLOW" }, \
     { DIKS_FAST, "FAST" }, \
     { DIKS_RECORD, "RECORD" }, \
     { DIKS_EJECT, "EJECT" }, \
     { DIKS_SHUFFLE, "SHUFFLE" }, \
     { DIKS_REWIND, "REWIND" }, \
     { DIKS_FASTFORWARD, "FASTFORWARD" }, \
     { DIKS_PREVIOUS, "PREVIOUS" }, \
     { DIKS_NEXT, "NEXT" }, \
     { DIKS_BEGIN, "BEGIN" }, \
     { DIKS_DIGITS, "DIGITS" }, \
     { DIKS_TEEN, "TEEN" }, \
     { DIKS_TWEN, "TWEN" }, \
     { DIKS_BREAK, "BREAK" }, \
     { DIKS_EXIT, "EXIT" }, \
     { DIKS_SETUP, "SETUP" }, \
     { DIKS_CURSOR_LEFT_UP, "CURSOR_LEFT_UP" }, \
     { DIKS_CURSOR_LEFT_DOWN, "CURSOR_LEFT_DOWN" }, \
     { DIKS_CURSOR_UP_RIGHT, "CURSOR_UP_RIGHT" }, \
     { DIKS_CURSOR_DOWN_RIGHT, "CURSOR_DOWN_RIGHT" }, \
     { DIKS_F1, "F1" }, \
     { DIKS_F2, "F2" }, \
     { DIKS_F3, "F3" }, \
     { DIKS_F4, "F4" }, \
     { DIKS_F5, "F5" }, \
     { DIKS_F6, "F6" }, \
     { DIKS_F7, "F7" }, \
     { DIKS_F8, "F8" }, \
     { DIKS_F9, "F9" }, \
     { DIKS_F10, "F10" }, \
     { DIKS_F11, "F11" }, \
     { DIKS_F12, "F12" }, \
     { DIKS_SHIFT, "SHIFT" }, \
     { DIKS_CONTROL, "CONTROL" }, \
     { DIKS_ALT, "ALT" }, \
     { DIKS_ALTGR, "ALTGR" }, \
     { DIKS_META, "META" }, \
     { DIKS_SUPER, "SUPER" }, \
     { DIKS_HYPER, "HYPER" }, \
     { DIKS_CAPS_LOCK, "CAPS_LOCK" }, \
     { DIKS_NUM_LOCK, "NUM_LOCK" }, \
     { DIKS_SCROLL_LOCK, "SCROLL_LOCK" }, \
     { DIKS_DEAD_ABOVEDOT, "DEAD_ABOVEDOT" }, \
     { DIKS_DEAD_ABOVERING, "DEAD_ABOVERING" }, \
     { DIKS_DEAD_ACUTE, "DEAD_ACUTE" }, \
     { DIKS_DEAD_BREVE, "DEAD_BREVE" }, \
     { DIKS_DEAD_CARON, "DEAD_CARON" }, \
     { DIKS_DEAD_CEDILLA, "DEAD_CEDILLA" }, \
     { DIKS_DEAD_CIRCUMFLEX, "DEAD_CIRCUMFLEX" }, \
     { DIKS_DEAD_DIAERESIS, "DEAD_DIAERESIS" }, \
     { DIKS_DEAD_DOUBLEACUTE, "DEAD_DOUBLEACUTE" }, \
     { DIKS_DEAD_GRAVE, "DEAD_GRAVE" }, \
     { DIKS_DEAD_IOTA, "DEAD_IOTA" }, \
     { DIKS_DEAD_MACRON, "DEAD_MACRON" }, \
     { DIKS_DEAD_OGONEK, "DEAD_OGONEK" }, \
     { DIKS_DEAD_SEMIVOICED_SOUND, "DEAD_SEMIVOICED_SOUND" }, \
     { DIKS_DEAD_TILDE, "DEAD_TILDE" }, \
     { DIKS_DEAD_VOICED_SOUND, "DEAD_VOICED_SOUND" }, \
     { DIKS_CUSTOM0, "CUSTOM0" }, \
     { DIKS_CUSTOM1, "CUSTOM1" }, \
     { DIKS_CUSTOM2, "CUSTOM2" }, \
     { DIKS_CUSTOM3, "CUSTOM3" }, \
     { DIKS_CUSTOM4, "CUSTOM4" }, \
     { DIKS_CUSTOM5, "CUSTOM5" }, \
     { DIKS_CUSTOM6, "CUSTOM6" }, \
     { DIKS_CUSTOM7, "CUSTOM7" }, \
     { DIKS_CUSTOM8, "CUSTOM8" }, \
     { DIKS_CUSTOM9, "CUSTOM9" }, \
     { DIKS_CUSTOM10, "CUSTOM10" }, \
     { DIKS_CUSTOM11, "CUSTOM11" }, \
     { DIKS_CUSTOM12, "CUSTOM12" }, \
     { DIKS_CUSTOM13, "CUSTOM13" }, \
     { DIKS_CUSTOM14, "CUSTOM14" }, \
     { DIKS_CUSTOM15, "CUSTOM15" }, \
     { DIKS_CUSTOM16, "CUSTOM16" }, \
     { DIKS_CUSTOM17, "CUSTOM17" }, \
     { DIKS_CUSTOM18, "CUSTOM18" }, \
     { DIKS_CUSTOM19, "CUSTOM19" }, \
     { DIKS_CUSTOM20, "CUSTOM20" }, \
     { DIKS_CUSTOM21, "CUSTOM21" }, \
     { DIKS_CUSTOM22, "CUSTOM22" }, \
     { DIKS_CUSTOM23, "CUSTOM23" }, \
     { DIKS_CUSTOM24, "CUSTOM24" }, \
     { DIKS_CUSTOM25, "CUSTOM25" }, \
     { DIKS_CUSTOM26, "CUSTOM26" }, \
     { DIKS_CUSTOM27, "CUSTOM27" }, \
     { DIKS_CUSTOM28, "CUSTOM28" }, \
     { DIKS_CUSTOM29, "CUSTOM29" }, \
     { DIKS_CUSTOM30, "CUSTOM30" }, \
     { DIKS_CUSTOM31, "CUSTOM31" }, \
     { DIKS_CUSTOM32, "CUSTOM32" }, \
     { DIKS_CUSTOM33, "CUSTOM33" }, \
     { DIKS_CUSTOM34, "CUSTOM34" }, \
     { DIKS_CUSTOM35, "CUSTOM35" }, \
     { DIKS_CUSTOM36, "CUSTOM36" }, \
     { DIKS_CUSTOM37, "CUSTOM37" }, \
     { DIKS_CUSTOM38, "CUSTOM38" }, \
     { DIKS_CUSTOM39, "CUSTOM39" }, \
     { DIKS_CUSTOM40, "CUSTOM40" }, \
     { DIKS_CUSTOM41, "CUSTOM41" }, \
     { DIKS_CUSTOM42, "CUSTOM42" }, \
     { DIKS_CUSTOM43, "CUSTOM43" }, \
     { DIKS_CUSTOM44, "CUSTOM44" }, \
     { DIKS_CUSTOM45, "CUSTOM45" }, \
     { DIKS_CUSTOM46, "CUSTOM46" }, \
     { DIKS_CUSTOM47, "CUSTOM47" }, \
     { DIKS_CUSTOM48, "CUSTOM48" }, \
     { DIKS_CUSTOM49, "CUSTOM49" }, \
     { DIKS_CUSTOM50, "CUSTOM50" }, \
     { DIKS_CUSTOM51, "CUSTOM51" }, \
     { DIKS_CUSTOM52, "CUSTOM52" }, \
     { DIKS_CUSTOM53, "CUSTOM53" }, \
     { DIKS_CUSTOM54, "CUSTOM54" }, \
     { DIKS_CUSTOM55, "CUSTOM55" }, \
     { DIKS_CUSTOM56, "CUSTOM56" }, \
     { DIKS_CUSTOM57, "CUSTOM57" }, \
     { DIKS_CUSTOM58, "CUSTOM58" }, \
     { DIKS_CUSTOM59, "CUSTOM59" }, \
     { DIKS_CUSTOM60, "CUSTOM60" }, \
     { DIKS_CUSTOM61, "CUSTOM61" }, \
     { DIKS_CUSTOM62, "CUSTOM62" }, \
     { DIKS_CUSTOM63, "CUSTOM63" }, \
     { DIKS_CUSTOM64, "CUSTOM64" }, \
     { DIKS_CUSTOM65, "CUSTOM65" }, \
     { DIKS_CUSTOM66, "CUSTOM66" }, \
     { DIKS_CUSTOM67, "CUSTOM67" }, \
     { DIKS_CUSTOM68, "CUSTOM68" }, \
     { DIKS_CUSTOM69, "CUSTOM69" }, \
     { DIKS_CUSTOM70, "CUSTOM70" }, \
     { DIKS_CUSTOM71, "CUSTOM71" }, \
     { DIKS_CUSTOM72, "CUSTOM72" }, \
     { DIKS_CUSTOM73, "CUSTOM73" }, \
     { DIKS_CUSTOM74, "CUSTOM74" }, \
     { DIKS_CUSTOM75, "CUSTOM75" }, \
     { DIKS_CUSTOM76, "CUSTOM76" }, \
     { DIKS_CUSTOM77, "CUSTOM77" }, \
     { DIKS_CUSTOM78, "CUSTOM78" }, \
     { DIKS_CUSTOM79, "CUSTOM79" }, \
     { DIKS_CUSTOM80, "CUSTOM80" }, \
     { DIKS_CUSTOM81, "CUSTOM81" }, \
     { DIKS_CUSTOM82, "CUSTOM82" }, \
     { DIKS_CUSTOM83, "CUSTOM83" }, \
     { DIKS_CUSTOM84, "CUSTOM84" }, \
     { DIKS_CUSTOM85, "CUSTOM85" }, \
     { DIKS_CUSTOM86, "CUSTOM86" }, \
     { DIKS_CUSTOM87, "CUSTOM87" }, \
     { DIKS_CUSTOM88, "CUSTOM88" }, \
     { DIKS_CUSTOM89, "CUSTOM89" }, \
     { DIKS_CUSTOM90, "CUSTOM90" }, \
     { DIKS_CUSTOM91, "CUSTOM91" }, \
     { DIKS_CUSTOM92, "CUSTOM92" }, \
     { DIKS_CUSTOM93, "CUSTOM93" }, \
     { DIKS_CUSTOM94, "CUSTOM94" }, \
     { DIKS_CUSTOM95, "CUSTOM95" }, \
     { DIKS_CUSTOM96, "CUSTOM96" }, \
     { DIKS_CUSTOM97, "CUSTOM97" }, \
     { DIKS_CUSTOM98, "CUSTOM98" }, \
     { DIKS_CUSTOM99, "CUSTOM99" }, \
     { DIKS_NULL, "NULL" } \
};


struct DFBKeyIdentifierName {
     DFBInputDeviceKeyIdentifier identifier;
     const char *name;
};

#define DirectFBKeyIdentifierNames(Identifier) struct DFBKeyIdentifierName Identifier[] = { \
     { DIKI_A, "A" }, \
     { DIKI_B, "B" }, \
     { DIKI_C, "C" }, \
     { DIKI_D, "D" }, \
     { DIKI_E, "E" }, \
     { DIKI_F, "F" }, \
     { DIKI_G, "G" }, \
     { DIKI_H, "H" }, \
     { DIKI_I, "I" }, \
     { DIKI_J, "J" }, \
     { DIKI_K, "K" }, \
     { DIKI_L, "L" }, \
     { DIKI_M, "M" }, \
     { DIKI_N, "N" }, \
     { DIKI_O, "O" }, \
     { DIKI_P, "P" }, \
     { DIKI_Q, "Q" }, \
     { DIKI_R, "R" }, \
     { DIKI_S, "S" }, \
     { DIKI_T, "T" }, \
     { DIKI_U, "U" }, \
     { DIKI_V, "V" }, \
     { DIKI_W, "W" }, \
     { DIKI_X, "X" }, \
     { DIKI_Y, "Y" }, \
     { DIKI_Z, "Z" }, \
     { DIKI_0, "0" }, \
     { DIKI_1, "1" }, \
     { DIKI_2, "2" }, \
     { DIKI_3, "3" }, \
     { DIKI_4, "4" }, \
     { DIKI_5, "5" }, \
     { DIKI_6, "6" }, \
     { DIKI_7, "7" }, \
     { DIKI_8, "8" }, \
     { DIKI_9, "9" }, \
     { DIKI_F1, "F1" }, \
     { DIKI_F2, "F2" }, \
     { DIKI_F3, "F3" }, \
     { DIKI_F4, "F4" }, \
     { DIKI_F5, "F5" }, \
     { DIKI_F6, "F6" }, \
     { DIKI_F7, "F7" }, \
     { DIKI_F8, "F8" }, \
     { DIKI_F9, "F9" }, \
     { DIKI_F10, "F10" }, \
     { DIKI_F11, "F11" }, \
     { DIKI_F12, "F12" }, \
     { DIKI_SHIFT_L, "SHIFT_L" }, \
     { DIKI_SHIFT_R, "SHIFT_R" }, \
     { DIKI_CONTROL_L, "CONTROL_L" }, \
     { DIKI_CONTROL_R, "CONTROL_R" }, \
     { DIKI_ALT_L, "ALT_L" }, \
     { DIKI_ALT_R, "ALT_R" }, \
     { DIKI_META_L, "META_L" }, \
     { DIKI_META_R, "META_R" }, \
     { DIKI_SUPER_L, "SUPER_L" }, \
     { DIKI_SUPER_R, "SUPER_R" }, \
     { DIKI_HYPER_L, "HYPER_L" }, \
     { DIKI_HYPER_R, "HYPER_R" }, \
     { DIKI_CAPS_LOCK, "CAPS_LOCK" }, \
     { DIKI_NUM_LOCK, "NUM_LOCK" }, \
     { DIKI_SCROLL_LOCK, "SCROLL_LOCK" }, \
     { DIKI_ESCAPE, "ESCAPE" }, \
     { DIKI_LEFT, "LEFT" }, \
     { DIKI_RIGHT, "RIGHT" }, \
     { DIKI_UP, "UP" }, \
     { DIKI_DOWN, "DOWN" }, \
     { DIKI_TAB, "TAB" }, \
     { DIKI_ENTER, "ENTER" }, \
     { DIKI_SPACE, "SPACE" }, \
     { DIKI_BACKSPACE, "BACKSPACE" }, \
     { DIKI_INSERT, "INSERT" }, \
     { DIKI_DELETE, "DELETE" }, \
     { DIKI_HOME, "HOME" }, \
     { DIKI_END, "END" }, \
     { DIKI_PAGE_UP, "PAGE_UP" }, \
     { DIKI_PAGE_DOWN, "PAGE_DOWN" }, \
     { DIKI_PRINT, "PRINT" }, \
     { DIKI_PAUSE, "PAUSE" }, \
     { DIKI_QUOTE_LEFT, "QUOTE_LEFT" }, \
     { DIKI_MINUS_SIGN, "MINUS_SIGN" }, \
     { DIKI_EQUALS_SIGN, "EQUALS_SIGN" }, \
     { DIKI_BRACKET_LEFT, "BRACKET_LEFT" }, \
     { DIKI_BRACKET_RIGHT, "BRACKET_RIGHT" }, \
     { DIKI_BACKSLASH, "BACKSLASH" }, \
     { DIKI_SEMICOLON, "SEMICOLON" }, \
     { DIKI_QUOTE_RIGHT, "QUOTE_RIGHT" }, \
     { DIKI_COMMA, "COMMA" }, \
     { DIKI_PERIOD, "PERIOD" }, \
     { DIKI_SLASH, "SLASH" }, \
     { DIKI_LESS_SIGN, "LESS_SIGN" }, \
     { DIKI_KP_DIV, "KP_DIV" }, \
     { DIKI_KP_MULT, "KP_MULT" }, \
     { DIKI_KP_MINUS, "KP_MINUS" }, \
     { DIKI_KP_PLUS, "KP_PLUS" }, \
     { DIKI_KP_ENTER, "KP_ENTER" }, \
     { DIKI_KP_SPACE, "KP_SPACE" }, \
     { DIKI_KP_TAB, "KP_TAB" }, \
     { DIKI_KP_F1, "KP_F1" }, \
     { DIKI_KP_F2, "KP_F2" }, \
     { DIKI_KP_F3, "KP_F3" }, \
     { DIKI_KP_F4, "KP_F4" }, \
     { DIKI_KP_EQUAL, "KP_EQUAL" }, \
     { DIKI_KP_SEPARATOR, "KP_SEPARATOR" }, \
     { DIKI_KP_DECIMAL, "KP_DECIMAL" }, \
     { DIKI_KP_0, "KP_0" }, \
     { DIKI_KP_1, "KP_1" }, \
     { DIKI_KP_2, "KP_2" }, \
     { DIKI_KP_3, "KP_3" }, \
     { DIKI_KP_4, "KP_4" }, \
     { DIKI_KP_5, "KP_5" }, \
     { DIKI_KP_6, "KP_6" }, \
     { DIKI_KP_7, "KP_7" }, \
     { DIKI_KP_8, "KP_8" }, \
     { DIKI_KP_9, "KP_9" }, \
     { DIKI_UNKNOWN, "UNKNOWN" } \
};

#endif
