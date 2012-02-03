#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <math.h>
using namespace std;

//finds out if a letter is a cds letter
bool iscds(char letter) {
	string cdsletters="efghab";
	if (cdsletters.find(letter) != string::npos) {
		return 1;
	}
	else {
		return 0;
	}
}

//finds out if a letter is a intron letter (unused)
bool isintron(char letter) {
	string intronletters="ijkzuvtlm";
	if (intronletters.find(letter) != string::npos) {
		return 1;
	}
	else {
		return 0;
	}
}

//finds out if a letter is a exon letter (unused)
bool isexon(char letter) {
	string exonletters="efgy53hab";
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
		cerr << "Please execute like " << argv[0] << " NonalignmentRows AligmentFileName NameForgffFiles PostionFile" << endl;
		exit(1);
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
	inFile.open(argv[2]);
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
	
	//find number of label columns and length of sequence
	int labelcolumns=0;
	while (stockholmfile[0][labelcolumns] != ' ') {
		labelcolumns++;
	}
	while (stockholmfile[0][labelcolumns] == ' ') {
		labelcolumns++;
	}
	int numsegpositions=stockholmfile[0].length()-labelcolumns+1;
	int segposition[numsegpositions];
	
	//open segment postions file and read into memory
	ifstream segfilein;
	segfilein.open(argv[4]);
	if (!segfilein) {
		cout << "Unable to open segmentation positions file " << argv[4] << endl;
		exit(1);
	}
	for (int i=0; i<numsegpositions; i++) {
		segfilein >> segposition[i];
	}
	segfilein.close();

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
		
	//write gff files
	ofstream outFile;
	string filenname;
	int labelchars;
	string label;
	int startgene;
	int endgene;
	bool foundcds;
	int startcds;
	int endcds;
	bool found5prime;
	int start5prime;
	int end5prime;
	bool found3prime;
	int start3prime;
	int end3prime;
	int mrnanum;
	int times=0; //debugging line
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
			while (!(stockholmfile[i][startgene-1]=='x' && stockholmfile[i][startgene]!='x') && startgene < stockholmfile[i].length()-1) {
				startgene++;	
			}
			endgene=startgene;
			while (stockholmfile[i][endgene] != 'x' && endgene < stockholmfile[i].length()-1) {
				endgene++;
			}
			/*
			if (times < 5) {
				cout << startgene << " " << endgene << endl;
				times++;
			}
			 */
			if (startgene < stockholmfile[i].length()-1) {
				mrnanum++;
				outFile << label << "	" << "." << '\t' << "gene" << '\t' << segposition[startgene-labelcolumns] << '\t' << segposition[endgene-labelcolumns]-1 << '\t' << "." << '\t' << "+" << '\t' << "." << '\t' << "ID=gene" << mrnanum << endl;
				outFile << label << "	" << "." << '\t' << "mRNA" << '\t' << segposition[startgene-labelcolumns] << '\t' << segposition[endgene-labelcolumns]-1 << '\t' << "." << '\t' << "+" << '\t' << "." << '\t' << "ID=mRNA" << mrnanum << ";parent=gene" << mrnanum << endl;
				foundcds=0;
				found5prime=0;
				found3prime=0;
				for (int j=startgene; j<endgene; j++) {
					if (iscds(stockholmfile[i][j]) && !foundcds) {
						foundcds=1;
						startcds=j;
					}
					if (stockholmfile[i][j] == '5' && !found5prime) {
						found5prime=1;
						start5prime=j;
					}
					if (stockholmfile[i][j] == '3' && !found3prime) {
						found3prime=1;
						start3prime=j;
					}
				}
				if (found5prime) {
					while (start5prime < endgene) {
						end5prime=start5prime;
						while (stockholmfile[i][end5prime] == '5' && end5prime <= endgene -1 ) {
							end5prime++;
						}
						end5prime--;
						outFile << label << "	" << "." << '\t' << "exon" << '\t' << segposition[start5prime-labelcolumns] << '\t' << segposition[end5prime-labelcolumns+1]-1 << '\t' << "." << '\t' << "+" << '\t' << "." << '\t' << "Parent=mRNA" << mrnanum << endl;
						start5prime=end5prime+1;
						while (stockholmfile[i][start5prime] != '5' && start5prime < endgene ) {
							start5prime++;
						}
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
				if (found3prime) {
					while (start3prime < endgene) {
						end3prime=start3prime;
						while (stockholmfile[i][end3prime] == '3' && end3prime <= endgene -1 ) {
							end3prime++;
						}
						end3prime--;
						outFile << label << "	" << "." << '\t' << "exon" << '\t' << segposition[start3prime-labelcolumns] << '\t' << segposition[end3prime-labelcolumns+1]-1 << '\t' << "." << '\t' << "+" << '\t' << "." << '\t' << "Parent=mRNA" << mrnanum << endl;
						start3prime=end3prime+1;
						while (stockholmfile[i][start3prime] != '3' && start3prime < endgene ) {
							start3prime++;
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