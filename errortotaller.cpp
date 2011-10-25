#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
using namespace std;

int main(int argc, char *argv[]) {
	if (argc < 3) {
		cerr << "Please exucute like" << endl << "./ErrorTotaler errorfile lengthgrouping" << endl;
		exit(1);
	}
	
	ifstream inFile;
	inFile.open(argv[1]);
	if (!inFile) {
		cout << "Unable to open error file: " << argv[1] << endl;
		exit(1);
	}
	int numlines=0;
	string line;
	getline(inFile,line);
	int linelength=0;
	for (int i=0; i<line.size(); i++) {
		if (line[i]==' ') {
			linelength++;
		}
	}
	numlines++;
	while (getline(inFile,line)) {
		numlines++;
	}
	inFile.close();
	
	inFile.open("errors");
	int errordata[numlines][linelength];
	for (int i=0; i<numlines; i++) {
		for (int j=0; j<linelength; j++) {
			inFile >> errordata[i][j];
		}
	}
	
	int sequenceslength[numlines];
	for (int i=0; i<numlines; i++) {
		sequenceslength[i]=errordata[i][linelength-1]+errordata[i][linelength-2]+errordata[i][linelength-3]+errordata[i][linelength-4]; //this works since root is only node at greatest depth
	}
	int	maxseqlength=0;
	for (int i=0; i<numlines; i++) {
		if (maxseqlength<sequenceslength[i]) {
			maxseqlength=sequenceslength[i];
		}
	}
	
	int lengthgrouping=atoi(argv[2]);
	int databyseqlength[maxseqlength][linelength+1];
	for (int i=0; i<maxseqlength; i++) {
		for (int j=0; j<linelength+1; j++) {
			databyseqlength[i][j]=0;
		}
	}
	for (int i=0; i<numlines; i++) {
		databyseqlength[(sequenceslength[i]-1)/lengthgrouping][0]=sequenceslength[i]-sequenceslength[i]%lengthgrouping+1;
		for (int j=0; j<linelength; j++) {
			databyseqlength[(sequenceslength[i]-1)/lengthgrouping][j+1]+=errordata[i][j];
		}
	}
	
	for (int i=0; i<maxseqlength; i++) {
		if (databyseqlength[i][0] != 0) {
			for (int j=0; j<linelength+1; j++) {
				cout << databyseqlength[i][j] << " ";
			}
			cout << endl;
		}
	}
	
	return 0;
}