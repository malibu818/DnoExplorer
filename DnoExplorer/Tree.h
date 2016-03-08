#pragma once
#include "TreeNode.h"

class Tree{
	public:
		TreeNode* root;
		Tree(char* name);
		TreeNode* addFolder(TreeNode* parent, char* value);
		TreeNode* addFile(TreeNode* parent, char* value);
		TreeNode* deleteNode(TreeNode* node);
		TreeNode* renameNode(TreeNode* node, char* value);
		char* getFullPath(TreeNode* node);
		TreeNode* find(char* name);
	private:
		TreeNode* findRec(char* name, TreeNode* node);
};