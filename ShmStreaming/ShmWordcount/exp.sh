#! /bin/sh

counter=1
while [ "$counter" -lt 5 ]; do
	echo "*******************************************" >>result.txt
	echo "This is wordcount test, size = 4GB" >>result.txt
	echo "*******************************************" >>result.txt
	(time ./hadoop_streaming_shm_wc_4GB.sh) 2>>result.txt
	counter=$(($counter+1))
	sleep 60
done
