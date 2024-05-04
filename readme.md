# GameSwitcher Protoype for RG35XX+ muOS v10

A barebones GameSwitcher app for muOS v10 inspired by onionOS

The GameSwitcher allows you to preview your recent Retroarch games and jump between them.
It also allows for setting up a quick save, shutoff, and resume feature (with some configuration required)

This is definitely more of a demo than a usable product, and it *MIGHT* break your muOS install. So backup your saves / games before installing.

# How To Install:
1. Ensure you have muOS v10 installed and you're running it on a RG35XX+
2. Download this repository as a ZIP
3. Move the `/dist` folder of the ZIP into the `ROMS` folder of your RG35XX+ SD card. Rename `/dist` as desired.
4. Navigate to the GameSwitcher on your RG35XX+, and launch it as *External - Ports* (press SELECT if it doesn't work automatically)

After launching, you should now see your recent Retroarch games show up.


# Additional Configuration:

If you want Shutoff / Resume to work, you will want to do some additional configuration 

1. From the muOS main menu, open `Configuration -> General Settings` and set Device Startup to `Last Game Played`.
This will allow you to resume the GameSwitcher when you power the device down

2. From the muOS main menu, open `Retroarch`. Then go into `Settings -> Saving` and toggle

- Save State Thumbnails: ON
- Sort Save States into Folders by Core Name: ON
- Save State Thumbnails: ON
- Auto Save State: ON
- Auto Load State: ON

In `Settings -> Input -> Hotkeys` configure a Quit button. I have mine set to `Menu + Select`

Once you've completed this, make sure to save your configuration (`Configuration File -> Save Current Configuration`)

# Controls:
- MENU (Hold) *or* MENU + SELECT: Shutoff device
- X: Exit GameSwitcher
- A: Launch Game
- Left/Right: Navigate recent games


# How To Build

Make sure your PC is running X86 Linux (or wsl like I am). Extract the RG35xx+ Batocera toolchain to /opt/rg35xx, then run `./buildArm7.sh`
Download Link: https://github.com/rg35xx-cfw/rg35xx-cfw.github.io/releases/tag/rg35xx_plus_h_sdk_20240207

You can also test the app on your PC by running `./buildX86.sh`, just make sure you have SDL2 and all other packages installed.
You will also need to copy some of the files onto `/mnt/muOSDump/` to test it properly.