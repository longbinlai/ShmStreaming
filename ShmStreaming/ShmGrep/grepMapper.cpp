#include "../HadoopShmTool/SharedMemoryOperator.h"
#include <cstdlib>
#include <boost/regex.hpp>

using namespace std;

stringstream ss;
string KV;
char* kvpair;
SharedMemoryOperator* reader;
SharedMemoryOperator* writer;

void grep_map(long, string, string);

int main(int argc, char * argv[]){
	if(argc != 3){
		perror("grepMapper <look-up-string> <buffersize>");
		return EXIT_FAILURE;
	}
	int buffersize = atoi(argv[2]);
	string value;
	long key;
	string svalue;
	string skey;
	string sline;

	string regstr(argv[1]);

	try{
	  	reader = new SharedMemoryOperator(buffersize);
		writer = new SharedMemoryOperator(buffersize);
	} catch(exception & ex){
	  	delete(reader);
		delete(writer);
		return EXIT_FAILURE;
	}

	int rcnt = 0;
	char* data;
	data = (char *)malloc(buffersize);
	memset(data, '0', buffersize);

	while(true){
	  	rcnt = reader->readSharedMemory(data);
		if(rcnt == -1){
		  	writer->writeSharedMemory(NULL, -1);
			break;
		} 
		else if(rcnt == 0)
		  	continue;

		sline.assign(data, rcnt);
		int splitpos = sline.find("\t");
		skey = sline.substr(0, splitpos);
		svalue = sline.substr(splitpos+1);
		
		key = atol(skey.c_str());
		value = svalue;

		grep_map(key, value, regstr);
	}
	delete(reader);
	delete(writer);
	free(data);

	return 0;
}

void grep_map(long key, string value, string regex){
	boost::regex e(regex);
	boost::smatch what;
	boost::sregex_token_iterator p(value.begin(), value.end(), e);
	boost::sregex_token_iterator j;
	while( p != j ){
		string result(p->first, p->second);
		//cout << result << "\t" << 1 << std::endl;
		ss << result << "\t" << 1;
		KV = ss.str();
		kvpair = const_cast<char *>(KV.c_str());
		writer->writeSharedMemory(kvpair, KV.size());
		ss.str("");
		p++;
	}
}
