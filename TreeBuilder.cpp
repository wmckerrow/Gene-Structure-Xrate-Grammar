#define MAXCHILDREN 10
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <math.h>
using namespace std;

struct TreeNode {
	TreeNode *children[MAXCHILDREN];      //array of pointers to the children
	string NodeLabel;                     //Node label for .stk file generation
};

string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
string numbers = "1234567890";

string convertInt(int number)
{
	stringstream ss;//create a stringstream
	ss << number;//add number to the stream
	return ss.str();//return a string with the contents of the stream
}

int TreeSize(TreeNode *root) {      //find the number of descendants of a node
	int thissize=1;
	if (root->children[0]==NULL) {
		thissize=0;
	}
	for (int i=0; i<MAXCHILDREN; i++) {
		if (root->children[i] != NULL) {
			thissize+=TreeSize(root->children[i]);
		}
	}
	return thissize;
}

void FullPrintTree (TreeNode *root) {   //print locations of nodes and children in memory. for debugging only
	if (root != NULL) {
		cout << "For node at " << root << endl;
		cout << "The children are at ";
		for (int i=0; i<MAXCHILDREN; i++) {
			cout << root->children[i] << " ";
		}
		cout << endl;
		cout << "and the node label is " << root->NodeLabel << endl;
		for (int i=0;i<MAXCHILDREN;i++) {
			FullPrintTree( root->children[i] );
		}
	}
}

int myCounter=0;

void *addevidence (TreeNode *root, int numevidence) {
	TreeNode *conductor;
	if (root->children[0] == NULL) {
		for (int i=0; i<numevidence; i++) {
			root->children[i]=new TreeNode;
			conductor=root->children[i];
			conductor->NodeLabel=letters[myCounter]+convertInt(i+1);
			for (int j=0; j<MAXCHILDREN; j++) {
				conductor->children[j]=NULL;
			}
		}
		myCounter++;
	}
	else {
		addevidence(root->children[0],numevidence);
		addevidence(root->children[1],numevidence);
	}
	return root;
}

void LabelTree(TreeNode *root) {
	TreeNode *child0;
	TreeNode *child1;
	if (root->NodeLabel=="unlabeled") {
		child0=root->children[0];
		child1=root->children[1];
		LabelTree(child0);
		LabelTree(child1);
		if (child0->NodeLabel[1]=='1') {
			root->NodeLabel=child0->NodeLabel[0];
		}
		else {
			root->NodeLabel=child0->NodeLabel + child1->NodeLabel;
		}

	}
}

float calculatebalance(TreeNode *root) {
	float balancehere=0;
	float balancetotal=0;
	if (root->children[0] != NULL) {
		float rightsize=TreeSize(root->children[0]);
		float leftsize=TreeSize(root->children[1]);
		if (rightsize==0) {
			balancehere=0;
		}
		else {
			if (rightsize<leftsize) {
				balancehere=1.0-rightsize/leftsize;
			}
			else {
				balancehere=1.0-leftsize/rightsize;
			}
			balancetotal=balancehere+calculatebalance(root->children[0])+calculatebalance(root->children[1]);
		}
	}
	return balancetotal;
}

string generateNewick(TreeNode *current) {
	string Newick;
	if (current->children[0]!=NULL) {
		Newick="(";
		for (int i=0; i<MAXCHILDREN; i++) {
			if (current->children[i] != NULL) {
				Newick+=generateNewick(current->children[i]);
				if (i+1<MAXCHILDREN && current->children[i+1]!=NULL) {
					Newick+=",";
				}
			}
		}
		Newick+=")";
		Newick+=current->NodeLabel;
	}
	else {
		Newick=current->NodeLabel;
	}
	Newick+=":.1";
	return Newick;
}


int main (int argc, char *argv[]) {
	srand((unsigned)time(0));
	float r;
	
	if (argc < 5) {
		cerr << "Please execute like ./TreeBuilder numspecies numevidence multiplier1 multiplier2 input.gene" << endl << "or ./TreeBuilder numspecies numevidence buildparameter input.gene" << endl;

		exit(1);
	}
	
	float multiplier1;
	float multiplier2;
	float buildparameter;
	int inputdotgene;
	
	if (argc==5) {
		buildparameter=atof(argv[3]);
		if (buildparameter <= 0 || buildparameter >= 1) {
			cerr << "Must have 0<buildparameter<1" << endl;
			exit(1);
		}
		multiplier1=1.0/buildparameter-1.0;
		multiplier2=1.0-4.0*(buildparameter-0.5)*(buildparameter-0.5);
		inputdotgene=4;
	}
	else {
		multiplier1=atof(argv[3]);
		multiplier2=atof(argv[4]);
		inputdotgene=5;
	}

	
	ifstream inFile;
	inFile.open("generator.cpp");
	string temp;
	int generatormaxchildren=0;
	int generatormaxdepth=0;
	bool gotgenerator;
	if (!inFile) {
		cerr << "Warning: unable to check MAXCHILDREN and MAXDEPTH in generator.cpp." << endl;
		gotgenerator=0;
	}
	else {
		inFile >> temp >> temp >> generatormaxchildren >> temp >> temp >> generatormaxdepth;
		gotgenerator=1;
	}
	inFile.close();
	
	int numspecies = atoi(argv[1]);
	int numnodes = 2*numspecies-1;
	
	if (numspecies > 52) {
		cerr << "Warning: With more than 52 species, nodes will not labeled properly." << endl;
	}
	int numevidence = atoi(argv[2]);
	if (numevidence > generatormaxchildren && gotgenerator == 1) {
		cout << "You have asked for " << numevidence << " pieces of evidence, but Generator only allows for " << generatormaxchildren << ". Change #define in generator.cpp and recompile." << endl;
	}
	if (numevidence > MAXCHILDREN) {
		cerr << "Please update #define in TreeBuilder.cpp to allow for more evidence." << endl;
		exit(1);
	}
	
	TreeNode interiornodes[numnodes];
	for (int i=0; i<numnodes; i++) {
		for (int j=0; j<MAXCHILDREN; j++) {
			interiornodes[i].children[j]=NULL;
		}
		interiornodes[i].NodeLabel="unlabeled";
	}
	
	TreeNode *root=&interiornodes[numnodes-1];
	
	int heights[numnodes];
	for (int i=0; i<numnodes; i++) {
		if (i<numspecies) {
			heights[i]=1;
		}
		else {
			heights[i]=0;
		}
	}
	
	float nodeweights1[numnodes];
	float nodeweights2[numnodes];
	bool buried[numnodes];
	for (int i=0; i<numnodes; i++) {
		buried[i]=0;
	}
	float totalweight1;
	float totalweight2;
	int firstnode;
	int secondnode;
	int counter=numspecies;
	
	while (heights[numnodes-1]==0) {
		totalweight1=0;
		totalweight2=0;
		for (int i=0; i<numnodes; i++) {
			if (heights[i]==0 || buried[i]) {
				nodeweights1[i]=0;
				nodeweights2[i]=0;
			}
			else {
				nodeweights1[i]=1;
				nodeweights2[i]=1;
				for (int j=1; j<heights[i]; j++) {
					nodeweights1[i]*=multiplier1;
					nodeweights2[i]*=multiplier2;
				}
				totalweight1+=nodeweights1[i];
				totalweight2+=nodeweights2[i];
			}
		}
		
		r = (float)rand()/(float)RAND_MAX;
		r*=totalweight1;
		firstnode=0;
		while (r>nodeweights1[firstnode]) {
			r-=nodeweights1[firstnode];
			firstnode++;
		}
		totalweight2-=nodeweights2[firstnode];
		nodeweights2[firstnode]=0;
		buried[firstnode]=1;
		
		r = (float)rand()/(float)RAND_MAX;
		r*=totalweight2;
		secondnode=0;
		while (r>nodeweights2[secondnode]) {
			r-=nodeweights2[secondnode];
			secondnode++;
		}
		buried[secondnode]=1;
		
		interiornodes[counter].children[0]=&interiornodes[firstnode];
		interiornodes[counter].children[1]=&interiornodes[secondnode];
		
		if (heights[firstnode]>heights[secondnode]) {
			heights[counter]=heights[firstnode]+1;
		}
		else {
			heights[counter]=heights[secondnode]+1;
		}

		counter++;
	}
	
	//FullPrintTree(root);
	
	int maxheight=0;
	for (int i=0; i<numnodes; i++) {
		if (heights[i]>maxheight) {
			maxheight=heights[i];
		}
	}
	
	ifstream gettreemetrics;
	ofstream rewritetreemetrics;
	string line;
	
	gettreemetrics.open("treemetrics");
	int treemetriclines=0;
	if (gettreemetrics) {
		while (getline(gettreemetrics,line)) {
			treemetriclines++;
		}
	}
	gettreemetrics.close();
	
	string treemetricfile[treemetriclines];
	gettreemetrics.open("treemetrics");
	if (gettreemetrics) {
		for (int i=0; i<treemetriclines; i++) {
			getline(gettreemetrics,treemetricfile[i]);
		}
	}
	gettreemetrics.close();	
	
	float unscaled=(float)maxheight/numnodes;
	float minratio=(ceil(log((float)numspecies)/log(2.0))+1)/numnodes;
	float maxratio=(float)numspecies/numnodes;
	float scaled=1.0-(unscaled-minratio)/(maxratio-minratio);
	
	rewritetreemetrics.open("treemetrics");
	for (int i=0; i<treemetriclines; i++) {
		rewritetreemetrics << treemetricfile[i] << endl;
	}
	rewritetreemetrics << unscaled << " " << scaled << endl;
	
	addevidence(root,numevidence);
	LabelTree(root);
	
	string Newick = generateNewick(root);
	Newick = Newick.substr(0,Newick.size()-3);
	cout << Newick << endl;
	
	ifstream getinput;
	ofstream rewrite;
	
	getinput.open(argv[inputdotgene]);
	int inputlines=0;
	if (getinput) {
		while (getline(getinput,line)) {
			inputlines++;
		}
	}
	else {
		cerr << "Couldn't open " << argv[inputdotgene] << endl;
		exit(1);
	}

	string inputfile[inputlines];
	getinput.close();
	getinput.open(argv[inputdotgene]);
	if (getinput) {
		for (int i=0; i<inputlines; i++) {
			getline(getinput,inputfile[i]);
		}
	}
	getinput.close();
	
	rewrite.open(argv[inputdotgene]);
	rewrite << Newick << endl;
	for (int i=1; i<inputlines; i++) {
		rewrite << inputfile[i] << endl;
	}
	
	return 0;
}