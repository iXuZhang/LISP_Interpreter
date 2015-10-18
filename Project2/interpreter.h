#ifndef __INTERPRETER_H__ 
#define __INTERPRETER_H__ 

/***
interpreter.h
LISP Interpreter Project
@Author Xu Zhang
10/02/2015
***/

#include "common.h"

using namespace std;

// A basic node to build a binary tree
struct TreeNode{
	string val;
	bool isList;
	TreeNode *left;
	TreeNode *right;
	TreeNode(string x): val(x), isList(false),left(NULL),right(NULL){}
};

// A sacnner to get next token
class Scanner{
public:
	vector<string> tokens;
	Scanner();
	void getAllTokens();
	string getNextToken(string &s, int &i);

};

// A parser to analyze the S-Expression
class Parser{
public:	
	vector<TreeNode*> SExpressions;
	Parser(vector<string>& tokens);
	void parseStart(vector<string> &tokens, int& i);
	TreeNode* parseSexp(vector<string> &tokens, int& i);
};

// A printer a print in List notation or Node notation
class Printer{
public:
	Printer(vector<TreeNode*>& SExpressions);
	void checkPList(TreeNode *root, bool& pList);
	void printNode(TreeNode *root, string& res);
	void printList(TreeNode *root, string& res);
};

#endif
