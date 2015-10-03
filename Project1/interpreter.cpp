/***
Interpreter.cpp
LISP Interpreter Project
@Author Xu Zhang
10/02/2015
***/
 
#include "interpreter.h"

using namespace std;

Scanner::Scanner(){
	getAllTokens();
}
	
void Scanner::getAllTokens(){
	string s;
	string curr;
	while(getline (cin,curr)) s += " " + curr;
	int n = s.size();
	int i = 0;
	while(i < n){
		string token = getNextToken(s, i);
		if(!token.empty()) tokens.push_back(token);
	}
	if(tokens.empty()){
		cout << "ERROR : Empty Input "<<endl;
		exit(1);
	}
}

string Scanner::getNextToken(string &s, int &i){
	int n = s.size();
	string token;
	bool isNumber = false;
	bool isLetter = false;
	while(i < n){
		if(s[i] >= '0' && s[i] <= '9'){
			token += s[i];
			if(!isLetter)isNumber = true;
		}
		else if(s[i] >= 'A' && s[i] <= 'Z'){
			token += s[i];
			isLetter = true;
		}
		else if((s[i] == ' ' || s[i] == '\t' )&& !isNumber && !isLetter);
		else if((s[i] == '.' || s[i] == '(' || s[i] == ')') && !isNumber && !isLetter){
			token = s[i++];
			return token;
		}
		else if(s[i] == '.' || s[i] == '(' || s[i] == ')' || s[i] == ' ' || s[i] == '\t')
			return token;
		else if(s[i] >= 'a' && s[i] <= 'z'){
			cout << "ERROR : Lower Letters for " << s[i] <<endl;
			exit(1);
		}
		else{
			cout << "ERROR : Ivalid Iput for " << s[i] << endl;
			exit(1);
		}
		if(isNumber && isLetter){
			cout << "ERROR : Mixed Numbers and Letters for " << token <<endl;
			exit(1);
		} 
		i++;
	}
	return token;
}

Parser::Parser(vector<string>& tokens){
	int i = 0;
	parseStart(tokens, i);
}

void Parser::parseStart(vector<string> &tokens, int& i){
	int n = tokens.size();
	TreeNode *root = parseSexp(tokens,i);
	SExpressions.push_back(root);
	if(i < n) parseStart(tokens, i);
}

TreeNode* Parser::parseSexp(vector<string>& tokens, int &i){	
	int n = tokens.size();
	string token = tokens[i++];
	if(token == ")" ||  token == "."){
		cout << "ERROR : Lack Atom or Opening Parentheses before "<< tokens[i-1] <<endl;
		exit(1);
	}
	if(token != "("){
		TreeNode *root = new TreeNode(token);
		if(token == "NIL") root->isList = true;
		return root;
	}
	if(i == n){
		cout << "ERROR : Incompleted Input "<<endl;
		exit(1);
	}
	TreeNode *root = new TreeNode(".");
	root->left = parseSexp(tokens,i);
	if(tokens[i++] != "."){
		cout << "ERROR : Lack Dot before "<< tokens[i-1] <<endl;
		exit(1);
	}
	if(i == n){
		cout << "ERROR : Incompleted Input "<<endl;
		exit(1);
	}
	root->right = parseSexp(tokens,i);
	if(i == n){
		cout << "ERROR : Incompleted Input "<<endl;
		exit(1);
	}
	if(tokens[i++] != ")"){
	cout << "ERROR : Lack Closing Parentheses before "<< tokens[i-1] <<endl;
		exit(1);
	}
	root->isList = root->right->isList;
	return root;
}

Printer::Printer(vector<TreeNode*>& SExpressions){
	int i = 0;
	int n = SExpressions.size();
	while(i < n){
		string res;
		TreeNode* root = SExpressions[i];
		bool pList = true;
		checkPList(root, pList);
		if(pList) printList(root, res);
		else printNode(root, res);
		cout << res << endl;
		i++;
	}
}

void Printer::checkPList(TreeNode *root, bool& pList){
	if(!root) return;
	if(!root->left && !root->left) return;
	pList &= root->isList;
	checkPList(root->left, pList);
	checkPList(root->right, pList);
}

void Printer::printNode(TreeNode *root, string& res){
	if(!root) return;
	if(!root->left && !root->right){
		res += root->val;
		return;
	}
	res += "(";
	printNode(root->left, res);
	res += " . ";
	printNode(root->right, res);
	res += ")";
}

void Printer::printList(TreeNode *root, string& res){
	if(!root) return;
	if(!root->left && !root->right){
		res += root->val;
		return;
}
	res += "(";
	printList(root->left, res);	
	while(root->right->val != "NIL"){
		res += " ";
		printList(root->right->left, res);
		root = root->right;
	}
	res += ")";
}
