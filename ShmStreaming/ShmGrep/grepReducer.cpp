#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>

using namespace std;

string K;
long V;

void reduce(string key, long value);

int main(int argc, char * argv[]){
	string line;

	getline(cin, line);
	int splitpos = line.find('\t');
	string ks = line.substr(0, splitpos);
	string vs = line.substr(splitpos + 1);

	K = ks;
	V = atol(vs.c_str());

	while( getline(cin, line) ){
		splitpos = line.find('\t');
		ks = line.substr(0, splitpos);
		vs = line.substr(splitpos + 1);

		string key = ks;
		long value = atol(vs.c_str());

		if( K.compare(key) == 0 )
			V += value;
		else{
			cout << K << "\t" << V << endl;
			K = key;
			V = value;
		}
	}
	if( K.compare("") != 0 )
		cout << K << "\t" << V << endl;
	return 0;
}
