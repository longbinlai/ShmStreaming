#include "LineSpliter.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

enum parameters{key, shmid, owner, perms, bytes, nattch, status};

int main(int argc, char** argv){
	if(argc != 2){
		perror("Should have 2 parameters!");
		return EXIT_FAILURE;
	}
	vector<string> vec;
	ifstream fin;
	string line;
	string word;
	string filename("./output");
	string command("ipcs > ./output");
	system(const_cast<char *>(command.c_str()));

	fin.open(const_cast<char *>(filename.c_str()));

	while(getline(fin, line)){
		if(line.length() == 0){
			continue;
		}
		LineSpliter* ls = new LineSpliter(line);
		word = ls->next();
		while(word.length() != 0){
			vec.push_back(word);
			word = ls->next();
		}
		if (vec.size() >= 6) {
			if (vec[bytes].compare(argv[1]) == 0 &&
					vec[nattch].compare("0") == 0) {
				command = "ipcrm -m ";
				command += vec[shmid];
				cout << command << endl;
				system(const_cast<char *> (command.c_str()));
			}
		}
		vec.clear();
		delete(ls);
	}

	return 0;
}
