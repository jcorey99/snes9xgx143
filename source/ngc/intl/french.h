/*
 * ---------------------------------------------------------------------------
 *  French by JyCet
 *  "stéréo" and "vidéo" bug with GetTextWidth ?
 * ---------------------------------------------------------------------------
 */

// Some general menu strings
#define MENU_ON                 "ON"
#define MENU_OFF                "OFF"
#define MENU_EXIT               "Menu précédant"
#define MENU_PRESS_A            "Appuyer sur A pour continuer"

// Main menu
#define MENU_MAIN_PLAY          "Lancer la ROM"
#define MENU_MAIN_RESET         "Reset émulateur"
#define MENU_MAIN_LOAD          "Charger une ROM"
#define MENU_MAIN_SAVE          "Gestion des sauvegardes"
#define MENU_MAIN_INFO          "Information de la ROM"
#define MENU_MAIN_JOYPADS       "Configuration manettes"
#define MENU_MAIN_OPTIONS       "Options d'émulation"
#ifdef __wii__
#define MENU_MAIN_RELOAD        "Recharger TP"
#define MENU_MAIN_REBOOT        "Redémarrer la Wii"
#else
#define MENU_MAIN_RELOAD        "Recharger PSO"
#define MENU_MAIN_REBOOT        "Redémarrer la Gamecube"
#endif
#define MENU_MAIN_CREDITS       "Voir les crédits"

// Media menu
#define MENU_MEDIA_TITLE        "Charger une ROM"
#define MENU_MEDIA_SDCARD       "Charger depuis la SDCard"
#define MENU_MEDIA_DVD          "Charger depuis le DVD"
#define MENU_MEDIA_STOPDVD      "Arrêter le moteur DVD"
#define MENU_MEDIA_STOPPING     "Arrêt du DVD ... Patienter"
#define MENU_MEDIA_STOPPED      "Moteur DVD arreté"

// File menu
#define MENU_FILE_TITLE         "Gestion sauvegarde de fichier"
#define MENU_FILE_MANAGER       "Gestion"

// Save menu
#define MENU_SAVE_TITLE         "Gestionnaire"
#define MENU_SAVE_SAVE          "Sauvegarde"
#define MENU_SAVE_LOAD          "Chargement"
#define MENU_SAVE_DEVICE        "Périphérique"

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
#define MENU_INFO_ENJOY         "Profitez du passé!"

// Config Joypad menu
#define MENU_CONFIG_TITLE       "Configuration manette"
#define MENU_CONFIG_SETUP       "Configuration"
#define MENU_CONFIG_A           "SNES bouton A -"
#define MENU_CONFIG_B           "SNES bouton B -"
#define MENU_CONFIG_X           "SNES bouton X -"
#define MENU_CONFIG_Y           "SNES bouton Y -"
#define MENU_CONFIG_ANALOG      "Offset analogique -"
#define MENU_CONFIG_UPDOWN      "Autoriser U+D / L+R"

// Emulator Options menu
#define MENU_EMU_TITLE          "Options d'émulation"
#define MENU_EMU_SOUND          "Echo sonore"
#define MENU_EMU_STEREO         "Inverser stereo"
#define MENU_EMU_TRANSP         "Transparence"
#define MENU_EMU_FPS            "Affichage FPS "
#define MENU_EMU_INTERP         "Interpolation sonore"
#define MENU_EMU_TIMER          "Synchronisation video"
#define MENU_EMU_MULTITAP       "MultiTap"

// Credits menu
#define MENU_CREDITS_TITLE      MENU_SNES9X_TITLE
#define MENU_CREDITS_GCPORT     "GC Port"
#define MENU_CREDITS_GXINFO     "GX Info"
#define MENU_CREDITS_FONT       "Police"
#define MENU_CREDITS_TEST       "Testeurs"
#define MENU_CREDITS_GREETS     "Remerciements"
#define MENU_CREDITS_EXTRAS     "Extras"
#define MENU_CREDITS_SUPPORT    "Support"
