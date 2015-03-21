#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <ostream>
using namespace std; 

// check if is a word, not link, citation, etc. 
bool isValidWord (string word) {

	// we don't want URLs
	string dotCom = ".com";
	if (word.find(dotCom) < word.length()) {
		return false;
	}

	for (int i = 0; i < word.length(); i++) {
		int ascii_val = static_cast<int>(word[i]);
		if (ascii_val <= 43 || ascii_val >= 123 || 
			ascii_val == 61 || ascii_val == 64 || 
			(ascii_val >= 91 && ascii_val <= 96) || 
			(ascii_val >= 48 && ascii_val <= 57) ) {
			return false; 
		}
	}
	return true; 
}

int main() {

	string query = "Dog"; 
	int numFiles = 5; 
	string newFileName = query + ".out";
	ofstream newFile (newFileName); 			

	for (int i = 0; i < numFiles; i++) {
		map<string, int> wordCounter; 
		
		ifstream file; 
		string fileName = query;
		fileName += to_string(i);
		fileName += ".txt";
		file.open(fileName);

		string word; 

		// read file word by word
		while (file >> word) {

			// if not a valid word, read next word in file
			if (!isValidWord(word)) {
				continue; 
			}

			// if key doesn't exist in map
			if (wordCounter.find(word) == wordCounter.end()) {
				wordCounter[word] = 1; 
			}
			else {
				wordCounter[word] += 1;
			}
		}
		
		// format for Blei's HLDA
		int wordCount = 0; 
		string output = ""; 
		for (auto iter = wordCounter.begin(); iter != wordCounter.end(); iter++) {
			output += to_string(wordCount);
			output += ":";
			output += to_string(iter->second);
			output += ' ';
			wordCount++; 
		}

		// write new file 

		string wc = to_string(wordCount);
		wc += " ";
		newFile << wc << output << '\n';
	}
	
	return 0;
}