/*
 * This file holds collections of strings to make localization easier.
 * Keep the max string length to 30 characters or bad things will happen
 */
#define MENU_STRING_LENGTH      30
//#define INTL_ENGLISH

/*
 * ---------------------------------------------------------------------------
 *  Spanish by Lopst's Wiimote version
 * ---------------------------------------------------------------------------
 */
#ifdef INTL_SPANISH

// Some general menu strings
#define MENU_ON                 "ON"
#define MENU_OFF                "OFF"
#define MENU_EXIT               "Volver atras"

// Main menu
#define MENU_MAIN_PLAY          "Jugar"
#define MENU_MAIN_RESET         "Resetear Emulador"
#define MENU_MAIN_LOAD          "Abrir Nuevo Juego"
#define MENU_MAIN_SAVE          "Guardar/Abrir"
#define MENU_MAIN_INFO          "Informacion ROM"
#define MENU_MAIN_JOYPADS       "Config. Controles"
#define MENU_MAIN_OPTIONS       "Opciones Emulador"
#ifdef HW_RVL
#define MENU_MAIN_RELOAD        "Reiniciar Cargador"
#define MENU_MAIN_REBOOT        "Reinciar Wii"
#else
#define MENU_MAIN_RELOAD        "PSO Reload"
#define MENU_MAIN_REBOOT        "Reboot Gamecube"
#endif
#define MENU_MAIN_CREDITS       "Ver Creditos"

// Media menu
#define MENU_MEDIA_TITLE        "Abrir Un Juego"
#define MENU_MEDIA_SDCARD       "Abrir desde SDCard"
#define MENU_MEDIA_DVD          "Abrir desde DVD"
#define MENU_MEDIA_STOPDVD      "Stop DVD Motor"
#define MENU_MEDIA_STOPPING     "Stopping DVD ... Wait"
#define MENU_MEDIA_STOPPED      "DVD Motor Stopped"

// File menu
#define MENU_FILE_TITLE         "Guardar Partidas"
#define MENU_FILE_MANAGER       "Manager"

// Save menu
#define MENU_SAVE_TITLE         "Manager"
#define MENU_SAVE_SAVE          "Guardar"
#define MENU_SAVE_LOAD          "Abrir"
#define MENU_SAVE_DEVICE        "Dispositivo"

// Rom Information
#define MENU_INFO_ROM           "ROM"
#define MENU_INFO_ROMID         "ROMID"
#define MENU_INFO_COMPANY       "Company"
#define MENU_INFO_SIZE          "Size"
#define MENU_INFO_SRAM          "SRAM"
#define MENU_INFO_TYPE          "Type"
#define MENU_INFO_CHECKSUM      "Checksum"
#define MENU_INFO_TVTYPE        "TV Type"
#define MENU_INFO_FRAMES        "Frames"
#define MENU_INFO_CRC32         "CRC32"
#define MENU_INFO_ENJOY         "Disfruta el passado!"

// Config Joypad menu
#define MENU_CONFIG_TITLE       "Configuracion del mando de GC"
#define MENU_CONFIG_SETUP       "Configuracion "
#define MENU_CONFIG_A           "Boton SNES A - "
#define MENU_CONFIG_B           "Boton SNES B - "
#define MENU_CONFIG_X           "Boton SNES X - "
#define MENU_CONFIG_Y           "Boton SNES Y - "
#define MENU_CONFIG_ANALOG      "Analog Clip   - "
#define MENU_CONFIG_UPDOWN      "Allow U+D / L+R "

// Emulator Options menu
#define MENU_EMU_TITLE          "Opciones Emulador"
#define MENU_EMU_SOUND          "Sounido Eco"
#define MENU_EMU_STEREO         "Inverti Stereo"
#define MENU_EMU_TRANSP         "Transparencias"
#define MENU_EMU_FPS            "Mostrar FPS"
#define MENU_EMU_INTERP         "Interpolar"
#define MENU_EMU_TIMER          "Timer"
#define MENU_EMU_MULTITAP       "MultiTap"

// Credits menu
#define MENU_CREDITS_TITLE      "Snes9x 1.43 - GX Edition v0.1.2beta1"
#define MENU_CREDITS_GCPORT     "GC Port"
#define MENU_CREDITS_GXINFO     "GX Info"
#define MENU_CREDITS_FONT       "Font"
#define MENU_CREDITS_TEST       "Testing"
#define MENU_CREDITS_GREETS     "Greets"
#define MENU_CREDITS_EXTRAS     "Extras"
#define MENU_CREDITS_SUPPORT    "Support"

/*
 * ---------------------------------------------------------------------------
 *  English
 * ---------------------------------------------------------------------------
 */
#else

// Some general menu strings
#define MENU_ON                 "ON"
#define MENU_OFF                "OFF"
#define MENU_EXIT               "Return to previous"

// Main menu
#define MENU_MAIN_PLAY          "Play Game"
#define MENU_MAIN_RESET         "Reset Emulator"
#define MENU_MAIN_LOAD          "Load New Game"
#define MENU_MAIN_SAVE          "Save Manager"
#define MENU_MAIN_INFO          "ROM Information"
#define MENU_MAIN_JOYPADS       "Configure Joypads"
#define MENU_MAIN_OPTIONS       "Emulator Options"
#ifdef HW_RVL
#define MENU_MAIN_RELOAD        "TP Reload"
#define MENU_MAIN_REBOOT        "Reboot Wii"
#else
#define MENU_MAIN_RELOAD        "PSO Reload"
#define MENU_MAIN_REBOOT        "Reboot Gamecube"
#endif
#define MENU_MAIN_CREDITS       "View Credits"

// Media menu
#define MENU_MEDIA_TITLE        "Load a Game"
#define MENU_MEDIA_SDCARD       "Load from SDCard"
#define MENU_MEDIA_DVD          "Load from DVD"
#define MENU_MEDIA_STOPDVD      "Stop DVD Motor"
#define MENU_MEDIA_STOPPING     "Stopping DVD ... Wait"
#define MENU_MEDIA_STOPPED      "DVD Motor Stopped"

// File menu
#define MENU_FILE_TITLE         "Save File Manager"
#define MENU_FILE_MANAGER       "Manager"

// Save menu
#define MENU_SAVE_TITLE         "Manager"
#define MENU_SAVE_SAVE          "Save"
#define MENU_SAVE_LOAD          "Load"
#define MENU_SAVE_DEVICE        "Device"

// Rom Information
#define MENU_INFO_ROM           "ROM"
#define MENU_INFO_ROMID         "ROMID"
#define MENU_INFO_COMPANY       "Company"
#define MENU_INFO_SIZE          "Size"
#define MENU_INFO_SRAM          "SRAM"
#define MENU_INFO_TYPE          "Type"
#define MENU_INFO_CHECKSUM      "Checksum"
#define MENU_INFO_TVTYPE        "TV Type"
#define MENU_INFO_FRAMES        "Frames"
#define MENU_INFO_CRC32         "CRC32"
#define MENU_INFO_ENJOY         "Enjoy the past!"

// Config Joypad menu
#define MENU_CONFIG_TITLE       "Controller Configuration"
#define MENU_CONFIG_SETUP       "SETUP"
#define MENU_CONFIG_A           "SNES Button A -"
#define MENU_CONFIG_B           "SNES Button B -"
#define MENU_CONFIG_X           "SNES Button X -"
#define MENU_CONFIG_Y           "SNES Button Y -"
#define MENU_CONFIG_ANALOG      "Analog Clip   -"
#define MENU_CONFIG_UPDOWN      "Allow U+D / L+R"

// Emulator Options menu
#define MENU_EMU_TITLE          "Emulator Options"
#define MENU_EMU_SOUND          "Sound Echo"
#define MENU_EMU_STEREO         "Reverse Stereo"
#define MENU_EMU_TRANSP         "Transparency"
#define MENU_EMU_FPS            "FPS Display"
#define MENU_EMU_INTERP         "Interpolate"
#define MENU_EMU_TIMER          "Timer"
#define MENU_EMU_MULTITAP       "MultiTap"

// Credits menu
#define MENU_CREDITS_TITLE      "Snes9x 1.43 - GX Edition v0.1.2beta1"
#define MENU_CREDITS_GCPORT     "GC Port"
#define MENU_CREDITS_GXINFO     "GX Info"
#define MENU_CREDITS_FONT       "Font"
#define MENU_CREDITS_TEST       "Testing"
#define MENU_CREDITS_GREETS     "Greets"
#define MENU_CREDITS_EXTRAS     "Extras"
#define MENU_CREDITS_SUPPORT    "Support"

#endif
