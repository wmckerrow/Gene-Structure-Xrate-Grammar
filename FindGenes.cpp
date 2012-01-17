#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <math.h>
using namespace std;

int main (int argc, char *argv[]) {
	if (argc < 5) {
		cerr << "Execute like " << argv[0] << " stkfile GrammarFile BasesFile SegmentationFile" << endl;
		exit(1);
	}
	string templocation="";
	tempstring=argv[2];
	string command;
	command = "xrate -g " + tempstring + " out.stk -t trained.eg";
	system(command.c_str());
	tempstring=argv[1];
	command = "xrate -g trained.eg " + tempstring + " -arpp > ppout";
	system(command.c_str());
	tempstring=argv[3];
	command="./viterbi_L2UTR ppout " + tempstring + " > vitout";
	system(command.c_str());
	tempstring=argv[4];
	command="./stk2gff 0 vitout enigma " + tempstring;
	system(command.c_str());
}