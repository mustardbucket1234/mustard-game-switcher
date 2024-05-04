#!/bin/sh

## File is broken and unused
#  if [ -s "$ROM_GO" ]; then
#  	#cat "$ROM_GO" > "$ROM_LAST"
#  
#  	
#  
#  	NAME=$(sed -n '1p' "$ROM_GO")
#  	CORE=$(sed -n '2p' "$ROM_GO" | tr -d '\n')
#  	R_DIR=$(sed -n '4p' "$ROM_GO")$(sed -n '5p' "$ROM_GO")
#  	ROM="$R_DIR"/$(sed -n '6p' "$ROM_GO")
#  
#  
#  	rm "$ROM_GO"
#  
#  	export HOME=/rootWhat 
#  
#  	if [ "$(cat /opt/muos/config/device.txt)" = "RG28XX" ]; then
#  		export SDL_HQ_SCALER=1
#  	fi
#  	
#  	
#  	#if [ -f "/mnt/mmc/MUOS/retroarch/retroarch.cfg" ]; then
#  	#	/opt/muos/script/mux/track.sh "$NAME" retroarch -v -f -c \""/mnt/mmc/MUOS/retroarch/retroarch.cfg"\" -L \""/mnt/mmc/MUOS/core/$CORE"\" \""$ROM"\"
#  	#else
#  		/opt/muos/script/MUX_track.sh "$NAME" /mnt/mmc/MUOS/retroarch -c \""/mnt/mmc/MUOS/.retroarch/retroarch.cfg"\" -L \""/mnt/mmc/MUOS/core/$CORE"\" \""$ROM"\"
#  	#fi
#  fi