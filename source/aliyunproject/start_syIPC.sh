#!/bin/sh

watch_sdkDemo_proc()
{
##############watch szy_exe	
	ps | grep  sdkDemo | grep -v  grep >> /dev/null
	if [ $? -eq 1 ]; then
		/mnt/mtd/ipc/sdkDemo & 
	fi
	
}

while [ 1 ]
do
watch_sdkDemo_proc
sleep 5
done


