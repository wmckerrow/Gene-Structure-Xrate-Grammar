#define MAXCHILDREN 4
#define MAXDEPTH 4
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

//The node of a tree. It contains a single character label and pointers to its daughter nodes.
struct TreeNode {
	char x;			                      //the node label
	TreeNode *children[MAXCHILDREN];      //array of pointers to the children
	string NodeLabel;                     //Node label for .stk file generation
	bool leaf;						      //1 for leaf, 0 otherwise
};

//Element of the linked list of trees contains a pointer to the root node of a tree
//and a pointer to the next element of the list.
struct TreeList {
	TreeNode *tree;        //the tree at this element
	TreeList *next;        //the next tree
};

void PrintTree (TreeNode *root) {  //preorder print the tree. only used for debugging.
	if (root != NULL) {
		cout << root->x << " ";
		for (int i=0;i<MAXCHILDREN;i++) {
			PrintTree( root->children[i] );
		}
	}
}

void FullPrintTree (TreeNode *root) {   //print locations of nodes and children in memory. for debugging only
	if (root != NULL) {
		cout << "For node at " << root << endl;
		cout << "The children are at ";
		for (int i=0; i<MAXCHILDREN; i++) {
			cout << root->children[i] << " ";
		}
		cout << endl;
		cout << "and the label is " << root->x << endl;
		for (int i=0;i<MAXCHILDREN;i++) {
			FullPrintTree( root->children[i] );
		}
	}
}

int TreeSize(TreeNode *root) {      //find the number of descendants of a node
	int thissize=1;
	for (int i=0; i<MAXCHILDREN; i++) {
		if (root->children[i] != NULL) {
			thissize+=TreeSize(root->children[i]);
		}
	}
	return thissize;
}

int myCounter = 0;  //keeps array position through function self calls. should be avoided by turning self calls into stacks.

char *TreeToArray (TreeNode *root, char TheArray[]) {        //return an array containing the node labels of a tree 
	char *TheArrayPtr;
	TheArrayPtr=TheArray;
	if (root !=NULL) {
		TheArray[myCounter]=root->x;
		myCounter++;
		for (int i=0;i<MAXCHILDREN;i++) {
			TreeToArray(root->children[i],TheArray);
		}
	}
	return TheArrayPtr;
}

void *NodeLocationsArray (TreeNode *root, int TheArray[], int depth, int ParentLocation[MAXDEPTH], int ParentDirection) {  //creates an array with the location of all nodes in a tree.
	if (root != NULL) {
		int ThisLocation[MAXDEPTH];       //This Array will contain the location of the current node
		if (depth == 0) {     //if we're at the root, give it root location
			for (int i=0; i<MAXDEPTH; i++) {
				ThisLocation[i]=-1;
			}
		}
		else {                //otherwise add the last step to the parent location to get the location of the current node
			for (int i=0; i<MAXDEPTH; i++) {
				ThisLocation[i] = ParentLocation[i];
			}
			ThisLocation[depth-1] = ParentDirection;
		}
		for (int i=0; i<MAXDEPTH; i++) {
			TheArray[myCounter*MAXDEPTH+i]=ThisLocation[i];  //dump ThisLocation into the larger array
		}
		myCounter++;
		for (int i=0; i<MAXCHILDREN; i++) {
			NodeLocationsArray(root->children[i],TheArray,depth+1,ThisLocation,i);  //find locations of children
		}
	}
}	

TreeNode *ParseTree (string Newick) {    //generates a new tree from a string in newick format
	int lastclose=Newick.size()-1;
	while (Newick[lastclose] != ')' && lastclose>=0) {
		lastclose--;
	}
	string labelandlength=Newick.substr(lastclose+1,Newick.size()-lastclose);      //everything after the last close parenthesis is the label for the root and length of the branch to the root
	int colon=0;
	while (labelandlength[colon] != ':' && colon < labelandlength.size()) {
		colon++;
	}
	string justlabel = labelandlength.substr(0,colon);      //ignore the colon and everything after. it is the branch lengths. branch lengths are not yet used.
	Newick=Newick.substr(0,lastclose+1);             //everything up to and including the last close parenthis describes the rest of the tree
	TreeNode *newtree;
	newtree=new TreeNode;               //start by creating the root node
	newtree->NodeLabel=justlabel;       //and labeling it
	int startpos;
	if (Newick[0]=='(') {         //the information about children is within open and closed parentheses. we ignore the outer parentheses
		startpos=1;
	}
	else {
		startpos=0;
	}
	int unclosed=0;
	int zerocommas[MAXCHILDREN+1];     //the children will be seperated by commas that occur when parentheses are balanced
	for (int i=0; i<MAXCHILDREN+1; i++) {
		zerocommas[i]=0;    //make sure we don't reference an array element without a value
	}
	zerocommas[0]=startpos-1;     //the first child starts at the beginning
	int commanum=1;
	string tempstring;
	for (int i=startpos; i<=Newick.size()-startpos; i++) {   //this loop finds the depth 0 commas
		if (Newick[i]=='(') {
			unclosed++;
		}
		if (Newick[i]==')') {
			unclosed--;
		}
		if (unclosed==0 && Newick[i]==',') {
			zerocommas[commanum]=i;
			commanum++;
		}
	}
	zerocommas[commanum]=Newick.size()-startpos;     //last child ends at the end of the string
	if (zerocommas[2] != 0) {
		newtree->leaf=0;     //if we write something in zerocommas[2] there must be at least one child
		for (int i=0; i<MAXCHILDREN; i++) {
			if (zerocommas[i+1] != 0) {    //loop through all the depth 0 commas
				tempstring=Newick.substr(zerocommas[i]+1,zerocommas[i+1]-zerocommas[i]-1);    //and pull out the child strings
				//cout << Newick << "->" << tempstring << endl;
				newtree->children[i]=ParseTree(tempstring);       //create a tree with the child string and attach in the proper place
			}
			else {
				newtree->children[i]=NULL;    //if there are less then MAXCHILDREN children set the rest of the pointers to NULL
			}
		}
	}
	else {       //if there are no children, we mark the node as a leaf and set all its children pointers to NULL
		newtree->leaf=1;
		for (int i=0; i<MAXCHILDREN; i++) {
			newtree->children[i]=NULL;
		}
	}
	
	return newtree;
}

TreeNode *CopyTree(TreeNode *oldtree, TreeNode *newtree) {  //returns a pointer to an exact copy of a preexisting tree
	TreeNode *temptree;
	newtree = new TreeNode;
	newtree->x=oldtree->x;
	for (int i=0; i<MAXCHILDREN; i++) {
		if (oldtree->children[i] != NULL) {
			temptree = CopyTree(oldtree->children[i],newtree->children[i]);
			newtree->children[i] = temptree;
		}
		else {
			newtree->children[i] = NULL;
		}
	}
	return newtree;
}

void RelabelTree(TreeNode *tree, char newlabel) {    //change the label of all descendant of a given node
	tree->x=newlabel;
	for (int i=0; i<MAXCHILDREN; i++) {
		if (tree->children[i] != NULL) {
			RelabelTree(tree->children[i],newlabel);
		}
	}
}

void MutateTree (TreeNode *mutatefrom, int mutateat[MAXDEPTH], char mutateto) {    //relabel all descendant of a node given a root and the path to the node
	int i=0;
	TreeNode *mutatehere;
	mutatehere=mutatefrom;
	while (mutateat[i] != -1) {
		if (mutatehere->children[mutateat[i]] != NULL) {
			mutatehere=mutatehere->children[mutateat[i]];
		}
		else {
			cout << "Warning: Trying to mutate part of the tree that doesn't exist." << endl;
		}
		i++;
	}
	RelabelTree (mutatehere, mutateto);
}

void AddTree (TreeNode *newtree, TreeList *previoustree) {   //insert a tree into the linked list of trees
	TreeList *newlistitem;
	newlistitem = new TreeList;
	newlistitem->tree=newtree;
	newlistitem->next=previoustree->next;
	previoustree->next=newlistitem;
}

char FindLabel (TreeNode *tree, int local[MAXDEPTH]) {      //find the label of a node given a pointer to its root and a path
	int i=0;
	while (local[i] != -1) {
		if (tree->children[local[i]] != NULL) {
			tree=tree->children[local[i]];
		}
		else {
			cerr << "Warning: Trying to find a label that doesn't exist." << endl;
		}
		i++;
	}
	return tree->x;
}

bool FindLeaf (TreeNode *tree, int local[MAXDEPTH]) {      //find out whether a node is a leaf
	int i=0;
	while (local[i] != -1) {
		if (tree->children[local[i]] != NULL) {
			tree=tree->children[local[i]];
		}
		else {
			cerr << "Warning: Trying to find leaf status of a node that doesn't exist." << endl;
		}
		i++;
	}
	return tree->leaf;
}

bool FindLeafParent (TreeNode *tree, int local[MAXDEPTH]) {     //find out if any of the children of a node are leaves
	int i=0;
	while (local[i] != -1) {
		if (tree->children[local[i]] != NULL) {
			tree=tree->children[local[i]];
		}
		else {
			cerr << "Warning: Trying to find leafparent status of a node that doesn't exist." << endl;
		}
		i++;
	}
	bool leafparent=0;
	TreeNode *child;
	for (int k=0; k<MAXCHILDREN; k++) {
		if (tree->children[k] != NULL) {
			child = tree->children[k];
			if (child->leaf == 1) {
				leafparent=1;
			}
		}
	}
	return leafparent;
}

TreeNode *MakeTree(char label, TreeNode *templatetree) {  //returns the location of a new tree with nodes labeled label.
	TreeNode *newtree;
	newtree = CopyTree(templatetree,newtree);
	RelabelTree(newtree,label);
	return newtree;
}

void *NodeLabelToArray (TreeNode *root, string TheArray[]) {        //return an array containing the node labels of a tree 
	if (root !=NULL) {
		TheArray[myCounter]=root->NodeLabel;
		myCounter++;
		for (int i=0;i<MAXCHILDREN;i++) {
			NodeLabelToArray(root->children[i],TheArray);
		}
	}
}

int main(int argc, char *argv[])
{
	//SET UP STEP
	srand((unsigned)time(0)); //seed random number generator. Took a vacation...
	//srand(1234345234);
	if (argc < 3) {
		cerr << "Please use Generator inputfile outputfile" << endl;
		exit(1);
	}
	ifstream inFile;
	if (argc > 3) {  //if we are running this many times in succession we cannot seed from the clock since it will run more than once in a single second
		if (argv[4]!="randtime") {
			inFile.open("randint");
			if (!inFile) {
				cerr << "Cannot read file randint." << endl;
			}
			else {
				int randint;
				inFile >> randint;
				srand((unsigned)randint);
			}
		}
	}
	inFile.close();
	
	inFile.open(argv[1]);  //count the number of lines in the input file until "end" and subtract 3 to get the number of lines of parameters
	string line="start";
	int parameterlines=-3;
	while (line != "end") {
		parameterlines++;
		getline(inFile,line);
	}
	inFile.close();
	
	inFile.open(argv[1]);   //open the input file to read data
	string Newick;
	if (!inFile) {
		cerr << "Unable to open input file " << argv[1] << "." << endl;
		exit(1);
	}
	inFile >> Newick;    //get the tree string
	float EtoI,XtoEnd;
	inFile >> EtoI >> XtoEnd;    //get the Markov parameters
	float xetoxx[parameterlines],xetoee[parameterlines],xtoxex[parameterlines];
	float extoxx[parameterlines],extoee[parameterlines],eitoii[parameterlines],eitoee[parameterlines],etoexe[parameterlines],etoeie[parameterlines];
	float ietoii[parameterlines],ietoee[parameterlines],itoiei[parameterlines];
	for (int i=0; i<parameterlines; i++) {    //get each line of mutation parameters
		inFile>>xetoxx[i]>>xetoee[i]>>xtoxex[i]>>extoxx[i]>>extoee[i]>>eitoii[i]>>eitoee[i]>>etoexe[i]>>etoeie[i]>>ietoii[i]>>ietoee[i]>>itoiei[i];
	}
	inFile.close();
	
	TreeNode *mytree;         //make a temporary tree to find the size and node locations for that tree
	TreeNode *testtree;
	mytree = ParseTree(Newick);
	//FullPrintTree(mytree);
	//cout << "done print" << endl;
		
	int NumNodes;
	NumNodes = TreeSize(mytree);  //we need to know how many nodes in tree so we know how much memory to set aside for their loactions.
	int TempNodeLocations[NumNodes*MAXDEPTH];
	int NodeLocations[NumNodes][MAXDEPTH];
	myCounter = 0;
	NodeLocationsArray(mytree,TempNodeLocations,0,NULL,NULL);  //find locations of all nodes in the tree. need these for mutation step.
	for (int i=0; i<NumNodes; i++) {
		for (int j=0; j<MAXDEPTH; j++) {
			NodeLocations[i][j]=TempNodeLocations[i*MAXDEPTH+j];
		}
	}
	
	string NodeLabelArray[NumNodes];   //we'll need the node labels to make the output easy to understand
	myCounter = 0;
	NodeLabelToArray(mytree,NodeLabelArray);
	
	int NumLeaves = 0;    //count the number of leaves
	for (int i=0; i<NumNodes; i++) {
		if (FindLeaf(mytree,NodeLocations[i])) {
			NumLeaves++;
		}
	}
	
	int NumLeafParents=0;    //count the number of nodes which are parents of leaves
	for (int i=0; i<NumNodes; i++) {
		if (FindLeafParent(mytree,NodeLocations[i])) {
			NumLeafParents++;
		}
	}
	
	int ratetype=-1;     //used the number of parameters to find out which rate type is desired
	if (parameterlines < 1) {
		cerr << "Need paraters to run.";
		exit(1);
	}
	if (parameterlines == 1) {
		ratetype=0;
	}
	if (parameterlines == 2) {
		ratetype=1;
	}
	if (parameterlines == NumLeafParents +1) {
		ratetype=2;
	}
	if (parameterlines == NumLeaves +1) {
		ratetype=3;
	}
	if (ratetype==-1) {
		cerr << "Found " << parameterlines << "parameter lines for " << NumLeaves << " leaves and " << NumLeafParents << " leaf parents. Using constant mutation rates." << endl;
		ratetype=0;   //default is all nodes have same mutation rate
	}
	
	TreeList *llroot;         //the root of the linked list of trees
	TreeList *llconductor;    //a pointer that moves along the list "doing stuff"
	llroot = new TreeList;
	llroot->next = NULL;
	llconductor = llroot;

	//MARKOV CHAIN STEP
	int chainleng = 0;       //this variable will count the number of emisions in the markov chain
	float r;                 //use this for random number generator
	int state = 0;           //markov state
	while (state != 4) {     //The Markov Chain step. state 0 is start, 1 is x, 2 is e, 3 is i and 4 is stop.
		switch (state) {
			case 0:              //From start we go straight to x
				state=1;
				break;
			case 1:              //From x we emit a tree with x labels and go to e or end.
				//cout << "x";
				chainleng++;
				llconductor->next=new TreeList;
				llconductor=llconductor->next;
				llconductor->tree=MakeTree('x',mytree);
				llconductor->next=NULL;
				r = (float)rand()/(float)RAND_MAX;
				//cout << r << endl;
				if (r < XtoEnd) {
					state = 4;
				}
				else {
					state = 2;
				}
				break;
			case 2:                //From state e we emit a tree with e labels and go to x or i.
				//cout << "e";
				chainleng++;
				llconductor->next=new TreeList;
				llconductor=llconductor->next;
				llconductor->tree=MakeTree('e',mytree);
				llconductor->next=NULL;
				r = (float)rand()/(float)RAND_MAX;
				if (r < EtoI) {
					state = 3;
				}
				else {
					state = 1;
				}
				break;
			case 3:                 //From state i we emit a tree with i labels and go to state e.
				//cout << "i";
				chainleng++;
				llconductor->next=new TreeList;
				llconductor=llconductor->next;
				llconductor->tree=MakeTree('i',mytree);
				llconductor->next=NULL;
				state = 2;
				break;
		}
	}
	//  cout << endl;
	
	
	TreeList *llconductornext;
	TreeNode *temptree1;
	TreeNode *temptree2;
	char ThisLabel;
	char NextLabel;
	
	//MUTATION STEP
	int ratenumber=0;   //which parameter line we are using
	int	leafparentnum=0;   //how many leaf parents we have mutated (+1 if we are in the process of mutating a leaf parent)
	int leafnumber=0;   //how many leaves we have mutated (+1 if we are in the process of mutating a leaf)
	for (int i=1; i<NumNodes; i++) {   //loop through the nodes, mutating them all
		llconductor=llroot->next;
		llconductornext=llconductor->next;
		
		switch (ratetype) {   //which parameter line will be used depends on the rate type
			case 0:      //in case 0, all rates are equal
				ratenumber=0;
				break;
			case 1:    //in case 1, we use one line for leaves and the other for the rest of the tree
				if (FindLeaf(mytree,NodeLocations[i])) {
					ratenumber=1;
				}
				else {
					ratenumber=0;
				}
				break;
			case 2:    //in case 2, we use one line for non leaf nodes. for leaves we use a line according to which node is their parent
				if (FindLeafParent(mytree,NodeLocations[i])) {
					leafparentnum++;
				}
				if (FindLeaf(mytree,NodeLocations[i])) {
					ratenumber=leafparentnum;
				}
				else {
					ratenumber=0;
				}

				break;
			case 3:     //in case three we use one line for non leaf nodes and a different line for each leaf
				if (FindLeaf(mytree,NodeLocations[i])) {
					leafnumber++;
					ratenumber=leafnumber;
				}
				else {
					ratenumber=0;
				}
				break;
			default:
				break;
		}
		
		//cout << "rate number is " << ratenumber << " at node " << NodeLabelArray[i] << endl;
		
		while (llconductornext != NULL) {  //move down the linked list until we get to the end
			ThisLabel=FindLabel(llconductor->tree,NodeLocations[i]);
			NextLabel=FindLabel(llconductornext->tree,NodeLocations[i]);
			r = (float)rand()/(float)RAND_MAX;   //random number between 0 and 1;
			switch (ThisLabel) {
				case 'x':
					if (r<xtoxex[ratenumber]) {  //the first possible mutation is insertion of an exon into an intergenic sequence
						temptree1=CopyTree(llconductor->tree,temptree2);
						AddTree(temptree1,llconductor);
						chainleng++;
						temptree1=CopyTree(llconductor->tree,temptree2);
						MutateTree(temptree1,NodeLocations[i],'e');
						AddTree(temptree1,llconductor);
						chainleng++;
					}
					if (NextLabel=='e') {  //if it is followed by an exon then we can shorten or lengthen the exon
						if (r>=xtoxex[ratenumber] && r<xtoxex[ratenumber]+xetoxx[ratenumber]) {
							temptree1 = CopyTree(llconductornext->tree,temptree2);
							MutateTree(temptree1,NodeLocations[i],'x');
							AddTree(temptree1,llconductor);
							chainleng++;
						}
						if (r>=xtoxex[ratenumber]+xetoxx[ratenumber] && r<xtoxex[ratenumber]+xetoxx[ratenumber]+xetoee[ratenumber]) {
							temptree1 = CopyTree(llconductor->tree,temptree2);
							MutateTree(temptree1,NodeLocations[i],'e');
							AddTree(temptree1,llconductor);
							chainleng++;							
						}
					}
					break;
				case 'e':
					if (r<etoeie[ratenumber]) {    //we can insert an intron
						temptree1=CopyTree(llconductor->tree,temptree2);
						AddTree(temptree1,llconductor);
						chainleng++;
						temptree1=CopyTree(llconductor->tree,temptree2);
						MutateTree(temptree1,NodeLocations[i],'i');
						AddTree(temptree1,llconductor);
						chainleng++;
					}
					if (r>=etoeie[ratenumber] && r<etoeie[ratenumber]+etoexe[ratenumber]) {   //or we can insert an intergenic sequence
						temptree1=CopyTree(llconductor->tree,temptree2);
						AddTree(temptree1,llconductor);
						chainleng++;
						temptree1=CopyTree(llconductor->tree,temptree2);
						MutateTree(temptree1,NodeLocations[i],'x');
						AddTree(temptree1,llconductor);
						chainleng++;
					}
					if (NextLabel=='i') {   //if it followed by an intron then we can extend the exon into the intron of extend the intron into the exon
						if (r>=etoeie[ratenumber]+etoexe[ratenumber] && r<etoeie[ratenumber]+etoexe[ratenumber]+eitoee[ratenumber]) {
							temptree1 = CopyTree(llconductornext->tree,temptree2);
							MutateTree(temptree1,NodeLocations[i],'e');
							AddTree(temptree1,llconductor);
							chainleng++;
						}
						if (r>=etoeie[ratenumber]+etoexe[ratenumber]+eitoee[ratenumber] && r<etoeie[ratenumber]+etoexe[ratenumber]+eitoee[ratenumber]+eitoii[ratenumber]) {
							temptree1 = CopyTree(llconductor->tree,temptree2);
							MutateTree(temptree1,NodeLocations[i],'i');
							AddTree(temptree1,llconductor);
							chainleng++;
						}
					}
					if (NextLabel=='x') {   //similarly we can shorted or lengthen an exon into an intergenic sequence
						if (r>=etoeie[ratenumber]+etoexe[ratenumber] && r<etoeie[ratenumber]+etoexe[ratenumber]+extoee[ratenumber]) {
							temptree1 = CopyTree(llconductornext->tree,temptree2);
							MutateTree(temptree1,NodeLocations[i],'e');
							AddTree(temptree1,llconductor);
							chainleng++;
						}
						if (r>=etoeie[ratenumber]+etoexe[ratenumber]+extoee[ratenumber] && r<etoeie[ratenumber]+etoexe[ratenumber]+extoee[ratenumber]+extoxx[ratenumber]) {
							temptree1 = CopyTree(llconductor->tree,temptree2);
							MutateTree(temptree1,NodeLocations[i],'x');
							AddTree(temptree1,llconductor);
							chainleng++;
						}
					}
					break;
				case 'i':
					if (r<itoiei[ratenumber]) {   //insert an exon into an intron
						temptree1=CopyTree(llconductor->tree,temptree2);
						AddTree(temptree1,llconductor);
						chainleng++;
						temptree1=CopyTree(llconductor->tree,temptree2);
						MutateTree(temptree1,NodeLocations[i],'e');
						AddTree(temptree1,llconductor);
						chainleng++;
					}
					if (NextLabel=='e') {   //if an exon sequence we can shorten or lengthen the intron into the exon.
						if (r>=itoiei[ratenumber] && r<itoiei[ratenumber]+ietoii[ratenumber]) {
							temptree1 = CopyTree(llconductornext->tree,temptree2);
							MutateTree(temptree1,NodeLocations[i],'i');
							AddTree(temptree1,llconductor);
							chainleng++;
						}
						if (r>=itoiei[ratenumber]+ietoii[ratenumber] && r<itoiei[ratenumber]+ietoii[ratenumber]+ietoee[ratenumber]) {
							temptree1 = CopyTree(llconductor->tree,temptree2);
							MutateTree(temptree1,NodeLocations[i],'e');
							AddTree(temptree1,llconductor);
							chainleng++;							
						}
					}
					break;
				default:
					break;
			}
			llconductor=llconductornext;
			llconductornext=llconductornext->next;
		}
	}	
	
	//COLLECT DATA STEP
	char data[NumNodes][chainleng];       //This array will hold the node labels for all trees in the linked list.
	char TempArray[NumNodes];             //TreeToArray fn needs an array to store data in before it returns it.
	char *TreeArray;                      //This holds the output of TreeToArray until it is put into data.
	
	
	llconductor=llroot->next;
	
	int i=0;
	if ( llconductor != NULL ) {          //Move down the linked list dumping node labels into data matrix.
		while ( llconductor->next != NULL ) {
			myCounter=0;
			//     PrintTree(llconductor->tree);
			//     cout << endl;
			TreeArray=TreeToArray(llconductor->tree,TempArray);
			for (int j=0;j<NumNodes;j++) {
				data[j][i]=TreeArray[j];
			}
			llconductor = llconductor->next;
			i++;
		}
		myCounter=0;
		TreeArray=TreeToArray(llconductor->tree,TempArray);
		for (int j=0;j<NumNodes;j++) {
			data[j][i]=TreeArray[j];
		}
		i++;
		//    PrintTree(llconductor->tree);
		//    cout << endl;
	}
	
	for (int j=0;j<NumNodes;j++) {  //return the node label together with the sequence at that node
		cout << NodeLabelArray[j];
		for (int k=0; k<10-NodeLabelArray[j].size(); k++) {
			cout << " ";
		}
		for (i=0;i<chainleng;i++) {
			cout << data[j][i];
		}
		cout << endl;
	}
	
	string response;  //make sure we don't accidently delete something important
	ifstream testFile;
	testFile.open (argv[2]);
	if (testFile) {
		cerr << argv[2] << " already exists. Okay to overwrite? (type yes to continue) ";
		cin >> response;
		if (response != "yes") {
			exit(1);
		}
	}
	
	ofstream outFile;    //create a .stk file to test xrate
	outFile.open (argv[2]);
	outFile << "#=GF NH " << Newick << ";" << endl << endl;
	for (int j=0;j<NumNodes;j++) {
		if (FindLeaf(mytree,NodeLocations[j]) == 1) {
			outFile << NodeLabelArray[j];
			for (int k=0; k<10-NodeLabelArray[j].size(); k++) {
				outFile << " ";
			}
			for (i=0;i<chainleng;i++) {
				outFile << data[j][i];
			}
			outFile << endl;
		}
	}
	outFile.close();
	
	outFile.open("randint");
	outFile << rand();
	outFile.close();
}
