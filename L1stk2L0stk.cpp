#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
using namespace std;

int main(int argc, char *argv[]) {
	ifstream inFile;
	inFile.open(argv[1]);
	if (!inFile) {
		cerr << "Unable to open file " << argv[1] << endl;
		exit(1);
	}
	string line;
	bool space;
	while (getline(inFile,line)) {
		if (line[0]!='#') {
			space=0;
			for (int i=0; i<line.size(); i++) {
				if (line[i]==' ') {
					space=1;
				}
				if (space == 1) {
					switch (line[i]) {
						case 'f':
							cout << 'e';
							break;
						case 'g':
							cout << 'e';
							break;
						case 'j':
							cout << 'i';
							break;
						case 'k':
							cout << 'i';
							break;
						default:
							cout << line[i];
							break;
					}
				}
				else {
					cout << line[i];
				}

			}
			cout << endl;
		}
		else {
			cout << line << endl;
		}

	}
	return 0;
}