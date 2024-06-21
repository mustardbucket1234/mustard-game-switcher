#!/bin/sh

# muOS v11 compatibility
if [ -d "/usr/lib32" ]; then
    export LD_LIBRARY_PATH=/usr/lib32
fi

export HOME=/root

# Set the working directory to the directory of this script
cd "$(realpath "$(dirname "$0")")"


# Ensure logfile is empty or create a new one
# Redirect all output (stdout and stderr) to the logfile
echo -n "" > logfile.txt
exec > logfile.txt 2>&1


echo "STARTUP HAS RUN!"
echo "rid: $((RANDOM % 9999))"
echo "$(date)"


echo "RUNNING GAME SWITCHER"

sleep 10

"/mnt/sdcard/ROMS/Apps/MustardGameSwitcher.sh"

echo "FINISHED RUNNING GAME SWITCHER"
echo "STARTUP FINISHED!"