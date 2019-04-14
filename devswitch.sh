#!/usr/bin/sh

if [ $# -lt 2 ]
then
	echo "Usage: $(basename $0) <device> enable/disable"
	exit
fi

device=$1
what=$2

devices=$(xinput --list | grep -Eo 'id=[[:digit:]]+' | cut -d '=' -f 2)
for d in $devices
do
	if test -n "$(xinput --list-props $d | grep "$device")"
	then
		case $what in
		enable)
			xinput --enable $d
			;;
		disable)
			xinput --disable $d
			;;
		esac
		break
	fi
done
