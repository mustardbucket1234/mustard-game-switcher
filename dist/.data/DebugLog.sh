#!/bin/sh

# Change directory to the script location
cd $(realpath "$(dirname "$0")")
cd ..
# Run the script
"./Game Switcher.sh" > log.txt