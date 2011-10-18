#define MAXCHILDREN 4
#define MAXDEPTH 4
#include <iostream>
#include <string>
using namespace std;

struct TreeNode {
	string NodeLabel;			                    //the node label
	TreeNode *children[MAXCHILDREN];				//array of pointers to the children
};

void PrintTree (TreeNode *root) {  //preorder print the tree. only used for debugging.
	if (root != NULL) {
		cout << root->NodeLabel << " ";
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

TreeNode *ParseTree (string Newick) {
	int lastclose=Newick.size()-1;
	while (Newick[lastclose] != ')' && lastclose>=0) {
		lastclose--;
	}
	string labelandlength=Newick.substr(lastclose+1,Newick.size()-lastclose);
	int colon=0;
	while (labelandlength[colon] != ':' && colon < labelandlength.size()) {
		colon++;
	}
	string justlabel = labelandlength.substr(0,colon);
	Newick=Newick.substr(0,lastclose+1);
	TreeNode *newtree;
	newtree=new TreeNode;
	newtree->NodeLabel=justlabel;
	int startpos;
	if (Newick[0]=='(') {
		startpos=1;
	}
	else {
		startpos=0;
	}
	int unclosed=0;
	int zerocommas[MAXCHILDREN+1];
	for (int i=0; i<MAXCHILDREN+1; i++) {
		zerocommas[i]=0;
	}
	zerocommas[0]=startpos-1;
	int commanum=1;
	string tempstring;
	for (int i=startpos; i<=Newick.size()-startpos; i++) {
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
	zerocommas[commanum]=Newick.size()-startpos;
	if (zerocommas[2] != 0) {
		for (int i=0; i<MAXCHILDREN; i++) {
			if (zerocommas[i+1] != 0) {
				tempstring=Newick.substr(zerocommas[i]+1,zerocommas[i+1]-zerocommas[i]-1);
				//cout << Newick << "->" << tempstring << endl;
				newtree->children[i]=ParseTree(tempstring);
			}
			else {
				newtree->children[i]=NULL;
			}
		}
	}
	else {
		cout << newtree->NodeLabel << endl;
		for (int i=0; i<MAXCHILDREN; i++) {
			newtree->children[i]=NULL;
		}
	}

	return newtree;
}

void FullPrintTree (TreeNode *root) {   //print locations of nodes and children in memory. for debugging only
	if (root != NULL) {
		cout << "For node at " << root << endl;
		cout << "The children are at ";
		for (int i=0; i<MAXCHILDREN; i++) {
			cout << root->children[i] << " ";
		}
		cout << endl;
		cout << "and the label is " << root->NodeLabel << endl;
		for (int i=0;i<MAXCHILDREN;i++) {
			FullPrintTree( root->children[i] );
		}
	}
}

void PrintTreeConnections (TreeNode *root) {
	TreeNode *temppointer;
	for (int i=0;i<MAXCHILDREN;i++) {
		if (root->children[i] != NULL) {
			temppointer=root->children[i];
			cout << root->NodeLabel << "->" << temppointer->NodeLabel << endl;
			PrintTreeConnections( root->children[i] );
		}
	}
}

int main() {
	string NewickTree;
	NewickTree = "(((A1:.1,A2:.1,A3:.1)A:.1,(B1:.1,B2:.1,B3:.1)B:.1)AB:.1,(C1:.1,C2:.1,C3:.1)C:.1)ABC";
	//	cin >> NewickTree;
	TreeNode *newtree;
	newtree=ParseTree(NewickTree);
	cout << endl;
	cout << endl;
	PrintTreeConnections(newtree);
	cout << endl;
	cout << endl;
	//PrintTree(newtree);
	cout << endl;
	cout << endl;
	myCounter=0;
	//cout << TreeSize(newtree) << endl;
	//FullPrintTree(newtree);
}