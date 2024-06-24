#!/bin/bash

# Clean the build
./clean.sh

# Build the app for ARM 7
make CONFIG_ARM7=1

# Delete dist/.MustardGameSwitcher/bin folder
rm -rf dist/mnt/mmc/MUOS/application/.MustardGameSwitcher/bin
sleep 1

# Delete previous backup
rm archive/MustardGameSwitcher_old.zip
sleep 1
echo "**Deleted previous build**"

# Backup the existing archive file to _old
mv archive/MustardGameSwitcher.zip archive/MustardGameSwitcher_old.zip
echo "**Backup the existing build**"

# Create the directory structure
mkdir -p dist/mnt/mmc/MUOS/application/.MustardGameSwitcher/bin
cp -r output/arm7/bin dist/mnt/mmc/MUOS/application/.MustardGameSwitcher/bin
echo "**Copied the files to the directory structure**"

# Zip the directory structure
cd dist
zip -r MustardGameSwitcher.zip mnt
cd ..
sleep 2
echo "**Archived the directory structure**"

# Move the zip file to the archive folder
mv dist/MustardGameSwitcher.zip archive

echo "**Finished Building for arm7**"
