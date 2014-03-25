#! /bin/sh

counter=1
while [ "$counter" -lt 4 ]; do
	echo "*******************************************" >>result.txt
	echo "This is wordcount test, size = 4GB, time to unlock sem = 4" >>result.txt
	echo "*******************************************" >>result.txt
	(time ./hadoop_streaming_shm_wc_4G.sh) 2>>result.txt
	counter=$(($counter+1))
	../../removeSem.sh
	../../removeShm.sh
	sleep 60
done
