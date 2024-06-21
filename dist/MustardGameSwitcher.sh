#!/bin/sh

# muOS v11 compatibility
if [ -d "/usr/lib32" ]; then
    export LD_LIBRARY_PATH=/usr/lib32
fi

export HOME=/root

ROM_LAST=/tmp/rom_last
LAST_PLAY="/opt/muos/config/lastplay.txt"

cd "$(realpath "$(dirname "$0")")"
cd ".data"

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