#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
using namespace std;

int main (int argc, char *argv[]) {
	
	if (argc < 3) {
		cerr << "Please execute like ./DifferenceFinder generatoroutput xrateoutput options." << endl;
		cerr << "Options are" << endl;
		cerr << "-i to write fraction identity of evidence to root sequence to errors file," << endl;
		cerr << "-l to write seqeunce length to errors file," << endl;
		cerr << "-r to write TP, TN, FP, FN to errors file," << endl;
		cerr << "-mcc to write mcc to errors file," << endl;
		cerr << "-d depths to seperate TP, TN, FP, FN and mcc data written to errors file by node depth," << endl;
		cerr << "-min min sequence length (integer) to only return data if the sequence length is above some minimum," << endl;
		cerr << "-maxd max depth to keep number of columns constant across runs with different trees," << endl;
		cerr << "-tn node label to output info about this node." << endl;
		exit(1);
	}
	
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
	
	int identity=0;
	int notidentity=0;
	int thisdepth;
	bool checkidentity=0;
	for (int i=0; i<argc; i++) {
		if (argv[i][0]=='-' && argv[i][1]=='i') {
			checkidentity=1;
		}
	}
	
	bool writemcc=0;
	for (int i=0; i<argc; i++) {
		if (argv[i][0]=='-' && argv[i][1]=='m' && argv[i][2]=='c' && argv[i][3]=='c') {
			writemcc=1;
		}
	}
	
	bool writelength=0;
	for (int i=0; i<argc; i++) {
		if (argv[i][0]=='-' && argv[i][1]=='l') {
			writelength=1;
		}
	}
	
	bool writeraw=0;
	for (int i=0; i<argc; i++) {
		if (argv[i][0]=='-' && argv[i][1]=='r') {
			writeraw=1;
		}
	}
	
	int minlength=0;
	for (int i=0; i<argc-1; i++) {
		if (argv[i][0]=='-' && argv[i][1]=='m' && argv[i][2]=='i' && argv[i][3]=='n') {
			minlength = atoi(argv[i+1]);
		}
	}
	
	int maxdepth=0;
	bool maxd=0;
	for (int i=0; i<argc-1; i++) {
		if (argv[i][0]=='-' && argv[i][1]=='m' && argv[i][2]=='a' && argv[i][3]=='x' && argv[i][4]=='d') {
			maxd=1;
			maxdepth = atoi(argv[i+1]);
		}
	}
	
	bool writenode=0;
	string thisnode;
	for (int i=0; i<argc-1; i++) {
		if (argv[i][0]=='-' && argv[i][1]=='t' && argv[i][2]=='n') {
			writenode=1;
			thisnode=argv[i+1];
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
		else {
			if (checkidentity) {
				for (int j=0; j<sequencelength; j++) {
					if (GeneratorSequence[permutation[i]][j]==GeneratorSequence[0][j]) {
						identity++;
					}
					else {
						notidentity++;
					}

				}
			}
		}
	}
	
	if (maxd==0) {
		for (int i=0; i<numnodes; i++) {
			if (depths[i]>maxdepth) {
				maxdepth=depths[i];
			}
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
		cout << "TP = " << TPdepth[0] << ", TN = " << TNdepth[0] << ", FP = " << FPdepth[0] << ", FN = " << FNdepth[0] << endl;
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
	if (sequencelength >= minlength) {
	outErrors.open("errors");
	for (int i=0; i<errorlines; i++) {
		outErrors << errorfile[i] << endl;
	}
	if (checkidentity) {
		outErrors << 1.0*identity/(identity+notidentity) << " ";
	}
	if (writelength) {
		outErrors << sequencelength << " ";
	}
	if (checkdepth) {
		if (writeraw) {
			for (int i=1; i<=maxdepth; i++) {
				outErrors << i << " " << TPdepth[i] << " " << TNdepth[i] << " " << FPdepth[i] << " " << FNdepth[i] <<  " ";
			}
		}
		if (writemcc) {
			for (int i=1; i<=maxdepth; i++) {
				outErrors << i << " " << (TPdepth[i]*TNdepth[i]-FPdepth[i]*FNdepth[i])/sqrt((TPdepth[i]+FPdepth[i])*(TPdepth[i]+FNdepth[i])*(TNdepth[i]+FPdepth[i])*(TNdepth[i]+FNdepth[i])) << " ";
			}
		}
	}
	else {
		if (writeraw) {
			outErrors << TPdepth[0] << " " << TNdepth[0] << " " << FPdepth[0] << " " << FNdepth[0] << " ";
		}
		if (writemcc) {
			outErrors << ((float)TPdepth[0]*(float)TNdepth[0]-(float)FPdepth[0]*(float)FNdepth[0])/sqrt(((float)TPdepth[0]+(float)FPdepth[0])*((float)TPdepth[0]+(float)FNdepth[0])*((float)TNdepth[0]+(float)FPdepth[0])*((float)TNdepth[0]+(float)FNdepth[0])) << " ";
		}
	}
	if (writenode) {
		for (int i=0; i<numnodes; i++) {
			if (XrateNodeLabel[i]==thisnode) {
				if (writeraw) {
					outErrors << TP[i] << " " << TN[i] << " " << FP[i] << " " << FN[i] << " ";
				}
				if (writemcc) {
					outErrors << (TP[i]*TN[i]-FP[i]*FN[i])/sqrt((TP[i]+FP[i])*(TP[i]+FN[i])*(TN[i]+FP[i])*(TN[i]+FN[i])) << " ";
				}
			}
		}
	}
		
	outErrors << endl;
	outErrors.close();
	}
	
	return 0;
}