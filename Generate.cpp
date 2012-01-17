#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <math.h>
using namespace std;

int main (int argc, char *argv[]) {
	if (argc < 2) {
		cerr << "Execute like " << argv[0] << " GeneratorInputFile" << endl;
		exit(1);
	}
	string templocation="";
	string command;
	string tempstring = argv[1];
	command="./Generator_L1UTR " + tempstring + " out.stk > " + templocation + "genout";
	system(command.c_str());
	command="./GenerateBases " + templocation + "genout > bases";
	system(command.c_str());
	command="./GeneratePositions 0" + templocation + "genout segfile";
	system(command.c_str());
	command="./stk2gff 0 " + templocation + "genout fromgen segfile";
	system(command.c_str());
}

/*
templocation can later be used as part of an option to put temporary files aside.
I should also add bin location and output location strings.
*/