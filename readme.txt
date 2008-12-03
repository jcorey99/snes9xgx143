­———————————————————————————————————————————————————————————————————————————­
:::::::::::::::×::::::::::::   .______   :::::::::::::::::::   _   ::::::::::
|        _________            /   ___°/           -------.    (_)'\ /     `°|
×       /______ °   ---__---./   /___ _________  /  ---  /    __| / \      °²
×      _______\ \ /  ___  //  /____//\_____ °  /---/   / ___    ---         ×
|     °________/ /  / /  //  /__    _______\ \    /   /  \  \  / /        .||
::::::::::::::::/   /::--/_______\::.________/::::/   /:­::\   _  \::::::×:::
:::::::°:::::::/___\:::::::::::::::::::::::::::::/   /::::/__/   \--::­::::::
°:::::::::::::::::×:::::::::::::::°::::×:::::::::\--/::::::::::::::::::×:::::
­———————————————————————————————————————————————————————————————————————•ßrK•

×—–­—–­—–­—–­ –­—–­—–­—–­—–­—–­—–­—–­—–­—–­— ­—–­—–­—–­—–­—–­—–­—–­—-­—–­-–•¬
|0O×øo·                  SNES9X v1.43 - GX Edition                    ·oø×O0|
|                       Designed for GameCube Users                         |
|                  http://code.google.com/p/snes9xgx143/                     |
`¨•¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨'

SNES9x GX 1.43 is a Super Nintendo emulator for the Gamecube based on the PC 
emulator SNES9x 1.43 (http://snes9x.ipherswipsite.com/). This project is a
branch of the main SNES9x GX project (http://code.google.com/p/snes9x-gx). 
It shares much of the same code, but offers the older 1.43 core, which gives 
a performance boost to GameCube users for those core-heavy games.

[ Features ]

* Port of SNES9X v1.43
* Gamecube, Wiimote, Nunchuk, Classic, controller support
* Auto Load/Save Game Snapshots and SRAM
* Custom controller configurations
* SD, USB, DVD (requires DVDx), SMB, GC Memory Card, Zip, and 7z support
* Autodetect PAL/NTSC, 16:9 widescreen support
* Original/filtered/unfiltered video modes
* Turbo Mode - up to 2x the normal speed
* Zoom option to zoom in/out
* Open Source!

×—–­—–­—–­—–­ –­—–­—–­—–­—–­—–­—–­—–­—–­—–­— ­—–­—–­—–­—–­—–­—–­—–­—-­—–­-–•¬
|0O×øo·                         UPDATE HISTORY                        ·oø×O0|
`¨•¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨'

Version 0.2.0
+[Tantric]
- Complete code replacement - ported Snes9x 1.43 core into Snes9x GX 007. 
- Features added:
  * Wiimote, Nunchuk, and Classic controller support
  * Button mapping for all controller types
  * Full support for SD, USB, SMB, DVD, GameCube MC
  * Load/save preference selector. ROMs, saves, and preferences are 
    saved/loaded according to these
  * 'Auto' settings for save/load - attempts to automatically determine
    your load/save device(s)
  * Preferences are loaded and saved in XML format. You can open
    snes9x143/settings.xml edit all settings, including some not 
    available within the program
  * Turbo mode, video zooming option
  * Original & Unfiltered video options
  * Widescreen support
  * 7z support
  * And more!

Version 0.1.1 [20080417]
+[Askot]
- Added Save state in Zip mode for SD Gecko and Memory Card. Source mainly taken
  from SnesGX 2.0b8 [beta]
- Fixed a sound glitch every time SDReload was used.

+[dsbomb]
- GC: fixed DVD support, can read ROMs and ZIPs.
- Removed certain menu options that are not usable in either Wii or GC mode
- Proper Wii reboot support
- Left/right on the dpad are usable on many menu items.
- WiiSD write support for SRAM, Save states

Version 0.1.0 [20080331]

+[Askot]
- Fixed/changed SDCARD slot selection for searching roms, at 
  start you will be prompted for this option.
- Code cleanup.

+[dsbomb]
- Added Wii mode support.
- Joystick fixes due to libogc r14's changed stick values
- Rearranged menu to make more sense, and consistent with FCEU
- Add "Reboot" menu option
- Removed "." directory from SD card listing, it's pointless
- Expand DVD reading to DVD9 size (one DVDs are working again)
- Added option to go back a menu by pressing B.

Version 0.0.9 [20080325]
- Added SDCARD slot selection for searching roms, meaning, you can search roms 
  from SDCARD SLOT A or SLOT B (Beta, meaning, buggy, but works).
- For standarization, you must create folders root:\snes9x\roms to read SNES 
  roms files from SDCARD.
- Added combo L+R+X+Y to call menu.

Version 0.0.8 [20080222]
- Fixed displaying information about a ROM file (oops!)
- Added saving SRAM file in SD Card with CRC32 ROM data, no LFN format.
- Menu in saving SRAM file changed to choose SLOT, DEVICE, Save SRAM, 
  Load SRAM, Return to previous.

Version 0.0.7 [20080127]
- Add SD card subdirectory browsing:
  + Set the cursor over [..] and push A button to browse.

- Modified controls when going into the rom selection menu (DVD or 
  SDCARD):  [ala eke-eke style]
  + Use B to quit selection list.
  + Use L/R triggrers or Pad Left/Right to go down/up one full page.

- Some menu rearrangment and a little of sourcecode cleanup: 
  + Everytime you pressed B button on any option, playgame started, not anymore
  until you select Play Game option.

Version 0.0.6 [20080121]

The changes are:
- Compiled with libOGC 20070214, meaning: can read SD Card Up to 2GB
- Fixed L & R buttons
- Fixed Multitap 5 set to Off
- Added auto reset after loading a SRAM file
- Reading files from SD Card it's faster, now they're called from cache
  after first reading.

×—–­—–­—–­—–­ –­—–­—–­—–­—–­—–­—–­—–­—–­—–­— ­—–­—–­—–­—–­—–­—–­—–­—-­—–­-–•¬
|0O×øo·                        PARTIAL PKZIP SUPPORT                  ·oø×O0|
`¨•¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨'

The Zip support in the emulator is courtesy of the zlib library. Currently,
it supports only the Inflate method.

The good news is that this is by far the most common zip method!

You should note also that this implementation assumes that the first file
in each archive is the required rom in smc/fig format.

In an attempt to save you time, we recommend using 7-Zip as the compressor,
as it gives a gain of 5-25% better compression over standard zips.

To use 7-Zip compression on either linux or windows, use the following,

  7za a -tzip -mx=9 myrom.zip myrom.smc

×—–­—–­—–­—–­ –­—–­—–­—–­—–­—–­—–­—–­—–­—–­— ­—–­—–­—–­—–­—–­—–­—–­—-­—–­-–•¬
|0O×øo·                         SETUP & INSTALLATION                  ·oø×O0|
`¨•¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨'

Unzip the archive into a folder with at least one SNES ROM image. Images must
be in the Super Magicom (SMC) or FIG format. Generally, all images you find
will be in this format, but if you run across one that isn't please download
RTOOL which will allow you to convert the image into SMC format.

After this you're going to need to inject a ROM image into the emulator
before you can get it up and running. To be fair to everyone including those
of you who do not have a spare DVD we've adopted this approach.

SNESGX supports ROM images up to 8Mb (64 megabit) from DVD, or 4Mb (32 megabit)
in a dol, so you shouldn't have to worry about what size the games are before you
load them, but since the saving and loading of your progress is based on the
internal ROM name (header) itself you may want to check to see if the ROM header
is intact, but don't worry about this as most ROM images have correct headers.

Next, for all you non-commandos out there here is a quick guide on how to
create the initial DOL (GameCube Executable File) you will need to load on
your system. If you're already familiar with the Command Line Interface (CLI)
simply skip to step 6. Note when I use '' to only type what is in between
them.

  (1) Unzip archive into c:\snes\
  (2) Put GAME.SMC into c:\snes\
      GAME.SMC can be any game you want
  (3) Click on the START MENU then RUN and type in 'CMD' and press ENTER
  (4) Type 'C:' press ENTER
  (5) Type 'CD\SNES' press ENTER
  (6) Type 'sgxi.exe GAME.SMC GAME.DOL' press ENTER

Now you will have a GAME.DOL file which you can burn onto a DVD and/or stream
to your GameCube. If you're unfamiliar with streaming and/or burning a DVD
please refer to the Mod That Cube -FAQ- for further instructions.

  http://modthatcube.pxn-os.com

×—–­—–­—–­—–­ –­—–­—–­—–­—–­—–­—–­—–­—–­—–­— ­—–­—–­—–­—–­—–­—–­—–­—-­—–­-–•¬
|0O×øo·                              MAIN MENU                        ·oø×O0|
`¨•¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨'

Once the DOL file is loaded you will be presented with the configuration
screen where you can adjust many options before you start the gameplay.
Additionally, if at any time your heart desires to alter these options, save
your game or load a new ROM image simply press the Z and the left trigger
button (at the same time) and you'll be brought back to this menu.

To leave the options at their default and continue to play the game simply
press the 'A' button and enjoy!

×—–­—–­—–­—–­ –­—–­—–­—–­—–­—–­—–­—–­—–­—–­— ­—–­—–­—–­—–­—–­—–­—–­—-­—–­-–•¬
|0O×øo·                            CONFIGURATION                      ·oø×O0|
`¨•¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨'

To load the configuration menu press Z + LEFT TRIGGER at the same time and
you can save/load you current progress, load a new game from DVD and/or tweak
the emulators options.

Save Game Manager

  Will bring you into the management system where you can Save or Load your
  current game progress. You may SAVE you game progress at any time, but when
  loading your SAVE please reload your game, load your SAVE and then play the
  game. If you try loading your SAVE after the game starts you may run into
  some nasty problems.

Load New ROM

  If you're tired of playing your current game pick this option
  so you can browse the current DVD in the GameCube and select a new game to
  play.

Pad Configuration

  Don't like the way the controller is setup (mapped)? Simply pick this
  option and you can chose which button does what.

Emulator Options

  Here you can tweak some of the internal Snes9x emulator options

  Sound Echo

    Will add a pseudo echo effect to all sounds creating a more
    in depth gameplay experience.

  Reverse Stereo

    Change the audio output from LEFT and RIGHT to RIGHT and LEFT.

  Transparency

    Some games, such as Terranigma (a great role playing game only
    released in the United Kingdom) include transparency effects. Use this
    option to turn these off or on.

  FPS Display

    This option will display the Frames Per Second (FPS) in the bottom left
    hand corner so you can see how accurate the graphics are being displayed
    on the screen.

  Sound Sync

    Snes9x allows for synchronising the sound with the emulator speed.

  Timer

    The original timing in Snes9x is based on the vertical blank counter.
    You now also have the option to use a crude clock timer, which may help
    playing PAL50 on a 60hz output.

  Reset Emulator

    Perform a soft-reset on the Snes9x core.

PSO/SD Reload

  If you're using Phantasy Star Online (PSO) or SD-Load (SD) to boot SNESGX
  from then you can use this option to go back to the loading menu to select
  another GameCube executable (DOL/ELF) file to load.

View Credits

  You can view the credits for the emulator. The credits can also be viewed
  at the end of this document.

ROM Information

  Here you can double check the ROM information. If SRAM == 0, you have no
  save available.

Stop DVD Motor

  As it says, once you know you're in for the long haul, save your drive
  motor by switching it off

×—–­—–­—–­—–­ –­—–­—–­—–­—–­—–­—–­—–­—–­—–­— ­—–­—–­—–­—–­—–­—–­—–­—-­—–­-–•¬
|0O×øo·                               CREDITS                         ·oø×O0|
`¨•¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨¨ ¨¨¨¨¨¨¨¨¨¨¨¨¨'

                                  Technical Credits

                           Snes9x GX 1.43 - Askot & dsbomb
                          Snes9x GX - Tantric & michniewski
                          Original GameCube Port - SoftDev
                             Snes9x v1.4.3 - Snes9x Team
                            GX - http://www.gc-linux.org
                                  libogc - Shagkur
