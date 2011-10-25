#include <iostream>
#include <string>
#include <fstream>
using namespace std;

int main (int argc, char *argv[]) {
	ifstream xratefile;
	xratefile.open(argv[1]);
	if (!xratefile) {
		cerr << "Unable to open xrate output file " << argv[1] << endl;
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
	
	int depths[numnodes];
	for (int i=0; i<numnodes; i++) {
		depths[i]=0;
	}
	bool checkdepth=0;
	string Newick;
	ifstream depthfile;
	for (int i=0; i<argc-1; i++) {
		if (argv[i][0]=='-' && argv[i][1]=='d') {
			checkdepth=1;
			depthfile.open(argv[i+1]);
			if (!depthfile) {
				cerr << "Unable to open depths file " << argv[i+1] << endl;
				exit(1);
			}
			for (int i=0; i<numnodes; i++) {
				depthfile >> depths[i];
			}
		}
	}
	
	string XrateNodeLabel[numnodes];
	string XrateNodeSequence[numnodes];
	string xi="xi";
	string ix="ix";
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
		if (XrateNodeSequence[i].find(xi)!=string::npos) {
			cerr << "ATTENTION: xi found in " << XrateNodeLabel[i] << "." << endl;
		}
		if (XrateNodeSequence[i].find(ix)!=string::npos) {
			cerr << "ATTENTION: ix found in " << XrateNodeLabel[i] << "." << endl;
		}
	}
	xratefile.close();
	
	ifstream generatorfile;
	generatorfile.open(argv[2]);
	if (!generatorfile) {
		cerr << "Unable to open generator output file " << argv[2] << endl;
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
	
	int TP[numnodes];
	int TN[numnodes];
	int FP[numnodes];
	int	FN[numnodes];
	for (int i=0; i<numnodes; i++) {
		TP[i]=0;
		TN[i]=0;
		FP[i]=0;
		FN[i]=0;
	}
	int numdata=0;
	string numbers="1234567890";
	int sequencelength = XrateNodeSequence[0].size();
	for (int i=0; i<numnodes; i++) {
		if (numbers.find(XrateNodeLabel[i][1])==string::npos) {
			for (int j=0; j<sequencelength; j++) {
				switch (GeneratorSequence[permutation[i]][j]) {
					case 'e':
						switch (XrateNodeSequence[i][j]) {
							case 'e':
								TP[i]++;
								break;
							case 'i':
								FN[i]++;
								break;
							case 'x':
								FN[i]++;
								break;
							default:
								break;
						}
						break;
					case 'i':
						switch (XrateNodeSequence[i][j]) {
							case 'e':
								FP[i]++;
								break;
							case 'i':
								TP[i]++;
								break;
							case 'x':
								FN[i]++;
								break;
						}
						break;
					case 'x':
						switch (XrateNodeSequence[i][j]) {
							case 'e':
								FP[i]++;
								break;
							case 'i':
								FP[i]++;
								break;
							case 'x':
								TN[i]++;
								break;
							default:
								break;
						}
						break;
				}
				if (XrateNodeSequence[i][j]!=GeneratorSequence[permutation[i]][j]) {
					//cout << "At " << GeneratorNodeLabel[permutation[i]] << " " << GeneratorSequence[permutation[i]][j] << "->" << XrateNodeSequence[i][j] << endl;
				}
			}
		}
	}
	
	int maxdepth=0;
	for (int i=0; i<numnodes; i++) {
		if (depths[i]>maxdepth) {
			maxdepth=depths[i];
		}
	}
	int TPdepth[maxdepth+1];
	int TNdepth[maxdepth+1];
	int FPdepth[maxdepth+1];
	int FNdepth[maxdepth+1];
	for (int i=0; i<=maxdepth; i++) {
		TPdepth[i]=0;
		TNdepth[i]=0;
		FPdepth[i]=0;
		FNdepth[i]=0;
	}
	for (int i=0; i<numnodes; i++) {
		TPdepth[depths[permutation[i]]]+=TP[i];
		TNdepth[depths[permutation[i]]]+=TN[i];
		FPdepth[depths[permutation[i]]]+=FP[i];
		FNdepth[depths[permutation[i]]]+=FN[i];
	}
	
	for (int i=0; i<numnodes; i++) {
		//cout << "at " << permutation[i] << " TN = " << TN[i] << ", TP = " << TP[i] << ", FN = " << FN[i] << ", FP = " << FP[i] << endl;
	}
	if (checkdepth) {
		for (int i=1; i<=maxdepth; i++) {
			cout << "at depth " << i << " TP = " << TPdepth[i] << ", TN = " << TNdepth[i] << ", FP = " << FPdepth[i] << ", FN = " << FNdepth[i] << endl;
		}
	}
	else {
		cout << " TP = " << TPdepth[0] << ", TN = " << TNdepth[0] << ", FP = " << FPdepth[0] << ", FN = " << FNdepth[0] << endl;
	}

	
	ifstream inErrors;
	inErrors.open("errors");
	int errorlines=0;
	if (inErrors) {
		while (getline(inErrors,line)) {
			errorlines++;
		}
	}
	string errorfile[errorlines];
	inErrors.close();
	inErrors.open("errors");
	if (inErrors) {
		for (int i=0; i<errorlines; i++) {
			getline(inErrors,errorfile[i]);
		}
	}
	inErrors.close();
	
	ofstream outErrors;
	outErrors.open("errors");
	for (int i=0; i<errorlines; i++) {
		outErrors << errorfile[i] << endl;
	}
	if (checkdepth) {
		for (int i=1; i<=maxdepth; i++) {
			outErrors << TPdepth[i] << " " << TNdepth[i] << " " << FPdepth[i] << " " << FNdepth[i] << " ";
		}
		outErrors << endl;
	}
	else {
		outErrors << TPdepth[0] << " " << TNdepth[0] << " " << FPdepth[0] << " " << FNdepth[0] << endl;
	}

	outErrors.close();
	
	return 0;
}