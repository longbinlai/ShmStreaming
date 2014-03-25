#! /bin/sh

counter=1
while [ "$counter" -lt 5 ]; do
	echo "*******************************************" >>result.txt
	echo "This is grep test, size = 4GB" >>result.txt
	echo "*******************************************" >>result.txt
	(time ./hadoop_streaming_shm_grep_4GB.sh) 2>>result.txt
	counter=$(($counter+1))
	../removeShm.sh
	../removeSem.sh
	sleep 60
done
counter=1
while [ "$counter" -lt 5 ]; do
	echo "*******************************************" >>result.txt
	echo "This is grep test, size = 6GB" >>result.txt
	echo "*******************************************" >>result.txt
	(time ./hadoop_streaming_shm_grep_6GB.sh) 2>>result.txt
	counter=$(($counter+1))
	../removeShm.sh
	../removeSem.sh
	sleep 60
done
counter=1
while [ "$counter" -lt 5 ]; do
	echo "*******************************************" >>result.txt
	echo "This is grep test, size = 8GB" >>result.txt
	echo "*******************************************" >>result.txt
	(time ./hadoop_streaming_shm_grep_8GB.sh) 2>>result.txt
	counter=$(($counter+1))
	../removeShm.sh
	../removeSem.sh
	sleep 60
done
