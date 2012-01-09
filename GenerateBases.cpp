#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
using namespace std;

bool isdigit(char character) {
	string digits="1234567890";
	if (digits.find(character) != string::npos) {
		return 1;
	}
	else {
		return 0;
	}
}


int main (int argc, char *argv[]) {
	srand((unsigned)time(0)); //seed random number generator.
	if (argc < 2) {
		cerr << "Please execute like " << argv[0] << " stkfile." << endl;
		exit(1);
	}
	
	/*
	int treelocation=0;
	string argument;
	for (int i=0; i<argc-1; i++) {
		argument=argv[i];
		if (argument=="-t") {
			treelocation=i+1;
		}
	}
	 */
	
	ifstream inFile;
	inFile.open(argv[1]);
	if (!inFile) {
		cerr << "Unable to open stk file: " << argv[1] << endl;
		exit(1);
	}
	int numlines=0;
	string line;
	while (getline(inFile,line)) {
		numlines++;
	}
	inFile.close();
	string stkfile[numlines];
	inFile.open(argv[1]);
	for (int i=0; i<numlines; i++) {
		getline(inFile,stkfile[i]);
	}
	inFile.close();
	
	int numtaxa=0;
	for (int i=0; i<numlines; i++) {
		if (stkfile[i][0] != '#' && stkfile[i][1] == ' ') {
			numtaxa++;
		}
	}
	int taxarow[numtaxa];
	int thistaxa=0;
	for (int i=0; i<numlines; i++) {
		if (stkfile[i][0] != '#' && stkfile[i][1] == ' ') {
			taxarow[thistaxa]=i;
			thistaxa++;
			//cout << i << endl;
		}
	}
	int nextnode[numtaxa];
	for (int i=0; i<numtaxa; i++) {
		int thisline=taxarow[i]+1;
		while (isdigit(stkfile[thisline][1]) && thisline < numlines) {
			thisline++;
		}
		nextnode[i]=thisline;
		//cout << thisline << endl;
	}
	
	 /*
	for (int i=0; i< numtaxa; i++) {
		cout << taxarow[i] << endl;
	}
	  */
	
	int sequencestart=1;
	while (stkfile[taxarow[0]][sequencestart] == ' ') {
		sequencestart++;
	}
	
	int base1,base2,base3,base4;
	bool conflict;
	for (int i=0; i<numtaxa; i++) {
		for (int k=taxarow[i]; k<nextnode[i]; k++) {
			int labelchars=0;
			while (stkfile[k][labelchars] != ' ') {
				labelchars++;
			}
			cout << stkfile[k].substr(0,labelchars) << " ";
		}
		cout << endl;
		char lastchar[taxarow[i+1]-taxarow[i]];
		for (int j=0; j<taxarow[j+1]-taxarow[j]; j++) {
			lastchar[j]='x';
		}
		for (int j=sequencestart; j<stkfile[taxarow[i]].size(); j++) {
			conflict=1;
			while (conflict) {
				base1 = rand() % 4;
				base2 = rand() % 4;
				base3 = rand() % 4;
				base4 = rand() % 4;
				conflict=0;
				for (int k=taxarow[i]; k<nextnode[i]; k++) {
					if (stkfile[k][j] == 'f' && lastchar[k-taxarow[i]]=='t') {
						if ((base3==0 && base4==0) || (base3==0 && base4==2) || (base3==2 && base4==0)) {
							conflict=1;
						}
					}
					if (stkfile[k][j] == 'g' && lastchar[k-taxarow[i]]=='l') {
						if (base3==0 || base3==2) {
							conflict=1;
						}
					}
					if (stkfile[k][j] == 'g' && lastchar[k-taxarow[i]]=='m') {
						if (base3==0) {
							conflict=1;
						}
					}
				}
			}
			
			for (int k=taxarow[i]; k<nextnode[i]; k++) {
				lastchar[k-taxarow[i]]='x';
				if (stkfile[k][j]=='j' && base2==3) {
					lastchar[k-taxarow[i]]='t';
				}
				if (stkfile[k][j]=='k' && base1==3 && base2==0) {
					lastchar[k-taxarow[i]]='l';
				}
				if (stkfile[k][j]=='k' && base1==3 && base2==2) {
					lastchar[k-taxarow[i]]='m';
				}
			}
			
			switch (base1) {
				case 0:
					cout << "a";
					break;
				case 1:
					cout << "c";
					break;
				case 2:
					cout << "g";
					break;
				case 3:
					cout << "t";
					break;
				default:
					break;
			}
			switch (base2) {
				case 0:
					cout << "a";
					break;
				case 1:
					cout << "c";
					break;
				case 2:
					cout << "g";
					break;
				case 3:
					cout << "t";
					break;
				default:
					break;
			}
			switch (base3) {
				case 0:
					cout << "a";
					break;
				case 1:
					cout << "c";
					break;
				case 2:
					cout << "g";
					break;
				case 3:
					cout << "t";
					break;
				default:
					break;
			}
			switch (base4) {
				case 0:
					cout << "a";
					break;
				case 1:
					cout << "c";
					break;
				case 2:
					cout << "g";
					break;
				case 3:
					cout << "t";
					break;
				default:
					break;
			}
			cout << " ";
		}
		cout << endl;
	}
}