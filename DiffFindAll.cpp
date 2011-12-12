#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
using namespace std;

int main (int argc, char *argv[]) {
	
	if (argc < 5) {
		cerr << "Please execute like ./DifferenceFinder reconstructionoutput generatoroutput level xrate/viterbi options." << endl;
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
	
	ifstream reconstruction;
	reconstruction.open(argv[1]);
	if (!reconstruction) {
		cerr << "Unable to open xrate output file " << argv[1] << endl;
		exit(1);
	}
	int numlines=0;
	string line;
	while (getline(reconstruction,line)) {
		numlines++;
	}
	reconstruction.close();
	int numnodes=(numlines-4)/3;
	string file[numlines];
	reconstruction.open(argv[1]);
	for (int i=0; i<numlines; i++) {
		getline(reconstruction,file[i]);
		//cout << i << " is " << file[i] << endl;
	}
	reconstruction.close();
	
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
	
	bool viterbi=0;
	if (argv[4][0]=='V' || argv[4][0]=='v') {
		viterbi=1;
	}
	
	int level=atoi(argv[3]);
	
	string ReconNodeLabel[numnodes];
	string ReconNodeSequence[numnodes];
	string xi="xi";
	string ix="ix";
	int position;
	
	if (viterbi) {
		for (int i=0; i<numnodes; i++) {
			position=0;
			while (file[i][position] != ' ') {
				position++;
			}
			ReconNodeLabel[i]=file[i].substr(0,position);
			position=file[i].size()-1;
			while (file[i][position] != ' ') {
				position--;
			}
			ReconNodeSequence[i]=file[i].substr(position+1,file[i].size()-position-1);
			if (ReconNodeSequence[i].find(xi)!=string::npos) {
				cerr << "ATTENTION: xi found in " << ReconNodeLabel[i] << "." << endl;
			}
			if (ReconNodeSequence[i].find(ix)!=string::npos) {
				cerr << "ATTENTION: ix found in " << ReconNodeLabel[i] << "." << endl;
			}
		}
	}
	else {
		for (int i=0; i<numnodes; i++) {
			position=0;
			while (file[4+3*i][position] != ' ') {
				position++;
			}
			ReconNodeLabel[i]=file[4+3*i].substr(0,position);
			position=file[5+3*i].size()-1;
			while (file[4+3*i][position] != ' ') {
				position--;
			}
			ReconNodeSequence[i]=file[5+3*i].substr(position+1,file[5+3*i].size()-position-1);
			if (ReconNodeSequence[i].find(xi)!=string::npos) {
				cerr << "ATTENTION: xi found in " << ReconNodeLabel[i] << "." << endl;
			}
			if (ReconNodeSequence[i].find(ix)!=string::npos) {
				cerr << "ATTENTION: ix found in " << ReconNodeLabel[i] << "." << endl;
			}
		}
	}
	
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
			if (ReconNodeLabel[i]==GeneratorNodeLabel[j]) {
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
	int sequencelength = ReconNodeSequence[0].size();
	
	if (level == 0) {
		for (int i=0; i<numnodes; i++) {
			if (numbers.find(ReconNodeLabel[i][1])==string::npos) {
				for (int j=0; j<sequencelength; j++) {
					switch (GeneratorSequence[permutation[i]][j]) {
						case 'e':
							switch (ReconNodeSequence[i][j]) {
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
							switch (ReconNodeSequence[i][j]) {
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
							switch (ReconNodeSequence[i][j]) {
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
					if (ReconNodeSequence[i][j]!=GeneratorSequence[permutation[i]][j]) {
						cout << "At " << GeneratorNodeLabel[permutation[i]] << " position " << j << " " << GeneratorSequence[permutation[i]][j] << "->" << ReconNodeSequence[i][j] << endl;
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
	}
	else {
		int sequencelength = ReconNodeSequence[0].size();
		for (int i=0; i<numnodes; i++) {
			if (numbers.find(ReconNodeLabel[i][1])==string::npos) {
				for (int j=0; j<sequencelength; j++) {
					switch (GeneratorSequence[permutation[i]][j]) {
						case 'e':
							switch (ReconNodeSequence[i][j]) {
								case 'e':
									TP[i]++;
									break;
								case 'f':
									FP[i]++;
									break;
								case 'g':
									FP[i]++;
									break;
								case 'i':
									FN[i]++;
									break;
								case 'j':
									FN[i]++;
									break;
								case 'k':
									FN[i]++;
									break;
								case 'x':
									FN[i]++;
									break;
								default:
									break;
							}
							break;
						case 'f':
							switch (ReconNodeSequence[i][j]) {
								case 'e':
									FP[i]++;
									break;
								case 'f':
									TP[i]++;
									break;
								case 'g':
									FP[i]++;
									break;
								case 'i':
									FN[i]++;
									break;
								case 'j':
									FN[i]++;
									break;
								case 'k':
									FN[i]++;
									break;
								case 'x':
									FN[i]++;
									break;
								default:
									break;
							}
							break;
						case 'g':
							switch (ReconNodeSequence[i][j]) {
								case 'e':
									TP[i]++;
									break;
								case 'f':
									FP[i]++;
									break;
								case 'g':
									TP[i]++;
									break;
								case 'i':
									FN[i]++;
									break;
								case 'j':
									FN[i]++;
									break;
								case 'k':
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
							switch (ReconNodeSequence[i][j]) {
								case 'e':
									FP[i]++;
									break;
								case 'f':
									FP[i]++;
									break;
								case 'g':
									FP[i]++;
									break;
								case 'i':
									TP[i]++;
									break;
								case 'j':
									FP[i]++;
									break;
								case 'k':
									FP[i]++;
									break;
								case 'x':
									FN[i]++;
									break;
							}
							break;
						case 'j':
							switch (ReconNodeSequence[i][j]) {
								case 'e':
									FP[i]++;
									break;
								case 'f':
									FP[i]++;
									break;
								case 'g':
									FP[i]++;
									break;
								case 'i':
									FP[i]++;
									break;
								case 'j':
									TP[i]++;
									break;
								case 'k':
									FP[i]++;
									break;
								case 'x':
									FN[i]++;
									break;
							}
							break;
						case 'k':
							switch (ReconNodeSequence[i][j]) {
								case 'e':
									FP[i]++;
									break;
								case 'f':
									FP[i]++;
									break;
								case 'g':
									FP[i]++;
									break;
								case 'i':
									FP[i]++;
									break;
								case 'j':
									FP[i]++;
									break;
								case 'k':
									TP[i]++;
									break;
								case 'x':
									FN[i]++;
									break;
							}
							break;
						case 'x':
							switch (ReconNodeSequence[i][j]) {
								case 'e':
									FP[i]++;
									break;
								case 'f':
									FP[i]++;
									break;
								case 'g':
									FP[i]++;
									break;
								case 'i':
									FP[i]++;
									break;
								case 'j':
									FP[i]++;
									break;
								case 'k':
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
					if (ReconNodeSequence[i][j]!=GeneratorSequence[permutation[i]][j]) {
						cout << "At " << GeneratorNodeLabel[permutation[i]] << " position " << j << " " << GeneratorSequence[permutation[i]][j] << "->" << ReconNodeSequence[i][j] << endl;
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
				if (ReconNodeLabel[i]==thisnode) {
					if (writeraw) {
						outErrors << TP[i] << " " << TN[i] << " " << FP[i] << " " << FN[i] << " ";
					}
					if (writemcc) {
						outErrors << ((float)TP[i]*(float)TN[i]-(float)FP[i]*(float)FN[i])/sqrt(((float)TP[i]+(float)FP[i])*((float)TP[i]+(float)FN[i])*((float)TN[i]+(float)FP[i])*((float)TN[i]+(float)FN[i])) << " ";
					}
				}
			}
		}
		
		outErrors << endl;
		outErrors.close();
	}
	
	return 0;
}