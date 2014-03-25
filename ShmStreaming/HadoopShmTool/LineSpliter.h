#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <vector>
using namespace std;

class LineSpliter{
	public:
		LineSpliter(string l);
		LineSpliter(char * l);
		string next();
		const static char wordSplitor = ' ';
	private:
		string line;
		vector<string> words;
		vector<string>::iterator wordsIt;
		vector<string>::iterator wordsEnd;
		void split();
};

LineSpliter::LineSpliter(string l){
	words.clear();
	line = l;
	split();
}

LineSpliter::LineSpliter(char * l){
	words.clear();
	line.assign(l);
	split();
}

void LineSpliter::split(){
	int spacePos;			
	string word;
	while(line.length() > 0){
		spacePos = line.find(wordSplitor);
		if( spacePos > 0 ){
			word = line.substr(0, spacePos);
			words.push_back(word);
			line = line.substr(spacePos + 1);
		}else if( spacePos == -1 ){
			word = line;
			words.push_back(word);
			line.assign("");
		}else if ( spacePos == 0 ){
			line = line.substr(spacePos + 1);
		}
	}

	wordsIt = words.begin();
	wordsEnd = words.end();
}

string LineSpliter::next(){
	string re = "";
	if( wordsIt != wordsEnd){
		re = *wordsIt;
		wordsIt ++;
	}
	return re;
}
