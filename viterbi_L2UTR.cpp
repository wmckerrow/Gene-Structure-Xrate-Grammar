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
		cerr << "Please execute like " << argv[0] << " ppfile TransBases" << endl;
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
	float logpps[numnodes][sequencelength][11];
	for (int i=0; i<numnodes; i++) {
		for (int j=0; j<sequencelength; j++) {
			for (int k=0; k<11; k++) {
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
			
			if (thisprobstring[2]=='5'){
				if(from_string<float>(logpps[thisnode][position-1][7], currentpp, std::dec)) {
					logpps[thisnode][position-1][7]=-log(logpps[thisnode][position-1][7]);
				}
				else {
					cerr << "from_string failed" << endl;
				}
			}
			
			if (thisprobstring[2]=='3'){
				if(from_string<float>(logpps[thisnode][position-1][8], currentpp, std::dec)) {
					logpps[thisnode][position-1][8]=-log(logpps[thisnode][position-1][8]);
				}
				else {
					cerr << "from_string failed" << endl;
				}
			}
			
			if (thisprobstring[2]=='u'){
				if(from_string<float>(logpps[thisnode][position-1][9], currentpp, std::dec)) {
					logpps[thisnode][position-1][9]=-log(logpps[thisnode][position-1][9]);
				}
				else {
					cerr << "from_string failed" << endl;
				}
			}
			
			if (thisprobstring[2]=='v'){
				if(from_string<float>(logpps[thisnode][position-1][10], currentpp, std::dec)) {
					logpps[thisnode][position-1][10]=-log(logpps[thisnode][position-1][10]);
				}
				else {
					cerr << "from_string failed" << endl;
				}
			}
			
			if (thisprobstring[2] != 'e' && thisprobstring[2] != 'f' && thisprobstring[2] != 'g' && thisprobstring[2] != 'i' && thisprobstring[2] != 'j' && thisprobstring[2] != 'k' && thisprobstring[2] != 'x' && thisprobstring[2] != '5' && thisprobstring[2] != '3' && thisprobstring[2] != 'u' && thisprobstring[2] != 'v') {
				cerr << "Couldn't get letter " << thisprobstring[2] << endl;
			}
			ppfile >> thisprobstring;
		}
		startline=thisprobstring;
	}
	ppfile.close();
	
	ifstream basesfile;
	basesfile.open(argv[2]);
	if (!basesfile) {
		cerr << "Unable to open input file " << argv[2] << endl;
		exit(1);
	}
	
	int numlines=0;
	while (getline(basesfile,line)) {
		numlines++;
	}
	string sequencegroups[numlines/2];
	string basestrings[numlines/2];
	basesfile.close();
	basesfile.open(argv[2]);
	for (int i=0; i<numlines/2; i++) {
		getline(basesfile,sequencegroups[i]);
		getline(basesfile,basestrings[i]);
	}
	basesfile.close();
	
	/*
	for (int i=0; i<numnodes; i++) {
		cout << endl << nodelabels[i] << endl;
		for (int j=0; j<sequencelength; j++) {
			for (int k=0; k<7; k++) {
				cout << logpps[i][j][k] << " ";
			}
			cout << endl;
		}
	}
	 */
	
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
	string sequenceH;
	string sequenceG;
	string sequenceA;
	string sequenceB;
	string sequenceI;
	string sequenceJ;
	string sequenceT;
	string sequenceK;
	string sequenceL;
	string sequenceM;
	string sequenceX;
	string sequence5;
	string sequence3;
	string sequenceU;
	string sequenceV;	
	string nextsequenceE;
	string nextsequenceF;
	string nextsequenceH;
	string nextsequenceG;
	string nextsequenceA;
	string nextsequenceB;
	string nextsequenceI;
	string nextsequenceJ;
	string nextsequenceT;
	string nextsequenceK;
	string nextsequenceL;
	string nextsequenceM;
	string nextsequenceX;
	string nextsequence5;
	string nextsequence3;
	string nextsequenceU;
	string nextsequenceV;
	float scoreE;
	float scoreF;
	float scoreH;
	float scoreG;
	float scoreA;
	float scoreB;
	float scoreI;
	float scoreJ;
	float scoreT;
	float scoreK;
	float scoreL;
	float scoreM;
	float scoreX;
	float score5;
	float score3;
	float scoreU;
	float scoreV;
	float nextscoreE;
	float nextscoreF;
	float nextscoreH;
	float nextscoreG;
	float nextscoreA;
	float nextscoreB;
	float nextscoreI;
	float nextscoreJ;
	float nextscoreT;
	float nextscoreK;
	float nextscoreL;
	float nextscoreM;
	float nextscoreX;
	float nextscore5;
	float nextscore3;
	float nextscoreU;
	float nextscoreV;
	float minexonscore;
	string minexonsequence;
	string arsequences[numnodes];
	float arscores[numnodes];
	char base1;
	char base2;
	char base3;
	char base4;
	
	for (int i=0; i<numnodes; i++) {
		int sequencegroupnum=-1;
		for (int j=0; j<numlines/2; j++) {
			if (sequencegroups[j].find(nodelabels[i]) != string::npos) {
				sequencegroupnum=j;
			}
		}
		
		sequenceE="";
		sequenceF="";
		sequenceH="";
		sequenceG="";
		sequenceA="";
		sequenceB="";
		sequenceI="";
		sequenceJ="";
		sequenceT="";
		sequenceK="";
		sequenceL="";
		sequenceM="";
		sequenceX="";
		sequence5="";
		sequence3="";
		sequenceU="";
		sequenceV="";
		scoreE=0;
		scoreF=0;
		scoreH=0;
		scoreG=0;
		scoreA=0;
		scoreB=0;
		scoreI=0;
		scoreJ=0;
		scoreT=0;
		scoreK=0;
		scoreL=0;
		scoreM=0;
		scoreX=0;
		score5=0;
		score3=0;
		scoreU=0;
		scoreV=0;
		
		for (int j=0; j<sequencelength; j++) {
			//cout << nodelabels[i] << " " << j+1 << ": score(g)=" << logpps[i][j][2] << " score(k)=" << logpps[i][j][5] << endl;
			//cout << nodelabels[i] << " " << j+1 << ": scoreG=" << scoreG << " scoreK=" << scoreK << endl;
			
			if (sequencegroupnum == -1) {
				base1='x';
				base2='x';
				base3='x';
				base4='x';
			}
			else {
				base1 = basestrings[sequencegroupnum][5*j];
				base2 = basestrings[sequencegroupnum][5*j+1];
				base3 = basestrings[sequencegroupnum][5*j+2];
				base4 = basestrings[sequencegroupnum][5*j+3];
			}
			
			//best sequence ending e
			nextscoreE=scoreE+logpps[i][j][0];
			nextsequenceE=sequenceE+"e";
			if (scoreX+logpps[i][j][0] < nextscoreE) {
				nextscoreE=scoreX+logpps[i][j][0];
				nextsequenceE=sequenceX+"e";
			}
			if (scoreI+logpps[i][j][0] < nextscoreE) {
				nextscoreE=scoreI+logpps[i][j][0];
				nextsequenceE=sequenceI+"e";
			}
			if (score5+logpps[i][j][0] < nextscoreE) {
				nextscoreE=score5+logpps[i][j][0];
				nextsequenceE=sequence5+"e";
			}
			
			//best sequence ending f
			nextscoreF = scoreF + logpps[i][j][1];
			nextsequenceF = sequenceF + "f";
			if (scoreJ+logpps[i][j][1] < nextscoreF && !((base3=='a' && base4=='a') || (base3=='a' && base4=='g') || (base3=='g' && base4=='a'))) {
				nextscoreF = scoreJ + logpps[i][j][1];
				nextsequenceF = sequenceJ + "f";
			}
			if (scoreT+logpps[i][j][1] < nextscoreF && !((base3=='a' && base4=='a') || (base3=='a' && base4=='g') || (base3=='g' && base4=='a'))) {
				nextscoreF = scoreT + logpps[i][j][1];
				nextsequenceF = sequenceT + "f";
			}
			
			//best sequence ending h
			nextscoreH = scoreH + logpps[i][j][1];
			nextsequenceH = sequenceH + "h";
			if (scoreJ+logpps[i][j][1] < nextscoreH && ((base3=='a' && base4=='a') || (base3=='a' && base4=='g') || (base3=='g' && base4=='a')) ) {
				nextscoreH = scoreJ + logpps[i][j][1];
				nextsequenceH = sequenceJ + "h";
			}
			
			//best sequence ending g
			nextscoreG = scoreG + logpps[i][j][2];
			nextsequenceG = sequenceG + "g";
			if (scoreK+logpps[i][j][2] < nextscoreG && base3 != 'a' && base3 != 'g') {
				nextscoreG=scoreK+logpps[i][j][2];
				nextsequenceG=sequenceK+"g";
			}
			if (scoreL+logpps[i][j][2] < nextscoreG && base3 != 'a' && base3 != 'g') {
				nextscoreG=scoreL+logpps[i][j][2];
				nextsequenceG=sequenceL+"g";
			}
			if (scoreM+logpps[i][j][2] < nextscoreG && base3 != 'a' && base3 != 'g') {
				nextscoreG=scoreM+logpps[i][j][2];
				nextsequenceG=sequenceM+"g";
			}
			
			//best sequence ending a
			nextscoreA = scoreA + logpps[i][j][2];
			nextsequenceA = sequenceA + "a";
			if (scoreK+logpps[i][j][2] < nextscoreA && base3=='a') {
				nextscoreA=scoreK+logpps[i][j][2];
				nextsequenceA = sequenceK + "a";
			}
			
			//best sequence ending b
			nextscoreB = scoreB + logpps[i][j][2];
			nextsequenceB = sequenceB + "b";
			if (scoreK+logpps[i][j][2] < nextscoreB && base3=='g') {
				nextscoreB = scoreK + logpps[i][j][2];
				nextsequenceB = sequenceK + "b";
			}
			if (scoreM+logpps[i][j][2] < nextscoreB && base3=='g') {
				nextscoreB = scoreM + logpps[i][j][2];
				nextsequenceB = sequenceM + "b";
			}
			
			//find min exon score and corresponding sequence (this will be used many times)
			minexonscore=scoreE;
			minexonsequence=sequenceE;
			if (scoreF < minexonscore) {
				minexonscore=scoreF;
				minexonsequence=sequenceF;
			}
			if (scoreH < minexonscore) {
				minexonscore=scoreH;
				minexonsequence=sequenceH;
			}
			if (scoreG < minexonscore) {
				minexonscore=scoreG;
				minexonsequence=sequenceG;
			}
			if (scoreA < minexonscore) {
				minexonscore=scoreA;
				minexonsequence=sequenceA;
			}
			if (scoreB < minexonscore) {
				minexonscore=scoreB;
				minexonsequence=sequenceB;
			}
			
			//best sequence ending i
			if (minexonscore < scoreI) {
				nextscoreI = minexonscore + logpps[i][j][3];
				nextsequenceI = minexonsequence + "i";
			}
			else {
				nextscoreI = scoreI + logpps[i][j][3];
				nextsequenceI = sequenceI + "i";
			}
			
			//best sequence ending j
			if (minexonscore < scoreJ && base2 != 't') {
				nextscoreJ = minexonscore + logpps[i][j][4];
				nextsequenceJ = minexonsequence + "j";
			}
			else {
				nextscoreJ = scoreJ + logpps[i][j][4];
				nextsequenceJ = sequenceJ + "j";
			}
			
			//best sequence ending t
			if (minexonscore < scoreT && base2=='t') {
				nextscoreT=minexonscore + logpps[i][j][4];
				nextsequenceT=minexonsequence + "t";
			}
			else {
				nextscoreT=scoreT+logpps[i][j][4];
				nextsequenceT=sequenceT+"t";
			}
			
			//best sequence ending k
			if (minexonscore < scoreK && !(base1=='t' && base2=='a') && !(base1=='t' && base2=='g')) {
				nextscoreK = minexonscore + logpps[i][j][5];
				nextsequenceK = minexonsequence + "k";
			}
			else {
				nextscoreK = scoreK + logpps[i][j][5];
				nextsequenceK = sequenceK + "k";
			}
			
			//best sequence ending l
			if (minexonscore < scoreL && base1=='t' && base2=='a') {
				nextscoreL=minexonscore + logpps[i][j][5];
				nextsequenceL=minexonsequence + "l";
			}
			else {
				nextscoreL=scoreL+logpps[i][j][5];
				nextsequenceL=sequenceL+"l";
			}
			
			//best sequence ending m
			if (minexonscore < scoreM && base1=='t' && base2=='g') {
				nextscoreM=minexonscore + logpps[i][j][5];
				nextsequenceM=minexonsequence + "m";
			}
			else {
				nextscoreM=scoreM+logpps[i][j][5];
				nextsequenceM=sequenceM+"m";
			}
			
			//best sequence ending x
			nextscoreX = scoreX + logpps[i][j][6];
			nextsequenceX = sequenceX + "x";
			if (minexonscore + logpps[i][j][6] < nextscoreX) {
				nextscoreX = minexonscore + logpps[i][j][6];
				nextsequenceX = minexonsequence + "x";
			}
			if (score3 + logpps[i][j][6] < nextscoreX) {
				nextscoreX = score3 + logpps[i][j][6];
				nextsequenceX = sequence3 + "x";
			}
			
			//best sequence ending 5
			nextscore5 = score5 + logpps[i][j][7];
			nextsequence5 = sequence5 + "5";
			if (scoreX + logpps[i][j][7] < nextscore5) {
				nextscore5 = scoreX + logpps[i][j][7];
				nextsequence5 = sequenceX + "5";
			}
			if (scoreU + logpps[i][j][7] < nextscore5) {
				nextscore5 = scoreU + logpps[i][j][7];
				nextsequence5 = sequenceU + "5";
			}
			
			//best sequence ending 3
			nextscore3 = score3 + logpps[i][j][8];
			nextsequence3 = sequence3 + "3";
			if (minexonscore + logpps[i][j][8] < nextscore3) {
				nextscore3 = minexonscore + logpps[i][j][8];
				nextsequence3 = minexonsequence + "3";
			}
			if (scoreV + logpps[i][j][8] < nextscore3) {
				nextscore3 = scoreV + logpps[i][j][8];
				nextsequence3 = sequenceV + "3";
			}
			
			//best sequence ending u
			if (score5 < scoreU) {
				nextscoreU = score5 + logpps[i][j][9];
				nextsequenceU = sequence5 + "u";
			}
			else {
				nextscoreU = scoreU + logpps[i][j][9];
				nextsequenceU = sequenceU + "u";
			}
			
			//best score ending v
			if (score3 < scoreV) {
				nextscoreV = score3 + logpps[i][j][10];
				nextsequenceV = sequence3 + "v";
			}
			else {
				nextscoreV = scoreV + logpps[i][j][10];
				nextsequenceV = sequenceV + "v";
			}
			
			//update scores and sequences
			scoreE=nextscoreE;
			scoreF=nextscoreF;
			scoreH=nextscoreH;
			scoreG=nextscoreG;
			scoreA=nextscoreA;
			scoreB=nextscoreB;
			scoreI=nextscoreI;
			scoreJ=nextscoreJ;
			scoreT=nextscoreT;
			scoreK=nextscoreK;
			scoreL=nextscoreL;
			scoreM=nextscoreM;
			scoreX=nextscoreX;
			score5=nextscore5;
			score3=nextscore3;
			scoreU=nextscoreU;
			scoreV=nextscoreV;
			sequenceE=nextsequenceE;
			sequenceF=nextsequenceF;
			sequenceH=nextsequenceH;
			sequenceG=nextsequenceG;
			sequenceA=nextsequenceA;
			sequenceB=nextsequenceB;
			sequenceI=nextsequenceI;
			sequenceJ=nextsequenceJ;
			sequenceT=nextsequenceT;
			sequenceK=nextsequenceK;
			sequenceL=nextsequenceL;
			sequenceM=nextsequenceM;
			sequenceX=nextsequenceX;
			sequence5=nextsequence5;
			sequence3=nextsequence3;
			sequenceU=nextsequenceU;
			sequenceV=nextsequenceV;
			
			//cout << scoreE << " " << scoreI << " " << scoreX << endl;
			//cout << sequenceE << endl << sequenceI << endl << sequenceX << endl;
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