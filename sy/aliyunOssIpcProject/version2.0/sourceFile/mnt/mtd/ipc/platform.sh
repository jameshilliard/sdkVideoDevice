#! /bin/sh

MAPINI="/mnt/mtd/ipc/conf/config_platform.ini"
OVINI="/mnt/mtd/ipc/conf/config_onvif.ini"

while [ 1 ]
do
	sleep 2
	RES=`ps | grep -c ipc_server`
	if [ $RES -gt 1 ]
	then
		break
	fi
done

sleep 10

enable=`grep oenable $MAPINI | awk -F "\"" '{print $2}'` 
if [ $enable -eq 1 ]
then
    /mnt/mtd/ipc/proxy &
fi

ovenable=`grep ovenable $OVINI | awk -F "\"" '{print $2}'` 
if [ $ovenable -eq 1 ]
then
    /mnt/mtd/ipc/tmpfs/onvif &
fi

tenable=`grep tenable $MAPINI | awk -F "\"" '{print $2}'` 
if [ $tenable -eq 1 ]
then
    /mnt/mtd/ipc/tutk &
fi
/mnt/mtd/ipc/uipcam &