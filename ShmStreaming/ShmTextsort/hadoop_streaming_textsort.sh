hadoop fs -rmr result
hadoop jar /home/epcc/robeen/hadoop-0.21.0-streaming.jar \
  -input 2GB.txt \
  -output ./result \
  -mapper "/home/epcc/robeen/ShmStreaming/ShmTextsort/textsortMapper" \
  -reducer "/home/epcc/robeen/ShmStreaming/ShmTextsort/textsortReducer" \
  -jobconf stream.map.input=shm \
  -jobconf stream.map.output=shm \
  -jobconf stream.reduce.output=shm \
  -jobconf stream.shm.buffersize=8192 \
  -jobconf stream.shm.map.input.unlock.empty.semaphore=100 \
  -jobconf stream.shm.map.input.unlock.full.semaphore=250 \
  -jobconf stream.shm.reduce.output.unlock.empty.semaphore=300 \
  -jobconf stream.shm.reduce.output.unlock.full.semaphore=300 \
  -jobconf mapred.reduce.tasks=2

