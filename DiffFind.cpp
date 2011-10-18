#include <iostream>
#include <string>
#include <fstream>
using namespace std;

int main (int argc, char *argv[]) {
	ifstream xratefile;
	xratefile.open(argv[1]);
	if (!xratefile) {
		cerr << "Unable to open input file " << argv[1] << endl;
		exit(1);
	}
	int numlines=0;
	string line;
	while (getline(xratefile,line)) {
		numlines++;
	}
	xratefile.close();
	int numnodes=(numlines-4)/3;
	string file[numlines];
	xratefile.open(argv[1]);
	for (int i=0; i<numlines; i++) {
		getline(xratefile,file[i]);
		//cout << i << " is " << file[i] << endl;
	}
	
	string XrateNodeLabel[numnodes];
	string XrateNodeSequence[numnodes];
	int position;
	for (int i=0; i<numnodes; i++) {
		position=0;
		while (file[4+3*i][position] != ' ') {
			position++;
		}
		XrateNodeLabel[i]=file[4+3*i].substr(0,position);
		position=file[5+3*i].size()-1;
		while (file[4+3*i][position] != ' ') {
			position--;
		}
		XrateNodeSequence[i]=file[5+3*i].substr(position+1,file[5+3*i].size()-position-1);
	}
	xratefile.close();
	
	ifstream generatorfile;
	generatorfile.open(argv[2]);
	if (!generatorfile) {
		cerr << "Unable to open file " << argv[2] << endl;
		exit(1);
	}
	string GeneratorNodeLabel[numnodes];
	string GeneratorSequence[numnodes];
	for (int i=0; i<numnodes; i++) {
		generatorfile >> GeneratorNodeLabel[i];
		generatorfile >> GeneratorSequence[i];
	}
	generatorfile.close();
	
	int permutation[numnodes];
	for (int i=0; i< numnodes; i++) {
		for (int j=0; j<numnodes; j++) {
			if (XrateNodeLabel[i]==GeneratorNodeLabel[j]) {
				permutation[i]=j;
			}
		}
	}
	
	int differences=0;
	int sequencelength = XrateNodeSequence[0].size();
	for (int i=0; i<numnodes; i++) {
		for (int j=0; j<sequencelength; j++) {
			if (XrateNodeSequence[i][j] != GeneratorSequence[permutation[i]][j]) {
				cout << "Difference in " << XrateNodeLabel[i] << " at position " << j << endl;
				differences++;
			}
		}
	}
	cout << "In total, " << differences << " differences were found on " << numnodes << " sequences of length " << sequencelength << "." << endl;
	
	return 0;
}