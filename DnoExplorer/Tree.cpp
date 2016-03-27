#include "stdafx.h"
#include "Tree.h"

int lengthOfArray(TreeNode* treeNodeArray[]){
	int counter = 0;
	while (treeNodeArray[counter]){
		counter++;
	}
	return counter;
}

Tree::Tree(char* name)
{
	root = (TreeNode*)calloc(1, sizeof(TreeNode));
	root->name = name;
	root->parent = NULL;
}

TreeNode * Tree::addFolder(TreeNode * parent, char * value)
{
	int length = lengthOfArray(parent->folders);
	parent->folders[length] = (TreeNode*)calloc(1, sizeof(TreeNode));
	parent->folders[length]->parent = parent;
	parent->folders[length]->name = value;
	return parent->folders[length];
}

TreeNode * Tree::addFile(TreeNode * parent, char * value)
{
	int length = lengthOfArray(parent->files);
	parent->files[length] = (TreeNode*)calloc(1, sizeof(TreeNode));
	parent->files[length]->parent = parent;
	parent->files[length]->name = value;
	return parent->files[length];
}


TreeNode * Tree::deleteNode(TreeNode * node)
{
	TreeNode* parent = node->parent;
	int length = lengthOfArray(parent->folders);
	int indexOfNode = -1;
	for (int i = 0; i < length; i++) {
		if (parent->folders[i]->name == node->name) {
			indexOfNode = i;
			break;
		}
	}
	if (indexOfNode > 0) {
		parent->folders[indexOfNode] = NULL;
		for (int i = indexOfNode + 1; i < length - 1; i++) {
			parent->folders[i] = parent->folders[i + 1];
		}
		parent->folders[length] = nullptr;
		return node;
	}
	length = lengthOfArray(parent->files);
	for (int i = 0; i < length; i++) {
		if (parent->files[i]->name == node->name) {
			indexOfNode = i;
			break;
		}
	}
	if (indexOfNode > 0) {
		parent->files[indexOfNode] = NULL;
		for (int i = indexOfNode + 1; i < length-1; i++) {
			parent->files[i] = parent->files[i + 1];
		}
		parent->files[length] = nullptr;
		return node;
	}
	throw;
}

TreeNode * Tree::renameNode(TreeNode * node, char * value)
{
	node->name = value;
	return node;
}

char * Tree::getFullPath(TreeNode* node)
{
	if (node->parent == NULL)
		return node->name;
	return strcat(getFullPath(node->parent) + '/',node->name);
}

TreeNode * Tree::find(char * name)
{
	return findRec(name, root);
}

TreeNode * Tree::findRec(char * name, TreeNode * node)
{
	if (node->name == name)
		return node;
	TreeNode* temp;
	int length = lengthOfArray(node->folders);
	for (int i = 0; i < length; i++) {
		temp = findRec(name, node->folders[i]);
		if (temp) break;
	}
	if (temp)
		return temp;
	length = lengthOfArray(node->files);
	for (int i = 0; i < length; i++){
		temp = findRec(name, node->files[i]);
		if (temp) break;
	}
	if (temp)
		return temp;
	return nullptr;
}
