#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

struct HldaTree {
	HldaTree* parent; 
	HldaTree* child; 
	vector<string> topicModel; 
	bool leafNode = false;
	bool parentNode = false;
};

int main() {
	
	vector<HldaTree*> roots; 

	string response; 
	cout << "Which query do you want?";
	cin >> response;

	for (int fileNum = 0; fileNum < 5; fileNum++) {

		string filename = response + to_string(fileNum) + ".txt";
		cout << "reading " << filename << "...";

		HldaTree* root = new HldaTree(); 
		root->parentNode = true;
		roots.push_back(root);

		// read in HLDA model into tree node
		string line;
		ifstream myfile (filename);
		bool parent = true;
		string buffer;
		while(getline(myfile, line)) {
			for (int i = 0; i < line.length(); i++) {
				if (line[i] == ' ') {
					cout << buffer << '\n';
					root->topicModel.push_back(buffer);
					buffer.clear();
				}
				else {
					buffer += line[i];
				}
			}

			HldaTree* newNode = new HldaTree(); 
			newNode->parent = root;
			root = newNode;
		}
		cout << buffer << '\n';
		root->topicModel.push_back(buffer);
		buffer.clear();

		root->leafNode = true;
	}

	return 0; 
}