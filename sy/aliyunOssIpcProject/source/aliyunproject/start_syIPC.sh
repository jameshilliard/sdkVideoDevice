#!/bin/sh

ln -s /mnt/mtd/ipc/tmpfs/lib/libapr-1.so.0.5.2 libapr-1.so
ln -s /mnt/mtd/ipc/tmpfs/lib/libapr-1.so.0.5.2 libapr-1.so.0
ln -s /mnt/mtd/ipc/tmpfs/lib/libaprutil-1.so.0.5.4 libaprutil-1.so
ln -s /mnt/mtd/ipc/tmpfs/lib/libaprutil-1.so.0.5.4 libaprutil-1.so.0
ln -s /mnt/mtd/ipc/tmpfs/lib/libcurl.so.4.3.0 libcurl.so
ln -s /mnt/mtd/ipc/tmpfs/lib/libcurl.so.4.3.0 libcurl.so.4
ln -s /mnt/mtd/ipc/tmpfs/lib/libexpat.so.0.5.0 libexpat.so
ln -s /mnt/mtd/ipc/tmpfs/lib/libexpat.so.0.5.0 libexpat.so.0
ln -s /mnt/mtd/ipc/tmpfs/lib/libfaac.so.0.0.0 libfaac.so
ln -s /mnt/mtd/ipc/tmpfs/lib/libfaac.so.0.0.0 libfaac.so.0
ln -s /mnt/mtd/ipc/tmpfs/lib/libmp4v2.so.2.0.0 libmp4v2.so
ln -s /mnt/mtd/ipc/tmpfs/lib/libmp4v2.so.2.0.0 libmp4v2.so.2
ln -s /mnt/mtd/ipc/tmpfs/lib/libxml2.so.2.9.2 libxml2.so
ln -s /mnt/mtd/ipc/tmpfs/lib/libxml2.so.2.9.2 libxml2.so.2
ln -s /mnt/mtd/ipc/tmpfs/lib/liboss_c_sdk.so  liboss_c_sdk.so
ln -s /mnt/mtd/ipc/tmpfs/lib/libutf8proc.so  libutf8proc.so

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


