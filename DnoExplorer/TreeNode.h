#pragma once
#define MAX_FILES_IN_FOLDER 10
#define MAX_FOLDERS_IN_FOLDER 10
#define MAX_FILE_NAME_LENGTH 200
class TreeNode {
	public:
		TreeNode* parent;
		TreeNode* files[MAX_FILES_IN_FOLDER];
		TreeNode* folders[MAX_FOLDERS_IN_FOLDER];
		char* name;
};