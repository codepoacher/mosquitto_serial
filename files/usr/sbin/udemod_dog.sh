#!/bin/sh
local ret
sleep 120
while :
do
	ret=`pidof udemod 2>/dev/null`
	[ -z "$ret" ] && {
	/etc/init.d/dtu restart
	}	
	sleep 30
done

