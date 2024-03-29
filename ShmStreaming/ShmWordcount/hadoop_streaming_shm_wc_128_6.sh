hadoop jar /home/epcc/robeen/hadoop-0.21.0-streaming.jar \
  -input file_6GB_128_6 \
  -output ./result \
  -mapper "/home/epcc/robeen/ShmStreaming/ShmWordcount/wordcountMapper 4096"\
  -reducer /home/epcc/robeen/ShmStreaming/ShmWordcount/wordcountReducer \
  -jobconf stream.map.input=shm \
  -jobconf stream.map.output=shm \
  -jobconf stream.shm.buffersize=4096 \
  -jobconf stream.shm.map.input.unlock.empty.semaphore=50 \
  -jobconf stream.shm.map.input.unlock.full.semaphore=300 \
  -jobconf stream.shm.map.output.unlock.empty.semaphore=600 \
  -jobconf stream.shm.map.output.unlock.full.semaphore=600
