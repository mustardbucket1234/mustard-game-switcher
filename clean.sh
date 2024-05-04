# Force current directory to be script directory
cd "$(dirname "$0")"

# Delete output dir
rm -rf output

echo "Cleaned up output directory"
