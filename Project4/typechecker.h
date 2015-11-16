#ifndef __TYPECHECKER_H__ 
#define __TYPECHECKER_H__ 

/***
typechecker.h
@Author Xu Zhang
11/16/2015
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

// A printer to print in List notation or Node notation
class Printer{
public:
	Printer(vector<TreeNode*>& SExpressions);
	void checkPList(TreeNode *root, bool& pList);
	void printNode(TreeNode *root, string& res);
	void printList(TreeNode *root, string& res);
};

// A typechecker to do tpye check
class Typechecker{
public:
	Typechecker(vector<TreeNode*>& SExpressions);
	string checker(TreeNode* node);
	void condCheck(TreeNode* node, vector<string>& tpyes);
	bool atom(TreeNode* node);
	bool null(TreeNode* node);
	bool INT(TreeNode* node);
	TreeNode* car(TreeNode* node);
	TreeNode* cdr(TreeNode* node);
};

#endif
