#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <math.h>
using namespace std;

template <class T>
bool from_string(T& t, 
                 const std::string& s, 
                 std::ios_base& (*f)(std::ios_base&))
{
	std::istringstream iss(s);
	return !(iss >> f >> t).fail();
}

int main (int argc, char *argv[]) {
	if (argc < 3) {
		cerr << "Please execute like ./viterbi ppfile multiplier" << endl;
		exit(1);
	}
	
	float multi=atof(argv[2]);
	
	ifstream ppfile;
	ppfile.open(argv[1]);
	if (!ppfile) {
		cerr << "Unable to open posterior probability file " << argv[1] << endl;
		exit(1);
	}
	string line;
	string lastline;
	int numnodes=0;
	while (getline(ppfile,line)) {
		if (line[0]!='#') {
			numnodes++;
			lastline=line;
		}
	}
	ppfile.close();
	
	int sequencelength = 0;
	for (int i=0; i<lastline.length(); i++) {
		if (lastline[i]=='*') {
			sequencelength++;
		}
	}
	
	string startline;
	string nodelabels[numnodes+1];
	int thisnode=0;
	string thislabel;
	string trash;
	string thisprobstring;
	float logpps[numnodes][sequencelength][3];
	for (int i=0; i<numnodes; i++) {
		for (int j=0; j<sequencelength; j++) {
			for (int k=0; k<3; k++) {
				logpps[i][j][k]=-log(0.000001);
			}
		}
	}
	string currentpp;
	string positionstr;
	int position;
	
	ppfile.open(argv[1]);
	ppfile >> startline;
	while (startline != "#=GS") {
		ppfile >> startline;
	}
	ppfile >> thislabel;
	nodelabels[0]=thislabel;
	bool firstime=1;
	
	while (startline != nodelabels[0]) {
		if (!firstime) {
			ppfile >> thislabel;
		}
		firstime=0;
		if (thislabel != nodelabels[thisnode]) {
			thisnode++;
			nodelabels[thisnode]=thislabel;
		}
		ppfile >> trash;
		while (trash[0]!='c') {
			ppfile >> trash;
		}
		ppfile >> positionstr;
		if (!from_string<int>(position,positionstr.substr(1,positionstr.length()-2),std::dec)) {
			cerr << "from_string failed" << endl;
		}
		ppfile >> thisprobstring;
		while (thisprobstring[0]=='P') {
			currentpp=thisprobstring.substr(5,thisprobstring.length()-5);
			if (thisprobstring[2]=='e'){
				if(from_string<float>(logpps[thisnode][position-1][0], currentpp, std::dec)) {
					logpps[thisnode][position-1][0]=-log(logpps[thisnode][position-1][0]);
				}
				else {
					cerr << "from_string failed" << endl;
				}

			}
			if (thisprobstring[2]=='i'){
				if(from_string<float>(logpps[thisnode][position-1][1], currentpp, std::dec)) {
					logpps[thisnode][position-1][1]=-log(logpps[thisnode][position-1][1]);
				}
				else {
					cerr << "from_string failed" << endl;
				}

			}
			if (thisprobstring[2]=='x'){
				if(from_string<float>(logpps[thisnode][position-1][2], currentpp, std::dec)) {
					logpps[thisnode][position-1][2]=-log(logpps[thisnode][position-1][2]);
				}
				else {
					cerr << "from_string failed" << endl;
				}

			}
			if (thisprobstring[2] != 'e' && thisprobstring[2] != 'i' && thisprobstring[2] != 'x') {
				cerr << "Couldn't get letter" << endl;
			}
			ppfile >> thisprobstring;
		}
		startline=thisprobstring;
	}
	ppfile.close();
	
	/*
	for (int i=0; i<numnodes; i++) {
		cout << nodelabels[i] << " ";
	}
	cout << endl;
	
	for (int i=0; i<numnodes; i++) {
		for (int k=0; k<3; k++) {
			for (int j=0; j<min(20,sequencelength); j++) {
				cout << logpps[i][j][k] << " ";
			}
			cout << endl;
		}
		cout << endl << endl;
	}
	 */
	
	string sequenceE;
	string sequenceI;
	string sequenceX;
	string nextsequenceE;
	string nextsequenceI;
	string nextsequenceX;
	float scoreE;
	float scoreI;
	float scoreX;
	float nextscoreE;
	float nextscoreI;
	float nextscoreX;
	string arsequences[numnodes];
	float arscores[numnodes];
	
	for (int i=0; i<numnodes; i++) {
		sequenceE="";
		sequenceI="";
		sequenceX="";
		scoreE=0;
		scoreI=0;
		scoreX=0;
		for (int j=0; j<sequencelength; j++) {
			//best sequence ending e
			if (scoreI + logpps[i][j][0] < (scoreE + logpps[i][j][0])*multi) {
				if (scoreI + logpps[i][j][0] < scoreX + logpps[i][j][0]) {
					nextscoreE=scoreI+logpps[i][j][0];
					nextsequenceE=sequenceI + "e";
				}
				else {
					nextscoreE=scoreX + logpps[i][j][0];
					nextsequenceE=sequenceX + "e";
				}
			}
			else {
				if ((scoreE + logpps[i][j][0])*multi < scoreX + logpps[i][j][0]) {
					nextscoreE=(scoreE + logpps[i][j][0])*multi;
					nextsequenceE=sequenceE + "e";
				}
				else {
					nextscoreE=scoreX + logpps[i][j][0];
					nextsequenceE=sequenceX + "e";
				}
			}
			//best sequence ending i
			if ((scoreI + logpps[i][j][1])*multi < scoreE + logpps[i][j][1]) {
				nextscoreI = scoreI + logpps[i][j][1];
				nextsequenceI = sequenceI + "i";
			}
			else {
				nextscoreI = scoreE + logpps[i][j][1];
				nextsequenceI = sequenceE + "i";
			}
			//best sequence ending x
			if (scoreE + logpps[i][j][2] < (scoreX + logpps[i][j][2])*multi) {
				nextscoreX = scoreE + logpps[i][j][2];
				nextsequenceX = sequenceE + "x";
			}
			else {
				nextscoreX = scoreX + logpps[i][j][2];
				nextsequenceX = sequenceX + "x";
			}
			//update scores and sequences
			scoreE=nextscoreE;
			scoreI=nextscoreI;
			scoreX=nextscoreX;
			sequenceE=nextsequenceE;
			sequenceI=nextsequenceI;
			sequenceX=nextsequenceX;
		}
		if (scoreE < scoreX) {
			arsequences[i]=sequenceE;
			arscores[i]=scoreE;
		}
		else {
			arsequences[i]=sequenceX;
			arscores[i]=scoreX;
		}
	}
	
	for (int i=0; i<numnodes; i++) {
		cout << nodelabels[i];
		for (int j=0; j<10-nodelabels[i].length(); j++) {
			cout << " ";
		}
		cout << arsequences[i] << endl;
	}
	
	return 0;
}