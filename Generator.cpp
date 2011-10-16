#define MAXCHILDREN 4
#define MAXDEPTH 4
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

//PARAMETERS. These will be read from file soon.
float EtoI=0.2;     //Probability of going from state E to state I. Otherwise we go from state E to state X.
float XtoEnd=0.1;   //Probability of ending any given time we are in state X. Otherwise we go to state E.
int BranchLength = 1;    //How many opportunites there are for mutation before reaching next node. Not used

float xetoxx=0.1;   //Prob of shortening an exon preceeded by intergenic
float xetoee=0.1;   //Prob of lengthening an exon preceeded by intergenic
float xtoxex=0.05;  //Prob of inserting an exon into an intergenic sequence
float extoxx=0.05;  //etc
float extoee=0.05;
float eitoii=0.05;
float eitoee=0.05;
float etoexe=0.025;
float etoeie=0.025;
float ietoii=0.1;
float ietoee=0.1;
float itoiei=0.05;

//The node of a tree. It contains a single character label and pointers to its daughter nodes.
struct TreeNode {
	char x;			                      //the node label
	TreeNode *children[MAXCHILDREN];      //array of pointers to the children
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

TreeNode *ParseTree (string Newick) {   //build a tree from a Newick tree string
	TreeNode *newtree;
	newtree=new TreeNode;
	int startpos;
	if (Newick[0]=='(') { //if the entire string is enclosed in parens we want to ignore them
		startpos=1;
	}
	else {
		startpos=0;
	}
	int unclosed=0;
	int zerocommas[MAXCHILDREN+1]; //we find commas at 0 parenthesis depth. these will divide string into the children of the root node
	for (int i=0; i<MAXCHILDREN+1; i++) {
		zerocommas[i]=0;
	}
	zerocommas[0]=startpos-1; //where the first child starts
	int commanum=1;
	string tempstring;
	for (int i=startpos; i<=Newick.size()-startpos; i++) {  //count # '(' minus # ')' to get parens depth
		if (Newick[i]=='(') {
			unclosed++;
		}
		if (Newick[i]==')') {
			unclosed--;
		}
		if (unclosed==0 && Newick[i]==',') {  //make node of where there are ','s at 0 parens depth
			zerocommas[commanum]=i;
			commanum++;
		}
	}
	zerocommas[commanum]=Newick.size()-startpos;  //end of the last child
	if (zerocommas[2] != 0) {    //if there are no zero depth commas then we want to terminate
		for (int i=0; i<MAXCHILDREN; i++) {
			if (zerocommas[i+1] != 0) {
				tempstring=Newick.substr(zerocommas[i]+1,zerocommas[i+1]-zerocommas[i]-1);  //remove the part of the string describing each child
				//cout << Newick << "->" << tempstring << endl;
				newtree->children[i]=ParseTree(tempstring);  //build that child and attach it to the root
			}
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
			cout << "Warning: Trying to find a label that doesn't exist." << endl;
		}
		i++;
	}
	return tree->x;
}

TreeNode *MakeTree(char label, TreeNode *templatetree) {  //returns the location of a new tree with nodes labeled label.
	TreeNode *newtree;
	newtree = CopyTree(templatetree,newtree);
	RelabelTree(newtree,label);
	return newtree;
}

int main(int argc, char *argv[])
{
	//SET UP STEP
	if (argc == 1) {
		cerr << "Please include an input file." << endl;
		exit(1);
	}
	ifstream inFile;
	inFile.open(argv[1]);
	string Newick;
	//float EtoI, XtoEnd, xetoxx;
	if (!inFile) {
		cerr << "Unable to open input file " << argv[1] << endl;
		exit(1);
	}
	inFile >> Newick;
	//inFile >> EtoI>>XtoEnd>>xetoxx;  //This line somehow causes the tree to have extra child pointers that cause segfaults.
	srand((unsigned)time(0)); //seed random number generator
	TreeNode *mytree;         //make a temporary tree to find the size and node locations for that tree
	TreeNode *testtree;
	mytree = ParseTree(Newick);
	//FullPrintTree(mytree);
	//cout << "done print" << endl;
		
	RelabelTree(mytree,'e'); //The segfault happens here if we try to read parameters from file. Segfault also happens here if use the tree in bug.gene.
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
	for (int i=1; i<NumNodes; i++) {   //at each of the nodes
		llconductor=llroot->next;
		llconductornext=llconductor->next;
		while (llconductornext != NULL) {  //move down the linked list until we get to the end
			ThisLabel=FindLabel(llconductor->tree,NodeLocations[i]);
			NextLabel=FindLabel(llconductornext->tree,NodeLocations[i]);
			r = (float)rand()/(float)RAND_MAX;   //random number between 0 and 1;
			switch (ThisLabel) {
				case 'x':
					if (r<xtoxex) {  //the first possible mutation is insertion of an exon into an intergenic sequence
						temptree1=CopyTree(llconductor->tree,temptree2);
						AddTree(temptree1,llconductor);
						chainleng++;
						temptree1=CopyTree(llconductor->tree,temptree2);
						MutateTree(temptree1,NodeLocations[i],'e');
						AddTree(temptree1,llconductor);
						chainleng++;
					}
					if (NextLabel=='e') {  //if it is followed by an exon then we can shorten or lengthen the exon
						if (r>=xtoxex && r<xtoxex+xetoxx) {
							temptree1 = CopyTree(llconductornext->tree,temptree2);
							MutateTree(temptree1,NodeLocations[i],'x');
							AddTree(temptree1,llconductor);
							chainleng++;
						}
						if (r>=xtoxex+xetoxx && r<xtoxex+xetoxx+xetoee) {
							temptree1 = CopyTree(llconductor->tree,temptree2);
							MutateTree(temptree1,NodeLocations[i],'e');
							AddTree(temptree1,llconductor);
							chainleng++;							
						}
					}
					break;
				case 'e':
					if (r<etoeie) {    //we can insert an intron
						temptree1=CopyTree(llconductor->tree,temptree2);
						AddTree(temptree1,llconductor);
						chainleng++;
						temptree1=CopyTree(llconductor->tree,temptree2);
						MutateTree(temptree1,NodeLocations[i],'i');
						AddTree(temptree1,llconductor);
						chainleng++;
					}
					if (r>=etoeie && r<etoeie+etoexe) {   //or we can insert an intergenic sequence
						temptree1=CopyTree(llconductor->tree,temptree2);
						AddTree(temptree1,llconductor);
						chainleng++;
						temptree1=CopyTree(llconductor->tree,temptree2);
						MutateTree(temptree1,NodeLocations[i],'x');
						AddTree(temptree1,llconductor);
						chainleng++;
					}
					if (NextLabel=='i') {   //if it followed by an intron then we can extend the exon into the intron of extend the intron into the exon
						if (r>=etoeie+etoexe && r<etoeie+etoexe+eitoee) {
							temptree1 = CopyTree(llconductornext->tree,temptree2);
							MutateTree(temptree1,NodeLocations[i],'e');
							AddTree(temptree1,llconductor);
							chainleng++;
						}
						if (r>=etoeie+etoexe+eitoee && r<etoeie+etoexe+eitoee+eitoii) {
							temptree1 = CopyTree(llconductor->tree,temptree2);
							MutateTree(temptree1,NodeLocations[i],'i');
							AddTree(temptree1,llconductor);
							chainleng++;
						}
					}
					if (NextLabel=='x') {   //similarly we can shorted or lengthen an exon into an intergenic sequence
						if (r>=etoeie+etoexe && r<etoeie+etoexe+extoee) {
							temptree1 = CopyTree(llconductornext->tree,temptree2);
							MutateTree(temptree1,NodeLocations[i],'e');
							AddTree(temptree1,llconductor);
							chainleng++;
						}
						if (r>=etoeie+etoexe+extoee && r<etoeie+etoexe+extoee+extoxx) {
							temptree1 = CopyTree(llconductor->tree,temptree2);
							MutateTree(temptree1,NodeLocations[i],'x');
							AddTree(temptree1,llconductor);
							chainleng++;
						}
					}
					break;
				case 'i':
					if (r<itoiei) {   //insert an exon into an intron
						temptree1=CopyTree(llconductor->tree,temptree2);
						AddTree(temptree1,llconductor);
						chainleng++;
						temptree1=CopyTree(llconductor->tree,temptree2);
						MutateTree(temptree1,NodeLocations[i],'e');
						AddTree(temptree1,llconductor);
						chainleng++;
					}
					if (NextLabel=='e') {   //if an exon sequence we can shorten or lengthen the intron into the exon.
						if (r>=itoiei && r<itoiei+ietoii) {
							temptree1 = CopyTree(llconductornext->tree,temptree2);
							MutateTree(temptree1,NodeLocations[i],'i');
							AddTree(temptree1,llconductor);
							chainleng++;
						}
						if (r>=itoiei+ietoii && r<itoiei+ietoii+ietoee) {
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
	for (int j=0;j<NumNodes;j++) {
		for (i=0;i<chainleng;i++) {
			cout << data[j][i];
		}
		cout << endl;
	}
}
