#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <math.h>
using namespace std;

bool iscds(char letter) {
	string cdsletters="efg";
	if (cdsletters.find(letter) != string::npos) {
		return 1;
	}
	else {
		return 0;
	}
}

bool isintron(char letter) {
	string intronletters="ijkz";
	if (intronletters.find(letter) != string::npos) {
		return 1;
	}
	else {
		return 0;
	}
}

bool isexon(char letter) {
	string exonletters="efgy";
	if (exonletters.find(letter) != string::npos) {
		return 1;
	}
	else {
		return 0;
	}
}


int main (int argc, char *argv[]) {
	//make sure we have enough input
	if (argc < 5) {
		cerr << "Please execute like " << argv[0] << " NonalignmentRows AligmentFileName NameForgffFiles TreeLine?" << endl;
		cerr << "To write lengths to file append -w filename. To read lengths from file append -r filename." << endl;
		exit(1);
	}
	
	//check for options
	string thisinput;
	bool write=0;
	bool read=0;
	int writearg;
	int readarg;
	for (int i=0; i<argc; i++) {
		thisinput=argv[i];
		if (thisinput == "-w") {
			write=1;
			writearg=i+1;
		}
		if (thisinput == "-r") {
			read=1;
			readarg=i+1;
		}
	}
	
	//open stk file
	ifstream inFile;
	inFile.open(argv[2]);
	if (!inFile) {
		cerr << "Unable to open .stk file: " << argv[2] << endl;
		exit(1);
	}
	
	//read stk file into memory
	int UnusedRows=atoi(argv[1]);
	int numlines=0;
	string line;
	while (getline(inFile,line)) {
		numlines++;
	}
	inFile.close();
	numlines-=UnusedRows;
	string stockholmfile[numlines];
	string tree="";
	inFile.open(argv[2]);
	if (argv[4][0] != '0' && argv[4][0] != 'n' && argv[4][0] != 'N') {
		getline(inFile,tree);
		UnusedRows--;
	}
	if (UnusedRows>0) {
		for (int i=0; i<UnusedRows; i++) {
			getline(inFile,line);
		}
	}
	for (int i=0; i < numlines; i++) {
		getline(inFile,stockholmfile[i]);
		//cout << stockholmfile[i] << endl;
	}
	inFile.close();
	
	//find number of label columns
	int labelcolumns=0;
	while (stockholmfile[0][labelcolumns] != ' ') {
		labelcolumns++;
	}
	while (stockholmfile[0][labelcolumns] == ' ') {
		labelcolumns++;
	}
	
	//find constant columns
	int constcolumns=0;
	int lastconstletter='a';
	for (int i=labelcolumns; i<stockholmfile[0].size(); i++) {
		char topletter = stockholmfile[0][i];
		int agrees=1;
		for (int j=1; j<numlines; j++) {
			if (stockholmfile[j][i]==topletter) {
				agrees++;
			}
		}
		if (agrees == numlines && topletter != lastconstletter) {
			constcolumns++;
			lastconstletter=topletter;
		};
	}
	int constpos[constcolumns];
	char constletter[constcolumns];
	int arraypos=0;
	lastconstletter='a';
	for (int i=labelcolumns; i<stockholmfile[0].size(); i++) {
		char topletter = stockholmfile[0][i];
		int agrees=1;
		for (int j=1; j<numlines; j++) {
			if (stockholmfile[j][i]==topletter) {
				agrees++;
			}
		}
		if (agrees == numlines && topletter != lastconstletter) {
			constpos[arraypos]=i-labelcolumns;
			constletter[arraypos]=topletter;
			arraypos++;
			lastconstletter=topletter;
		};
	}
	
	//count cds sections in each row
	int numcds[numlines];
	for (int i=0; i<numlines; i++) {
		numcds[i]=0;
		for (int j=labelcolumns; j<stockholmfile[i].size(); j++) {
			if (stockholmfile[i][j]=='e' || stockholmfile[i][j]=='f' || stockholmfile[i][j]=='g') {
				numcds[i]++;
			}
		}
	}
	
	//find max and mix number of cds segments
	int maxcds=numcds[0];
	int mincds=numcds[0];
	for (int i=1; i<numlines; i++) {
		if (maxcds < numcds[i]) {
			maxcds = numcds[i];
		}
		if (mincds > numcds[i]) {
			mincds = numcds[i];
		}
	}
	
	//find segment lengths for non constant columns that will give proper amout of cds content variance
	int seglengthforvariance;
	if (maxcds!=mincds) {
		seglengthforvariance=100/(maxcds-mincds);
	}
	else {
		seglengthforvariance=100;
	}
	int seglengthforsize=900/maxcds;
	int nonconstseglength=min(seglengthforsize,seglengthforvariance);
	nonconstseglength-=nonconstseglength%3;
	if (nonconstseglength<3) {
		nonconstseglength=3;
	}

	//find number of e and x constant columns
	int econstcols=0;
	int xconstcols=0;
	for (int i=0; i<constcolumns; i++) {
		if (constletter[i]=='e' || constletter[i]=='f' || constletter[i]=='g') {
			econstcols++;
		}
		if (constletter[i]=='x') {
			xconstcols++;
		}
	}
	
	//find constant segment lengths that will give proper cds and intergenic content
	int eiconstseglength=(int)(900.0-((maxcds-mincds)/2.0-econstcols)*nonconstseglength)/econstcols;
	eiconstseglength-=eiconstseglength%3;
	if (eiconstseglength < 3) {
		eiconstseglength=3;
	}
	int xconstseglength=(int)(5000.0-((maxcds-mincds)/2.0-xconstcols)*nonconstseglength)/xconstcols;
	
	//find positions of each segment
	int numsegpositions=stockholmfile[0].length()-labelcolumns+1;
	int segposition[numsegpositions];
	segposition[0]=1;
	segposition[1]=xconstseglength;
	int lastconstcolumn=0;
	for (int i=1; i < stockholmfile[0].length()-labelcolumns; i++) {
		if (i == constpos[lastconstcolumn+1]) {
			lastconstcolumn++;
			switch (constletter[lastconstcolumn]) {
				case 'e':
					segposition[i+1]=segposition[i]+eiconstseglength;
					if (constletter[lastconstcolumn+1] == 'j') {
						segposition[i+1]++;
					}
					if (constletter[lastconstcolumn+1] == 'k') {
						segposition[i+1]--;
					}
					break;
				case 'f':
					segposition[i+1]=segposition[i]+eiconstseglength;
					if (constletter[lastconstcolumn+1] == 'i' || constletter[lastconstcolumn+1] == 'x') {
						segposition[i+1]--;
					}
					if (constletter[lastconstcolumn+1] == 'k') {
						segposition[i+1]++;
					}
					break;
				case 'g':
					segposition[i+1]=segposition[i]+eiconstseglength;
					if (constletter[lastconstcolumn+1] == 'i' || constletter[lastconstcolumn+1] == 'x') {
						segposition[i+1]++;
					}
					if (constletter[lastconstcolumn+1] == 'j') {
						segposition[i+1]--;
					}
					break;
				case 'x':
					segposition[i+1]=segposition[i]+xconstseglength;
					break;
				default:
					segposition[i+1]=segposition[i]+eiconstseglength;
					break;
			}
		}
		else {
			segposition[i+1]=segposition[i]+nonconstseglength;
		}
	}
	
	//if read=1 then overwrite segposition with data from file
	if (read) {
		ifstream segfilein;
		segfilein.open(argv[readarg]);
		if (!segfilein) {
			cout << "Unable to open segmentation file " << argv[readarg] << endl;
			exit(1);
		}
		for (int i=0; i<numsegpositions; i++) {
			segfilein >> segposition[i];
		}
	}
	
	if (write) {
		ofstream segfileout;
		segfileout.open(argv[writearg]);
		for (int i=0; i<numsegpositions; i++) {
			segfileout << segposition[i] << " ";
		}
	}
	
	//output for debugging
	/*
	for (int i=0; i<stockholmfile[0].length()-labelcolumns+1; i++) {
		cout << segposition[i] << " ";
	}
	cout << endl;
	for (int i=0; i<constcolumns; i++) {
		cout << constpos[i] << " " << constletter[i] << endl;
	}
	*/
	
	//write tree file
	ofstream outFile;
	string filenname;
	if (tree != "") {
		filenname = argv[3];
		filenname += ".tree";
		outFile.open(filenname.c_str());
		outFile << tree;
		outFile.close();
	}
	
	//write gff files
	int labelchars;
	string label;
	int startgene;
	int endgene;
	bool foundcds;
	int startcds;
	int endcds;
	bool foundexon;
	int startexon;
	int endexon;
	int mrnanum;
	int times=0;
	for (int i=0; i<numlines; i++) {
		labelchars=0;
		while (stockholmfile[i][labelchars]!=' ') {
			labelchars++;
		}
		label=stockholmfile[i].substr(0,labelchars);
		filenname = argv[3];
		filenname += "_" + label + ".gff";
		outFile.open(filenname.c_str());
		outFile << "##gff-version 3" << endl;
		mrnanum=0;
		startgene=labelcolumns+1;
		while (startgene < stockholmfile[i].length()-1) {
			while (!(stockholmfile[i][startgene-1]=='x' && stockholmfile[i][startgene]=='e') && startgene < stockholmfile[i].length()-1) {
				startgene++;	
			}
			endgene=startgene;
			while (stockholmfile[i][endgene] != 'x' && endgene < stockholmfile[i].length()-1) {
				endgene++;
			}
			if (times < 5) {
				//cout << startgene << " " << endgene << endl;
				times++;
			}
			if (startgene < stockholmfile[i].length()-1) {
				mrnanum++;
				outFile << label << "	" << "." << '\t' << "gene" << '\t' << segposition[startgene-labelcolumns] << '\t' << segposition[endgene-labelcolumns]-1 << '\t' << "." << '\t' << "+" << '\t' << "." << '\t' << "ID=gene" << mrnanum << endl;
				outFile << label << "	" << "." << '\t' << "mRNA" << '\t' << segposition[startgene-labelcolumns] << '\t' << segposition[endgene-labelcolumns]-1 << '\t' << "." << '\t' << "+" << '\t' << "." << '\t' << "ID=mRNA" << mrnanum << ";parent=gene" << mrnanum << endl;
				foundcds=0;
				foundexon=0;
				for (int j=startgene; j<endgene; j++) {
					if ((stockholmfile[i][j]=='e' || stockholmfile[i][j]=='f' || stockholmfile[i][j]=='e') && !foundcds) {
						foundcds=1;
						startcds=j;
					}
					if (stockholmfile[i][j]=='y' && !foundexon) {
						foundexon=1;
						startexon=j;
					}
				}
				if (foundcds) {
					while (startcds < endgene) {
						endcds=startcds;
						while (iscds(stockholmfile[i][endcds]) && endcds <= endgene -1 ) {
							endcds++;
						}
						endcds--;
						outFile << label << "	" << "." << '\t' << "CDS" << '\t' << segposition[startcds-labelcolumns] << '\t' << segposition[endcds-labelcolumns+1]-1 << '\t' << "." << '\t' << "+" << '\t' << "." << '\t' << "Parent=mRNA" << mrnanum << endl;
						startcds=endcds+1;
						while (!iscds(stockholmfile[i][startcds]) && startcds < endgene ) {
							startcds++;
						}
					}
				}
				if (foundexon) {
					while (startexon < endgene) {
						endexon=startexon;
						while (isexon(stockholmfile[i][endexon]) && endcds <= endgene -1) {
							endexon++;
						}
						endexon--;
						outFile << label << "	" << "." << '\t' << "exon" << '\t' << segposition[startexon-labelcolumns] << '\t' << segposition[endexon-labelcolumns+1]-1 << '\t' << "." << '\t' << "+" << '\t' << "." << '\t' << "Parent=mRNA" << mrnanum << endl;
						startexon=endexon;
						while (!stockholmfile[i][startexon]=='y' && startcds < endgene) {
							startexon++;
						}
					}
				}
			}
			startgene=endgene;
		}
		outFile.close();
	}
	
	return 0;
	
}