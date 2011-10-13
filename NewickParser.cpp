#define MAXCHILDREN 4
#define MAXDEPTH 4
#include <iostream>
#include <string>
using namespace std;

struct TreeNode {
	string NodeLabel;			                      //the node label
	TreeNode *children[MAXCHILDREN];				//array of pointers to the children
};

TreeNode *ParseTree (string Newick, TreeNode *newtree) {
	newtree=new TreeNode;
	newtree->NodeLabel=Newick;
	if (Newick.size()<=3) {
		return 0;
	}
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
				cout << Newick << "->" << tempstring << endl;
				newtree->children[i]=ParseTree(tempstring,newtree->children[i]);
			}
		}
	}
	return newtree;
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

void PrintTree (TreeNode *root) {  //preorder print the tree. only used for debugging.
	if (root != NULL) {
		cout << root->NodeLabel << " ";
		for (int i=0;i<MAXCHILDREN;i++) {
			PrintTree( root->children[i] );
		}
	}
}

int main() {
	string NewickTree;
//	cin >> NewickTree;
	NewickTree = "(A,(B,C))";
	TreeNode *newtree;
	newtree=ParseTree(NewickTree,newtree);
	cout << endl;
	cout << endl;
	PrintTreeConnections(newtree);
	cout << endl;
	cout << endl;
	PrintTree(newtree);
	cout << endl;
}