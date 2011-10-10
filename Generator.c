#define MAXCHILDREN 4
#define MAXDEPTH 4
#include <iostream>
using namespace std;

//PARAMETERS. These will be read from file soon.
float EtoI=0.2;     //Probability of going from state E to state I. Otherwise we go from state E to state X.
float XtoEnd=0.1;   //Probability of ending any given time we are in state X. Otherwise we go to state E.
int BranchLength = 1;    //How many opportunites there are for mutation before reaching next node.
float xetoxx=0.2;   //Probability that we will make the xe to xx mutation at any given opportunity.

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

int myCounter = 0;  //there must be a better way

int TreeSize (TreeNode *root) {     //return the number of nodes in a tree
	if (root != NULL) {
		myCounter++;
		for (int i=0;i<MAXCHILDREN;i++) {
			TreeSize(root->children[i]);
		}
	}
	return myCounter;
}

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

void *NodeLocationsArray (TreeNode *root, int TheArray[], int depth, int ParentLocation[MAXDEPTH], int ParentDirection) {
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
			TheArray[myCounter*MAXDEPTH+i]=ThisLocation[i];
		}
		myCounter++;
		for (int i=0; i<MAXCHILDREN; i++) {
			NodeLocationsArray(root->children[i],TheArray,depth+1,ThisLocation,i);
		}
	}
}	


TreeNode *MakeTree(char label) {  //returns the location of a new tree with nodes labeled label.
	TreeNode *root;
	TreeNode *conductor;

	root = new TreeNode;
	root->children[0] = NULL;
	root->children[1] = NULL;
	root->x = label;
	conductor = root;
	conductor->children[0] = new TreeNode;
	conductor = conductor->children[0];
	conductor->x=label;
	conductor->children[0] = new TreeNode;
	conductor=conductor->children[0];
	conductor->x = label;
	conductor=root;
	conductor=conductor->children[0];
	conductor->children[1] = new TreeNode;
	conductor=conductor->children[1];
	conductor->x = label;
	conductor = root;
	conductor->children[1]=new TreeNode;
	conductor=conductor->children[1];
	conductor->x = label;
	
	return root;
}

TreeNode *CopyTree(TreeNode *oldtree, TreeNode *newtree) {
	if (oldtree != NULL) {
		newtree = new TreeNode;
		newtree->x=oldtree->x;
		for (int i=0; i<MAXCHILDREN; i++) {
			if (oldtree->children[i] != NULL) {
				newtree->children[i] = CopyTree(oldtree->children[i],newtree->children[i]);
			}
		}
	}
	return newtree;
}

void RelabelTree(TreeNode *tree, char newlabel) {
	tree->x=newlabel;
	for (int i=0; i<MAXCHILDREN; i++) {
		if (tree->children[i] != NULL) {
			RelabelTree(tree->children[i],newlabel);
		}
	}
}

void MutateTree (TreeNode *mutatefrom, int mutateat[MAXDEPTH], char mutateto) {
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

void AddTree (TreeNode *newtree, TreeList *previoustree) {
	TreeList *newlistitem;
	newlistitem = new TreeList;
	newlistitem->tree=newtree;
	newlistitem->next=previoustree->next;
	previoustree->next=newlistitem;
}

char FindLabel (TreeNode *tree, int local[MAXDEPTH]) {
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

int main()
{
	srand((unsigned)time(0)); //seed random number generator
	TreeNode *mytree;         //make a temporary tree to find the size and node locations for that tree
	mytree = MakeTree('e');
	myCounter=0;
	int NumNodes;
	NumNodes = TreeSize(mytree);	
	int TempNodeLocations[NumNodes*MAXDEPTH];
	int NodeLocations[NumNodes][MAXDEPTH];
	myCounter = 0;
	NodeLocationsArray(mytree,TempNodeLocations,0,NULL,NULL);
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
				llconductor->tree=MakeTree('x');
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
				llconductor->tree=MakeTree('e');
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
				llconductor->tree=MakeTree('i');
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
	
//	cout << endl << chainleng << endl;
	for (int i=1; i<NumNodes; i++) {
		llconductor=llroot->next;
		llconductornext=llconductor->next;
		while (llconductornext != NULL) {
			ThisLabel=FindLabel(llconductor->tree,NodeLocations[i]);
			NextLabel=FindLabel(llconductornext->tree,NodeLocations[i]);
			if (ThisLabel == 'x' && NextLabel == 'e') {
				r = (float)rand()/(float)RAND_MAX;
				if (r<xetoxx) {
					temptree1 = CopyTree(llconductornext->tree,temptree2);
					MutateTree(temptree1,NodeLocations[i],'x');
					AddTree(temptree1,llconductor);
					chainleng++;
				}
			}
			llconductor=llconductornext;
			llconductornext=llconductornext->next;
		}
	}	
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
