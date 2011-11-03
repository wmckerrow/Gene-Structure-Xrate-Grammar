#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
using namespace std;

string convertInt(int number)
{
	stringstream ss;//create a stringstream
	ss << number;//add number to the stream
	return ss.str();//return a string with the contents of the stream
}

int main (int argc, char *argv[]) {
	if (argc < 4) {
		cerr << "Please executre like ./reevidence input.gene nodelabel numevidence" << endl;
		exit(1);
	}
	
	ifstream inFile;
	inFile.open(argv[1]);
	if (!inFile) {
		cerr << "Unable to open input file " << argv[1] << endl;
		exit(1);
	}
	string newick;
	inFile >> newick;
	inFile.close();
	string nodelabel=argv[2];
	string lookforthis = ")" + nodelabel + ":";
	
	cout << newick << endl;
	
	size_t found;
	found = newick.find(lookforthis);
	if (found == string::npos) {
		cerr << "Could not find node " << nodelabel << endl;
		exit(1);
	}
	int start=(int)found-1;
	while (newick[start] != '(') {
		if (newick[start]==')') {
			cerr << nodelabel << " does not appear to be a species node." << endl;
			exit(1);
		}
		start--;
	}
	
	int numevidence = atoi(argv[3]);
	string newevidence = "(";
	for (int i=1; i<=numevidence; i++) {
		newevidence+=nodelabel+convertInt(i)+":.1";
		if (i<numevidence) {
			newevidence+=",";
		}
	}
	newevidence+=")";
	
	newick.replace(start,(int)found-start+1,newevidence);
	cout << newick << endl;
	
	ifstream getinput;
	ofstream rewrite;
	string line;
	
	getinput.open(argv[1]);
	int inputlines=0;
	if (getinput) {
		while (getline(getinput,line)) {
			inputlines++;
		}
	}
	else {
		cerr << "Couldn't open " << argv[1] << endl;
		exit(1);
	}
	
	string inputfile[inputlines];
	getinput.close();
	getinput.open(argv[1]);
	if (getinput) {
		for (int i=0; i<inputlines; i++) {
			getline(getinput,inputfile[i]);
		}
	}
	getinput.close();
	
	rewrite.open(argv[1]);
	rewrite << newick << endl;
	for (int i=1; i<inputlines; i++) {
		rewrite << inputfile[i] << endl;
	}	
	
	return 0;
}