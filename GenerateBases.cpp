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
	srand(662737016); //seed random number generator. Seeding with a constant allows for consistency in debugging.
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
	
	//Read generator output into memory
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
	
	//Count taxa nodes under the assumption that they are exactly those nodes whose labels are a single character. This is consistant with TreeBuilder.
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
	int nextnode[numtaxa]; //Find the evidence nodes for each taxa under the assumption that the evidence nodes immediately follow the taxa node and that they second character of an evidence node is a number. This is consistent with generator and TreeBuilder.
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
	
	int sequencestart=1; //Find where the label stops and the sequence begins.
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
			cout << stkfile[k].substr(0,labelchars) << " "; //In this first line write the nodes the sequence applies to.
		}
		cout << endl;
		char lastchar[taxarow[i+1]-taxarow[i]];
		for (int j=0; j<taxarow[j+1]-taxarow[j]; j++) {
			lastchar[j]='x';
		}
		for (int j=sequencestart; j<stkfile[taxarow[i]].size(); j++) {
			conflict=1;
			while (conflict) { //Generate random bases until we hit on legal bases
				base1 = rand() % 4;
				base2 = rand() % 4;
				base3 = rand() % 4;
				base4 = rand() % 4;
				conflict=0;
				for (int k=taxarow[i]; k<nextnode[i]; k++) { //check if the bases are legal
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
			
			/*
			if (argc==4 && j==atoi(argv[3]) && i==atoi(argv[2])) {
				cout << "HERE" << endl;
				for (int k=taxarow[i]; k<nextnode[i]; k++) {
					cout << stkfile[k][j] << " " << lastchar[k-taxarow[i]] << endl;
				}
				cout << endl << base1 << base2 << base3 << base4 << endl;
			}
			 */
			
			//Update previous letter. We only bother with intron labels that carry t, ta or tg since these are the only letter that can cause the generated bases to be invalid.
			for (int k=taxarow[i]; k<nextnode[i]; k++) {
				if (stkfile[k][j] != stkfile[k][j-1]) {
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
			}
			
			//if (argc != 4) {
			switch (base1) { //In the second line we out put legal bases surrounding each transition.
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
			//}
		}
		cout << endl;
	}
}