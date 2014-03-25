hadoop fs -rmr result
hadoop jar /home/epcc/robeen/hadoop-0.21.0-streaming.jar \
  -input 20GB.txt \
  -output ./result \
  -mapper /home/epcc/robeen/ShmStreaming/ShmWordcount/wordcountMapper \
  -combiner /home/epcc/robeen/ShmStreaming/ShmWordcount/wordcountReducer \
  -reducer /home/epcc/robeen/ShmStreaming/ShmWordcount/wordcountReducer \
  -jobconf stream.map.input=shm \
  -jobconf stream.map.output=shm \
  -jobconf stream.shm.buffersize=8192 \
  -jobconf stream.shm.map.input.unlock.empty.semaphore=100 \
  -jobconf stream.shm.map.input.unlock.full.semaphore=300 
	
