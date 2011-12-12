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
	if (argc < 2) {
		cerr << "Please execute like ./viterbi ppfile" << endl;
		exit(1);
	}
	
	//float multi=atof(argv[2]);
	
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
	float logpps[numnodes][sequencelength][7];
	for (int i=0; i<numnodes; i++) {
		for (int j=0; j<sequencelength; j++) {
			for (int k=0; k<7; k++) {
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
			if (thisprobstring[2]=='f'){
				if(from_string<float>(logpps[thisnode][position-1][1], currentpp, std::dec)) {
					logpps[thisnode][position-1][1]=-log(logpps[thisnode][position-1][1]);
				}
				else {
					cerr << "from_string failed" << endl;
				}
				
			}
			if (thisprobstring[2]=='g'){
				if(from_string<float>(logpps[thisnode][position-1][2], currentpp, std::dec)) {
					logpps[thisnode][position-1][2]=-log(logpps[thisnode][position-1][2]);
				}
				else {
					cerr << "from_string failed" << endl;
				}
				
			}
			if (thisprobstring[2]=='i'){
				if(from_string<float>(logpps[thisnode][position-1][3], currentpp, std::dec)) {
					logpps[thisnode][position-1][3]=-log(logpps[thisnode][position-1][3]);
				}
				else {
					cerr << "from_string failed" << endl;
				}

			}
			if (thisprobstring[2]=='j'){
				if(from_string<float>(logpps[thisnode][position-1][4], currentpp, std::dec)) {
					logpps[thisnode][position-1][4]=-log(logpps[thisnode][position-1][4]);
				}
				else {
					cerr << "from_string failed" << endl;
				}
				
			}
			if (thisprobstring[2]=='k'){
				if(from_string<float>(logpps[thisnode][position-1][5], currentpp, std::dec)) {
					logpps[thisnode][position-1][5]=-log(logpps[thisnode][position-1][5]);
				}
				else {
					cerr << "from_string failed" << endl;
				}
				
			}
			if (thisprobstring[2]=='x'){
				if(from_string<float>(logpps[thisnode][position-1][6], currentpp, std::dec)) {
					logpps[thisnode][position-1][6]=-log(logpps[thisnode][position-1][6]);
				}
				else {
					cerr << "from_string failed" << endl;
				}

			}
			if (thisprobstring[2] != 'e' && thisprobstring[2] != 'f' && thisprobstring[2] != 'g' && thisprobstring[2] != 'i' && thisprobstring[2] != 'j' && thisprobstring[2] != 'k' && thisprobstring[2] != 'x') {
				cerr << "Couldn't get letter " << thisprobstring[2] << endl;
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
		for (int k=0; k<7; k++) {
			for (int j=0; j<min(20,sequencelength); j++) {
				cout << logpps[i][j][k] << " ";
			}
			cout << endl;
		}
		cout << endl << endl;
	}
	 */
	 
	
	string sequenceE;
	string sequenceF;
	string sequenceG;
	string sequenceI;
	string sequenceJ;
	string sequenceK;
	string sequenceX;
	string nextsequenceE;
	string nextsequenceF;
	string nextsequenceG;
	string nextsequenceI;
	string nextsequenceJ;
	string nextsequenceK;
	string nextsequenceX;
	float scoreE;
	float scoreF;
	float scoreG;
	float scoreI;
	float scoreJ;
	float scoreK;
	float scoreX;
	float nextscoreE;
	float nextscoreF;
	float nextscoreG;
	float nextscoreI;
	float nextscoreJ;
	float nextscoreK;
	float nextscoreX;
	float minexonscore;
	string minexonsequence;
	string arsequences[numnodes];
	float arscores[numnodes];
	
	for (int i=0; i<numnodes; i++) {
		sequenceE="";
		sequenceF="";
		sequenceG="";
		sequenceI="";
		sequenceJ="";
		sequenceK="";
		sequenceX="";
		scoreE=0;
		scoreF=0;
		scoreG=0;
		scoreI=0;
		scoreJ=0;
		scoreK=0;
		scoreX=0;
		for (int j=0; j<sequencelength; j++) {
			//best sequence ending e
			if (scoreI < scoreE) {
				if (scoreI < scoreX) {
					nextscoreE=scoreI+logpps[i][j][0];
					nextsequenceE=sequenceI + "e";
				}
				else {
					nextscoreE=scoreX + logpps[i][j][0];
					nextsequenceE=sequenceX + "e";
				}
			}
			else {
				if (scoreE < scoreX) {
					nextscoreE=scoreE + logpps[i][j][0];
					nextsequenceE=sequenceE + "e";
				}
				else {
					nextscoreE=scoreX + logpps[i][j][0];
					nextsequenceE=sequenceX + "e";
				}
			}
			//best sequence ending f
			if (scoreJ < scoreF) {
				nextscoreF = scoreJ + logpps[i][j][1];
				nextsequenceF = sequenceJ + "f";
			}
			else {
				nextscoreF = scoreF + logpps[i][j][1];
				nextsequenceF = sequenceF + "f";
			}
			//best sequence ending g
			if (scoreK < scoreG) {
				nextscoreG = scoreK + logpps[i][j][2];
				nextsequenceG = sequenceK + "g";
			}
			else {
				nextscoreG = scoreG + logpps[i][j][2];
				nextsequenceG = sequenceG + "g";
			}
			
			minexonscore=scoreE;
			minexonsequence=sequenceE;
			if (scoreF < minexonscore) {
				minexonscore=scoreF;
				minexonsequence=sequenceF;
			}
			if (scoreG < minexonscore) {
				minexonscore=scoreG;
				minexonsequence=sequenceG;
			}
			
			//best sequence ending i
			if (scoreI < minexonscore) {
				nextscoreI = scoreI + logpps[i][j][3];
				nextsequenceI = sequenceI + "i";
			}
			else {
				nextscoreI = minexonscore + logpps[i][j][3];
				nextsequenceI = minexonsequence + "i";
			}
			
			//best sequence ending j
			if (scoreJ < minexonscore) {
				nextscoreJ = scoreJ + logpps[i][j][4];
				nextsequenceJ = sequenceJ + "j";
			}
			else {
				nextscoreJ = minexonscore + logpps[i][j][4];
				nextsequenceJ = minexonsequence + "j";
			}
			
			//best sequence ending k
			if (scoreK < minexonscore) {
				nextscoreK = scoreK + logpps[i][j][5];
				nextsequenceK = sequenceK + "k";
			}
			else {
				nextscoreK = minexonscore + logpps[i][j][5];
				nextsequenceK = minexonsequence + "k";
			}
			
			//best sequence ending x
			if (minexonscore < scoreX) {
				nextscoreX = minexonscore + logpps[i][j][5];
				nextsequenceX = minexonsequence + "x";
			}
			else {
				nextscoreX = scoreX + logpps[i][j][5];
				nextsequenceX = sequenceX + "x";
			}
			//update scores and sequences
			scoreE=nextscoreE;
			scoreF=nextscoreF;
			scoreG=nextscoreG;
			scoreI=nextscoreI;
			scoreJ=nextscoreJ;
			scoreK=nextscoreK;
			scoreX=nextscoreX;
			sequenceE=nextsequenceE;
			sequenceF=nextsequenceF;
			sequenceG=nextsequenceG;
			sequenceI=nextsequenceI;
			sequenceJ=nextsequenceJ;
			sequenceK=nextsequenceK;
			sequenceX=nextsequenceX;
		}
		arsequences[i]=sequenceX;
		arscores[i]=scoreX;
	}
	
	int maxlabellength=0;
	for (int i=0; i<numnodes; i++) {
		if (nodelabels[i].size()>maxlabellength) {
			maxlabellength=nodelabels[i].size();
		}
	}
	for (int i=0; i<numnodes; i++) {
		cout << nodelabels[i];
		for (int j=0; j<max(10,maxlabellength+4)-nodelabels[i].length(); j++) {
			cout << " ";
		}
		cout << arsequences[i] << endl;
	}
	
	return 0;
}