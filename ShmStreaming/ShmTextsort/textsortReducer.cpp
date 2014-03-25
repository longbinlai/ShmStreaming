#include <sstream>
#include <cstdlib>
#include <map>
#include "../HadoopShmTool/SharedMemoryOperator.h"

using namespace std;

static int buffersize = 8192;
stringstream ss;
string KV;
char* kvpair;
SharedMemoryOperator* reader;
SharedMemoryOperator* writer;
int times;
map<string, long> KVMap;
map<string, long>::iterator it;

void reduce();

int main(int argc, char * argv[]){
	char line[1024];
	string key;
	long value;
	string *sline;
	string skey;
	string svalue;

	times = 0;

	try{
	  	//reader = new SharedMemoryOperator(8192);
		writer = new SharedMemoryOperator(buffersize);
	} catch(exception & ex){
	  	//delete(reader);
		delete(writer);
		return EXIT_FAILURE;
	}

	while( cin.getline(line, 1024) ){
		sline = new string(line);
		int splitpos = sline->find("\t");
		skey = sline->substr(0, splitpos);
		svalue = sline->substr(splitpos + 1);

		key = skey;
		value = atol(svalue.c_str());
		KVMap.insert(make_pair(key,value));
		if((++times) % 100 == 0){
		  	reduce();
		}
	}

	reduce();
	writer->writeSharedMemory(NULL, -1);
	delete(writer);

	return 0;
}

void reduce(){
  	if(!KVMap.empty()){
		it = KVMap.begin();
		while(it != KVMap.end()){
			//cout << it->first << "\t" << " " << endl;

			ss.str("");
			//cout << key << "\t" << " " << endl;
			ss << it->first << "\t" << " ";
			KV = ss.str();
			kvpair = const_cast<char *>(KV.c_str());
			writer->writeSharedMemory(kvpair, KV.size());
			++it;
		}
		KVMap.clear();
		times = 0;
	}
}
