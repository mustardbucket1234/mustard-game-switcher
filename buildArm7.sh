./clean.sh

# Run this to build the app for ARM 7
make CONFIG_ARM7=1

# Delate dist/.data folder
rm -rf dist/.data/bin

echo "Deleted dist/.data folder"
sleep 1


mkdir -p dist/.data
# Copy the output/arm7/bin folder to dist/.data/bin
cp -r output/arm7/bin dist/.data/bin


echo "Finished Building for arm7"