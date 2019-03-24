#!/usr/bin/bash
while read line
do
	t=$(echo "$line" | awk '{print $1}')
	h=$(echo "$line" | awk '{print $2}')
	k=$(echo "$line" | awk '{print $3}')
	case $t in
	"P")
		echo "$k"
		;;
	"R")
		d=$(echo "$line" | awk '{print $4}')
		echo "$k $d"
		;;
	"H")
		;;
	*)
		;;
	esac
done < "${1:-/dev/stdin}"
