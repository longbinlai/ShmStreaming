hadoop fs -rmr result
hadoop jar /home/robeen/FiFoMapper/hadoop-0.21.0/mapred/build/contrib/streaming/hadoop-0.21.0-streaming.jar \
  -libjars /home/robeen/hadoop_streaming/invertedindex/java/invertedindex.jar \
  -input index \
  -output result \
  -inputformat robeen.TokenInputFormat \
  -mapper "/home/robeen/ShmStreaming/ShmInvertedIndex/invertedIndexMapper 4096" \
  -combiner /home/robeen/ShmStreaming/ShmInvertedIndex/invertedIndexReducer \
  -reducer /home/robeen/ShmStreaming/Shm/invertedIndexReducer \
  -jobconf stream.map.input=shm \
  -jobconf stream.map.output=shm \
  -jobconf stream.shm.buffersize=4096 \
  -jobconf stream.shm.map.input.unlock.empty.semaphore=100 \
  -jobconf stream.shm.map.input.unlock.full.semaphore=100 \
  -jobconf stream.shm.map.output.unlock.empty.semaphore=60 \
  -jobconf stream.shm.map.output.unlock.full.semaphore=60 \
	
