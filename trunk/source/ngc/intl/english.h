/*
 * ---------------------------------------------------------------------------
 *  English by dsbomb
 * ---------------------------------------------------------------------------
 */

// Some general menu strings
#define MENU_ON                 "ON"
#define MENU_OFF                "OFF"
#define MENU_EXIT               "Return to Previous"
#define MENU_PRESS_A            "Press A to Continue"

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
#define MENU_CREDITS_TITLE      MENU_SNES9X_TITLE
#define MENU_CREDITS_GCPORT     "GC Port"
#define MENU_CREDITS_GXINFO     "GX Info"
#define MENU_CREDITS_FONT       "Font"
#define MENU_CREDITS_TEST       "Testing"
#define MENU_CREDITS_GREETS     "Greets"
#define MENU_CREDITS_EXTRAS     "Extras"
#define MENU_CREDITS_SUPPORT    "Support"

