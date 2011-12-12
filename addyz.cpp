#define MAXCHILDREN 10
#define MAXDEPTH 10
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
using namespace std;

int main (int argc, char *argv[]) {
	if (argc < 2) {
		cerr << "Exectute like " << argv[0] << "stkfile labels of lines to change to yz." << endl;
		exit(1);
	}
	string LabelsToChange[argc-2];
	bool LabelsChanged[argc-2];
	for (int i=0; i<argc-2; i++) {
		LabelsToChange[i]=argv[i+2];
		LabelsChanged[i]=0;
	}
	ifstream inFile;
	inFile.open(argv[1]);
	if (!inFile) {
		cerr << "Unable to open stk file " << argv[1] << endl;
		exit(1);
	}
	string line;
	bool space;
	bool gotlabel;
	string label;
	bool changethis;
	bool changedecided;
	while (getline(inFile,line)) {
		if (line[0]!='#') {
			space=0;
			gotlabel=0;
			changethis=0;
			changedecided=0;
			for (int i=0; i<line.size(); i++) {
				if (line[i]==' ') {
					space=1;
				}
				if (space && !gotlabel) {
					label=line.substr(0,i);
					gotlabel=1;
				}
				if (gotlabel && !changedecided) {
					for (int i=0; i < argc-2; i++) {
						if (label==LabelsToChange[i]) {
							changethis=1;
							LabelsChanged[i]=1;
						}
					}
					changedecided=1;
				}
				if (space && changethis) {
					switch (line[i]) {
						case 'e':
							cout << 'y';
							break;
						case 'f':
							cout << 'y';
							break;
						case 'g':
							cout << 'y';
							break;
						case 'i':
							cout << 'z';
							break;
						case 'j':
							cout << 'z';
							break;
						case 'k':
							cout << 'z';
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
	for (int i=0; i<argc-2; i++) {
		if (!LabelsChanged[i]) {
			cerr << "Could not find and change label " << LabelsToChange[i] << endl;
		}
	}
	return 0;
	
}