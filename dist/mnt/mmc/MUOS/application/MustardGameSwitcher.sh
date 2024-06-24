#!/bin/sh

if pgrep -f "playbgm.sh" > /dev/null; then
	killall -q "playbgm.sh"
	killall -q "mp3play"
fi

if pgrep -f "muplay" > /dev/null; then
	kill -9 "muplay"
	rm "$SND_PIPE"
fi

echo app > /tmp/act_go

. /opt/muos/script/system/parse.sh

DEVICE=$(tr '[:upper:]' '[:lower:]' < "/opt/muos/config/device.txt")
DEVICE_CONFIG="/opt/muos/device/$DEVICE/config.ini"

STORE_ROM=$(parse_ini "$DEVICE_CONFIG" "storage.rom" "mount")
SDL_SCALER=$(parse_ini "$DEVICE_CONFIG" "sdl" "scaler")

MGS_DIR="$STORE_ROM/MUOS/application/.MustardGameSwitcher"

cd "$MGS_DIR" || exit

export SDL_GAMECONTROLLERCONFIG_FILE="/usr/lib32/gamecontrollerdb.txt"
export HOME=/root
export LD_LIBRARY_PATH=/usr/lib32
export ROM_LAST=/tmp/rom_last
export LAST_PLAY="/opt/muos/config/lastplay.txt"

# # Get the data of the current script
# # Ensure the data is not empty
# # Store it in next_play.txt
# if [ -f "$ROM_LAST" ]; then
#     ROM_LAST_DATA=$(cat "$ROM_LAST")
#     if [ ${#ROM_LAST_DATA} -gt 15 ]; then
#         echo "$ROM_LAST_DATA" > "$LAST_PLAY"
#         echo "$ROM_LAST_DATA" > "rom_last.txt"
#     fi
# fi

# Launch app executable and wait for it to finish
# Store the exit code in APP_EXIT_CODE
cd "bin"
./app "$(realpath "$0")" &
APP_PID=$!
wait $APP_PID
APP_EXIT_CODE=$?
