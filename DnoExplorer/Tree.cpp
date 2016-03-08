#include "stdafx.h"
#include "Tree.h"

Tree::Tree(char* name)
{
	root = (TreeNode*)calloc(1, sizeof(TreeNode));
	root->name = name;
	root->parent = NULL;
}

TreeNode * Tree::addFolder(TreeNode * parent, char * value)
{
	int length = (int)sizeof(parent->folders) / sizeof(TreeNode*);
	parent->folders[length] = (TreeNode*)calloc(1, sizeof(TreeNode));
	parent->folders[length]->parent = parent;
	parent->folders[length]->name = value;
	return parent->folders[length];
}

TreeNode * Tree::addFile(TreeNode * parent, char * value)
{
	int length = (int)sizeof(parent->files) / sizeof(TreeNode*);
	parent->files[length] = (TreeNode*)calloc(1, sizeof(TreeNode));
	parent->files[length]->parent = parent;
	parent->files[length]->name = value;
	return parent->files[length];
}


TreeNode * Tree::deleteNode(TreeNode * node)
{
	TreeNode* parent = node->parent;
	int length = (int)sizeof(parent->folders) / sizeof(TreeNode*);
	int indexOfNode = -1;
	for (int i = 0; i < length; i++) {
		if (parent->folders[i]->name == node->name) {
			indexOfNode = i;
			break;
		}
	}
	if (indexOfNode > 0) {
		parent->folders[indexOfNode] = NULL;
		return node;
	}
	length = (int)sizeof(parent->files) / sizeof(TreeNode*);
	for (int i = 0; i < length; i++) {
		if (parent->files[i]->name == node->name) {
			indexOfNode = i;
			break;
		}
	}
	if (indexOfNode > 0) {
		parent->files[indexOfNode] = NULL;
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
	int length = (int)sizeof(node->folders) / sizeof(TreeNode);
	for (int i = 0; i < length; i++) {
		findRec(name, node->folders[i]);
	}
	length = (int)sizeof(node->files) / sizeof(TreeNode);
	for (int i = 0; i < length; i++){
		findRec(name, node->files[i]);
	}
	return nullptr;
}
