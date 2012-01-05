/*Convert the e i x alphabet into the holes alphabet*/
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <math.h>
using namespace std;

int main (int argc, char *argv[]) {
	ifstream inFile;
	inFile.open(argv[1]);
	if (!inFile) {
		cerr << "Unable to open .stk file: " << argv[1] << endl;
		exit(1);
	}
	int numlines=0;
	string line;
	while (getline(inFile,line)) {
		numlines++;
	}
	string stockholmfile[numlines];
	inFile.close();
	inFile.open(argv[1]);
	for (int i=0; i < numlines; i++) {
		getline(inFile,stockholmfile[i]);
	}
	inFile.close();
	int constcolumns=0;
	for (int i=10; i<stockholmfile[2].size(); i++) {
		char topletter = stockholmfile[2][i];
		int agrees=1;
		for (int j=3; j<numlines; j++) {
			if (stockholmfile[j][i]==topletter) {
				agrees++;
			}
		}
		if (agrees == numlines - 2) {
			constcolumns++;
		};
	}
	int constpos[constcolumns];
	char constletter[constcolumns];
	int arraypos=0;
	for (int i=10; i<stockholmfile[2].size(); i++) {
		char topletter = stockholmfile[2][i];
		int agrees=1;
		for (int j=3; j<numlines; j++) {
			if (stockholmfile[j][i]==topletter) {
				agrees++;
			}
		}
		if (agrees == numlines - 2) {
			constpos[arraypos]=i-10;
			constletter[arraypos]=topletter;
			arraypos++;
		};
	}
	
	int numleft[numlines-2][constcolumns-1];
	for (int k=2; k<numlines; k++) {
		for (int i=0; i<constcolumns-1; i++) {
			numleft[k-2][i]=0;
			int j=constpos[i]+11;
			if (constpos[i+1] != constpos[i]+1) {
				while (stockholmfile[k][j]==constletter[i]) {
					numleft[k-2][i]++;
					j++;
				}
			}
			else {
				numleft[k-2][i]=0;
			}
		}
	}
	

	for (int i=0; i<numlines; i++) {
		//cout << stockholmfile[i] << endl;
	}
	for (int i=2; i<numlines; i++) {
		stockholmfile[i]=stockholmfile[i].substr(0,10)+constletter[0];
		for (int j=0; j<constcolumns-1; j++) {
			for (int k=0; k<numleft[i-2][j]; k++) {
				stockholmfile[i]+=constletter[j];
			}
			for (int k=0; k<constpos[j+1]-constpos[j]-numleft[i-2][j]-1; k++) {
				stockholmfile[i]+=static_cast<char>(constletter[j]+1);
			}
			for (int k=0; k<constpos[j+1]-constpos[j]-numleft[i-2][j]-1; k++) {
				stockholmfile[i]+=constletter[j+1];
			}
			for (int k=0; k<numleft[i-2][j]; k++) {
				stockholmfile[i]+=static_cast<char>(constletter[j+1]+1);
			}
			stockholmfile[i]+=constletter[j+1];
		}
	}
	for (int i=0; i<numlines; i++) {
		cout << stockholmfile[i] << endl;
	}
}