#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
using namespace std;

int main (int argc, char *argv[]) {
	if (argc < 3) {
		cerr << "Please execute like " << argv[0] << " BasesFile stkFile" << endl;
		exit(1);
	}
	
	ifstream inFile;
	inFile.open(argv[1]);
	if (!inFile) {
		cerr << "Unable to open input file " << argv[1] << endl;
		exit(1);
	}
		
	string line;
	int numlines=0;
	while (getline(inFile,line)) {
		numlines++;
	}
	string sequencegroups[numlines/2];
	string basestrings[numlines/2];
	inFile.close();
	inFile.open(argv[1]);
	for (int i=0; i<numlines/2; i++) {
		getline(inFile,sequencegroups[i]);
		getline(inFile,basestrings[i]);
	}
	inFile.close();
	
	inFile.open(argv[2]);
	while (getline(inFile,line)) {
		if (line[0] == '#' || line=="") {
			cout << line << endl;
		}
		else {
			int labelchars=0;
			while (line[labelchars] != ' ') {
				labelchars++;
			}
			string label = line.substr(0,labelchars);
			int sequencegroupnum=-1;
			for (int i=0; i<numlines/2; i++) {
				if (sequencegroups[i].find(label) != string::npos) {
					sequencegroupnum=i;
				}
			}
			if (sequencegroupnum != -1) {
				char lastletter='0';
				int seqstart=line.size()-1;
				while (line[seqstart-1] != ' ') {
					seqstart--;
				}
				cout << line.substr(0,seqstart);
				for (int i=seqstart; i<line.size(); i++) {
					
					if (line[i]==line[i-1]) {
						cout << lastletter;
					}
					else {
						char base1 = basestrings[sequencegroupnum][5*(i-seqstart)];
						char base2 = basestrings[sequencegroupnum][5*(i-seqstart)+1];
						char base3 = basestrings[sequencegroupnum][5*(i-seqstart)+2];
						char base4 = basestrings[sequencegroupnum][5*(i-seqstart)+3];
						
						//cout << " " << base1 << base2 << base3 << base4 << " ";
						
						switch (line[i]) {
							case 'j':
								if (base2 == 't') {
									cout << 't';
									lastletter='t';
								}
								else {
									cout << 'j';
									lastletter='j';
								}
								break;
							case 'k':
								if (base1 == 't') {
									switch (base2) {
										case 'a':
											cout << 'l';
											lastletter='l';
											break;
										case 'g':
											cout << 'm';
											lastletter='m';
											break;
										default:
											cout << 'k';
											lastletter='k';
											break;
									}
								}
								else {
									cout << 'k';
									lastletter='k';
								}
								break;
							case 'g':
								switch (base3) {
									case 'a':
										cout << 'a';
										lastletter='a';
										break;
									case 'g':
										cout << 'b';
										lastletter='b';
										break;
									default:
										cout << 'g';
										lastletter='g';
										break;
								}
								break;
							case 'f':
								if ((base3=='a' && base4=='a') || (base3=='a' && base4=='g') || (base3=='g' && base4=='a')) {
									cout << 'h';
									lastletter='h';
								}
								else {
									cout << 'f';
									lastletter='f';
								}
								break;
							default:
								cout << line[i];
								lastletter=line[i];
								break;
						}
					}
				}
				cout << endl;
			}
			else {
				cout << line << endl;
			}

		}

	}
	
	return 0;
}