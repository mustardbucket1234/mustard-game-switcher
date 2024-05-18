#!/bin/sh

# muOS v11 compatibility
if [ -d "/usr/lib32" ]; then
    export LD_LIBRARY_PATH=/usr/lib32
fi

# Set the working directory to the directory of this script
cd "$(realpath "$(dirname "$0")")"


# Ensure logfile is empty or create a new one
# Redirect all output (stdout and stderr) to the logfile
echo -n "" > logfile.txt
exec > logfile.txt 2>&1


echo "STARTUP HAS RUN!"
echo "$(date)"
echo "STARTUP FINISHED!"