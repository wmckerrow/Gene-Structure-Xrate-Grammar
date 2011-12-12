#include <iostream>
#include <string>
#include <fstream>
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

struct treenode {
	int data;
	string stringdata;
	treenode *left;
	treenode *right;
};

void inorderprint(treenode *root) {
	if (root!=NULL) {
		inorderprint(root->left);
		cout << root->data << " ";
		inorderprint(root->right);
	}
}

void stringinorderprint(treenode *root) {
	if (root!=NULL) {
		stringinorderprint(root->left);
		cout << root->stringdata << " ";
		stringinorderprint(root->right);
	}
}

int myCounter;  //keeps array position through function self calls. should be avoided by turning self calls into stacks.

void TreeToArray (treenode *root, int TheArray[]) {        //return an array containing the node labels of a tree 
	if (root !=NULL) {
		TreeToArray(root->left, TheArray);
		TheArray[myCounter]=root->data;
		myCounter++;
		TreeToArray(root->right,TheArray);
	}
}

void StringTreeToArray (treenode *root, string TheArray[]) {        //return an array containing the node labels of a tree 
	if (root !=NULL) {
		StringTreeToArray(root->left, TheArray);
		TheArray[myCounter]=root->stringdata;
		myCounter++;
		StringTreeToArray(root->right,TheArray);
	}
}

int main (int argc, char *argv[]) {
	//make sure we have enough arguments
	if (argc < 3) {
		cerr << "Please execute like " << argv[0] << " gffFiles UnusedLines outputfile" << endl;
		cerr << "If you do not give an integer for UnusedLines, you will be allowed to specify the number of unused lines in each file" << endl;
		exit(1);
	}
	
	argc-=2; //easier then fixing future instances
	string unusedstring=argv[argc];
	int inputunsed;
	int unused[argc-1];
	if (from_string<int>(inputunsed,unusedstring,std::dec)) {
		for (int i=0; i<argc; i++) {
			unused[i]=inputunsed;
		}
	}
	else {
		for (int i=0; i<argc; i++) {
			cout << "Enter number of header lines in file " << argv[i+1] << endl;
			cin >> unused[i];
		}
	}

	
	//Find size of largest gff file and declare an array large enough to hold all the data
	ifstream inFile;
	string line;
	int maxgfflines=0;
	int currentgfflines;
	for (int n=1; n<argc; n++) {
		currentgfflines=0;
		inFile.open(argv[n]);
		if (!inFile) {
			cerr << "Unable to open file " << argv[n] << endl;
			exit(1);
		}
		while (getline(inFile,line)) {
			currentgfflines++;
		}
		inFile.close();
		if (currentgfflines > maxgfflines) {
			maxgfflines = currentgfflines;
		}
	}
	string gffdata[argc-1][maxgfflines][9];
	for (int i=0; i<argc-1; i++) {
		for (int j=0; j<maxgfflines; j++) {
			for (int k=0; k<9; k++) {
				gffdata[i][j][k]="";
			}
		}
	}
	
	//Read gff files in memory
	for (int n=1; n<argc; n++) {
		int numgfflines=0;
		inFile.open(argv[n]);
		while (getline(inFile,line)) {
			numgfflines++;
		}
		numgfflines-=unused[n];
		inFile.close();
	
		string gfflines[numgfflines];
		inFile.open(argv[n]);
		if (unused[n]>0) {
			for (int i=0; i<unused[n]; i++) {
				getline(inFile,line);
			}
		}
		for (int i=0; i<numgfflines; i++) {
			getline(inFile,gfflines[i]);
		}
		inFile.close();
	
		int tablocation[10];
		int location;
		for (int i=0; i<numgfflines; i++) {
			location=1;
			tablocation[0]=-1;
			tablocation[9]=gfflines[i].length();
			for (int j=0; j<gfflines[i].length(); j++) {
				if (gfflines[i][j]=='\t') {
					tablocation[location]=j;
					location++;
				}
			}
			for (int j=0; j<9; j++) {
				gffdata[n-1][i][j]=gfflines[i].substr(tablocation[j]+1,tablocation[j+1]-tablocation[j]-1);
			}
		}
	}
	
	//Output for debugging
	/*
	for (int i=0; i<argc-1; i++) {
		for (int j=0; j<maxgfflines; j++) {
			for (int k=0; k<9; k++) {
				cout << gffdata[i][j][k] << '\t';
			}
			cout << endl;
		}
	}
	 */
	
	//initialize tree
	treenode *root;
	treenode *conductor;
	int thisposition;
	bool placement;
	root = new treenode;
	if (from_string<int>(thisposition,gffdata[0][0][3],std::dec)) {
		root->data=thisposition;
	}
	else {
		root->data=1;
		cerr << "Could not read line one from " << argv[1] << " the output will begin with an extra column of x." << endl;
	}
	root->left=NULL;
	root->right=NULL;

	//Collect transition positions
	int numtrans=1;
	for (int i=0; i<argc-1; i++) {
		for (int j=0; j<maxgfflines; j++) {
			for (int k=3; k<5; k++) {
				if (from_string<int>(thisposition,gffdata[i][j][k],std::dec)) {
					if (k==4) {
						thisposition++;
					}
					//cout << thisposition << endl;
					placement=0;
					conductor=root;
					while (placement == 0) {
						if (thisposition == conductor->data) {
							placement=1;
						}
						else {
							if (thisposition < conductor->data) {
								if (conductor -> left == NULL) {
									conductor->left=new treenode;
									numtrans++;
									conductor=conductor->left;
									conductor->data=thisposition;
									conductor->left=NULL;
									conductor->right=NULL;
									placement =1;
								}
								else {
									conductor=conductor->left;
								}
							}
							else {
								if (conductor -> right == NULL) {
									conductor->right=new treenode;
									numtrans++;
									conductor=conductor->right;
									conductor->data=thisposition;
									conductor->left=NULL;
									conductor->right=NULL;
									placement =1;
								}
								else {
									conductor=conductor->right;
								}
							}
						}
					}
				}
			}
		}
	}
	int transitions[numtrans];
	myCounter=0;
	TreeToArray(root,transitions);
	
	//re-initialize tree
	root = new treenode;
	root->stringdata=gffdata[0][0][0];
	root->left=NULL;
	root->right=NULL;
	
	//Collect transition positions
	string thislabel;
	int numsequences=1;
	int compared;
	for (int i=0; i<argc-1; i++) {
		for (int j=0; j<maxgfflines; j++) {
			thislabel=gffdata[i][j][0];
			if (thislabel != "") {
				//cout << thislabel << endl;
				placement=0;
				conductor=root;
				while (placement == 0) {
					compared = thislabel.compare(conductor->stringdata);
					if (compared == 0) {
						placement=1;
					}
					else {
						if (compared < 0) {
							if (conductor -> left == NULL) {
								conductor->left=new treenode;
								numsequences++;
								conductor=conductor->left;
								conductor->stringdata=thislabel;
								conductor->left=NULL;
								conductor->right=NULL;
								placement =1;
							}
							else {
								conductor=conductor->left;
							}
						}
						else {
							if (conductor -> right == NULL) {
								conductor->right=new treenode;
								numsequences++;
								conductor=conductor->right;
								conductor->stringdata=thislabel;
								conductor->left=NULL;
								conductor->right=NULL;
								placement =1;
							}
							else {
								conductor=conductor->right;
							}
						}
					}
				}
			}
		}
	}
	string labels[numsequences];
	myCounter=0;
	StringTreeToArray(root,labels);
	
	//output for debugging
	/*
	for (int i=0; i<numsequences; i++) {
		cout << labels[i] << " ";
	}
	cout << endl;
	for (int i=0; i<numtrans; i++) {
		cout << transitions[i] << " ";
	}
	cout << endl;
	 */
	
	//create string sequences
	string sequences[numsequences];
	bool inmrna;
	bool incds;
	int mRNA_start;
	int cds_start;
	int cds_length;
	int startfeature;
	int endfeature;
	int startframe;
	int endframe=0;
	char thisletter;
	for (int i=0; i<numsequences; i++) {
		sequences[i]="x";
		thisletter='x';
		endframe=0;
		for (int j=0; j<numtrans-1; j++) {
			inmrna=0;
			incds=0;
			for (int k=0; k<argc-1; k++) {
				for (int l=0; l<maxgfflines; l++) {
					if (gffdata[k][l][0]==labels[i]) {
						if (from_string<int>(startfeature,gffdata[k][l][3],std::dec) && from_string<int>(endfeature,gffdata[k][l][4],std::dec)) {
							if (transitions[j]>=startfeature && transitions[j]<=endfeature) {
								if (gffdata[k][l][2]=="mRNA") {
									inmrna=1;
									mRNA_start=startfeature;
								}
								if (gffdata[k][l][2]=="cds") {
									incds=1;
									cds_start=startfeature;
									cds_length=endfeature-startfeature+1;
								}
							}
						}
					}
				}
			}
			startframe=(3-endframe)%3;
			//cout << j << " " << cds_length << " " << startframe << " " << endframe << endl;
			if (inmrna && incds) {
				if (thisletter != 'e' && thisletter != 'f' && thisletter!='g') {
					switch (startframe) {
						case 0:
							thisletter='e';
							break;
						case 1:
							thisletter='g';
							break;
						case 2:
							thisletter='f';
							break;
						default:
							break;
					}
				}
				endframe=(transitions[j+1]-transitions[j]-startframe)%3;
			}
			if (inmrna && !incds) {
				if (thisletter != 'i' && thisletter != 'j' && thisletter != 'k') {
					switch (startframe) {
						case 0:
							thisletter='i';
							break;
						case 1:
							thisletter='k';
							break;
						case 2:
							thisletter='j';
							break;
						default:
							break;
					}
				}
				endframe=endframe;
			}
			if (!inmrna) {
				thisletter='x';
				endframe=0;
			}
			sequences[i]+=thisletter;
			//cout << j << " " << cds_length << " " << startframe << " " << endframe << endl;
		}
		sequences[i]+='x';
	}
	
	//create output
	ofstream outFile;
	outFile.open(argv[argc+1]);
	int maxlabellength=0;
	for (int i=0; i<numsequences; i++) {
		if (labels[i].length()>maxlabellength) {
			maxlabellength=labels[i].length();
		}
	}
	int labelcolumns=max(10,maxlabellength+4);
	for (int i=0; i<numsequences; i++) {
		outFile << labels[i];
		for (int j=0; j<labelcolumns-labels[i].length(); j++) {
			outFile << " ";
		}
		outFile << sequences[i] << endl;
	}
	
	return 0;
}