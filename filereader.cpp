#define MAXCHILDREN 4
#define MAXDEPTH 4
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main(int argc, char *argv[]) {
	ifstream inFile;
	inFile.open(argv[1]);
	string Newick;
	float	EtoI, XtoEnd, xetoxx;
	if (!inFile) {
		cerr << "Unable to open input file " << argv[1] << endl;
		exit(1);
	}
	inFile >> Newick>>EtoI>>XtoEnd>>xetoxx;
	cout << Newick << " " << EtoI << " " <<XtoEnd << " " <<xetoxx << endl;
	return 0;
}