#!/usr/bin/bash
while read line
do
	t=$(echo "$line" | awk '{print $1}')
	k=$(echo "$line" | awk '{print $2}')
	d=$(echo "$line" | awk '{print $3}')
	case $t in
	+)
		echo "$k"
		;;
	-)
		echo "$k $d"
		;;
	esac
done < "${1:-/dev/stdin}"
