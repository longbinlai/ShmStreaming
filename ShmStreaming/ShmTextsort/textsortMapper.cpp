#include "../HadoopShmTool/SharedMemoryOperator.h"
#include "LineSpliter.h"
#include <sstream>

using namespace std;

stringstream strstream;
string KV;
char* kvpair;
SharedMemoryOperator* reader;
SharedMemoryOperator* writer;

void map(long key, string value){
	LineSpliter * ls = new LineSpliter(value);
	string word = ls->next();
	while( word.length() > 0){
	  	// This should be feed back to the shared memory
		//cout << word << "\t" << 1 << endl;
		strstream << word << "\t" << 1;
		KV = strstream.str();
		kvpair = const_cast<char *>(KV.c_str());
		writer->writeSharedMemory(kvpair, KV.size());
		strstream.str("");
		word = ls->next();
	}
	delete ls;
}


int main(int argc, char * argv[]){
	int rcnt = 0;
	string line;
	try{
		reader = new SharedMemoryOperator(8192);
		writer = new SharedMemoryOperator(8192);
	} catch(exception & ex){
	  	delete(reader);
		delete(writer);
	  	return EXIT_FAILURE;
	}

	int buffersize = reader->getBufferSize();

	// Elements to copy data out of the shared memory delicately
	char* data;
	data = (char *)malloc(buffersize);
	memset(data, '0', buffersize);

	//while(getline(cin, line)){
	while(true){
	  	rcnt = reader->readSharedMemory(data);
		if(rcnt == -1){
		  	writer->writeSharedMemory(NULL, -1);
			break;
		}
		else if(rcnt == 0)
		  	continue;
		line.assign(data, rcnt);
		int splitpos = line.find('\t');
		string ks = line.substr(0, splitpos);
		string vs = line.substr(splitpos + 1);
		long key = atol(ks.c_str());
		string value = vs;
		map(key, value);
	}
	delete(reader);
	delete(writer);
	free(data);
	return EXIT_SUCCESS;
}

