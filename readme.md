# GameSwitcher Protoype for RG35XX+ muOS v10

A GameSwitcher app for muOS v10 inspired by onionOS

<div>
<img src="https://i.imgur.com/a1uIJaF.png" width="300">
<img src="https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExeG1oMjY3ZHNubnpnMTJpMmg5MGUwemcwenJ2eTZxY3lpdDZheGhydCZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/Pq4IfxNQF8FO6e1x0g/source.gif" width="300">
</div>

The GameSwitcher allows you to launch your Recent/Favorite Retroarch games and jump between them.
It also allows for setting up a quick save, shutoff, and resume feature (with some configuration required)

This app was developed rapidly so it may have bugs. Backup your saves / games before installing.

# How To Install:
1. Ensure you have muOS v10 installed and you're running it on a RG35XX+
2. Download the latest release ZIP (https://github.com/mustardbucket1234/mustard-game-switcher/releases/tag/release)
3. Copy the contents of the ZIP into the `ROMS` folder of your RG35XX+ SD card 
4. Navigate to the GameSwitcher on your RG35XX+, and launch it as *External - Ports* (press SELECT if it doesn't work automatically)

After launching, you should now see your recent Retroarch games show up.


# Additional Configuration:

If you want Shutoff / Resume to work, you will want to do some additional configuration 

1. From the muOS main menu, open `Configuration -> General Settings` and set Device Startup to `Last Game Played`.
This will allow you to resume the GameSwitcher when you power the device down

2. From the muOS main menu, open `Retroarch`. Then go into `Settings -> Saving` and toggle

- `Save State Thumbnails: ON`
- `Sort Save States into Folders by Core Name: ON`
- `Save State Thumbnails: ON`
- `Auto Save State: ON`
- `Auto Load State: ON`

In `Settings -> Input -> Hotkeys` configure a Quit button. I have mine set to `Menu + Select`

Once you've completed this, make sure to save your configuration (`Configuration File -> Save Current Configuration`)

# Controls:

If you followed the configuration guide, you can now open games from the GameSwitcher by pressing `A`, and close them by pressing `MENU + SELECT`.
Keep `MENU + SELECT` held to power off. The next time you power on, you should be back to the GameSwitcher screen with your last played save.  

- `MENU (Hold) or MENU + SELECT`: Shutoff device
- `X`: Exit GameSwitcher
- `B`: Swap to list view
- `A`: Launch Game
- `Left/Right`: Navigate recent games


# How To Build

Make sure your PC is running X86 Linux (or wsl like I am). Extract the RG35xx+ Batocera toolchain to /opt/rg35xx, then run `./buildArm7.sh`
Download Link: https://github.com/rg35xx-cfw/rg35xx-cfw.github.io/releases/tag/rg35xx_plus_h_sdk_20240207

You can also test the app on your PC by running `./buildX86.sh`, just make sure you have SDL2 and all other packages installed.
You will also need to copy some of the files onto `/mnt/muOSDump/` to test it properly.