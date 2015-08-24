#!/bin/sh

rm_aliyunOss.lib_proc
{
	rm /lib/libapr-1.so
	rm /lib/libapr-1.so.0
	rm /lib/libaprutil-1.so
	rm /lib/libaprutil-1.so.0
	rm /lib/libcurl.so
	rm /lib/libcurl.so.4
	rm /lib/libexpat.so
	rm /lib/libexpat.so.0
	rm /lib/libfaac.so
	rm /lib/libfaac.so.0
	rm /lib/libmp4v2.so
	rm /lib/libmp4v2.so.2
	rm /lib/libxml2.so
	rm /lib/libxml2.so.2
	rm /lib/liboss_c_sdk.so
	rm /lib/libutf8proc.so
}

link_aliyunOss.lib_proc
{
	ln -s /mnt/mtd/ipc/tmpfs/lib/libapr-1.so.0.5.2 /lib/libapr-1.so
	ln -s /mnt/mtd/ipc/tmpfs/lib/libapr-1.so.0.5.2 /lib/libapr-1.so.0
	ln -s /mnt/mtd/ipc/tmpfs/lib/libaprutil-1.so.0.5.4 /lib/libaprutil-1.so
	ln -s /mnt/mtd/ipc/tmpfs/lib/libaprutil-1.so.0.5.4 /lib/libaprutil-1.so.0
	ln -s /mnt/mtd/ipc/tmpfs/lib/libcurl.so.4.3.0 /lib/libcurl.so
	ln -s /mnt/mtd/ipc/tmpfs/lib/libcurl.so.4.3.0 /lib/libcurl.so.4
	ln -s /mnt/mtd/ipc/tmpfs/lib/libexpat.so.0.5.0 /lib/libexpat.so
	ln -s /mnt/mtd/ipc/tmpfs/lib/libexpat.so.0.5.0 /lib/libexpat.so.0
	ln -s /mnt/mtd/ipc/tmpfs/lib/libfaac.so.0.0.0 /lib/libfaac.so
	ln -s /mnt/mtd/ipc/tmpfs/lib/libfaac.so.0.0.0 /lib/libfaac.so.0
	ln -s /mnt/mtd/ipc/tmpfs/lib/libmp4v2.so.2.0.0 /lib/libmp4v2.so
	ln -s /mnt/mtd/ipc/tmpfs/lib/libmp4v2.so.2.0.0 /lib/libmp4v2.so.2
	ln -s /mnt/mtd/ipc/tmpfs/lib/libxml2.so.2.9.2 /lib/libxml2.so
	ln -s /mnt/mtd/ipc/tmpfs/lib/libxml2.so.2.9.2 /lib/libxml2.so.2
	ln -s /mnt/mtd/ipc/tmpfs/lib/liboss_c_sdk.so  /lib/liboss_c_sdk.so
	ln -s /mnt/mtd/ipc/tmpfs/lib/libutf8proc.so  /lib/libutf8proc.so

}

watch_aliyunOss.bin_proc()
{
##############watch aliyunOss.bin	
	ps | grep  aliyunOss.bin | grep -v  grep >> /dev/null
	if [ $? -eq 1 ]; then
		tar /mnt/mtd/ipc/aliyunOss.tar.gz -c /mnt/mtd/ipc/tmpfs/
		/mnt/mtd/ipc/tmpfs/aliyunOss.bin & 
		ps | grep  aliyunOss.bin | grep -v  grep >> /dev/null
		if [ $? -eq 1 ]; then
			rm_aliyunOss.lib_proc
			link_aliyunOss.lib_proc
		fi
	fi
	
}

while [ 1 ]
do
watch_aliyunOss.bin_proc
sleep 5
done


