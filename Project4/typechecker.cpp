/***
typechecker.cpp
@Author Xu Zhang
11/16/2015
***/
 
#include "typechecker.h"

using namespace std;

Scanner::Scanner() {getAllTokens();}
	
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
	if(root->val == "NIL") root->isList = true;
	if(!root->left && !root->left) return;
	checkPList(root->left, pList);
	checkPList(root->right, pList);
	root->isList = root->right->isList;
	pList &= root->isList;
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

Typechecker::Typechecker(vector<TreeNode*>& SExpressions){
	int n = SExpressions.size();
	for(int i = 0; i < n; i++ )
		checker(SExpressions[i]);
}

string Typechecker::checker(TreeNode* node){
	if(atom(node)){
		if(node->val == "T" || node->val == "F") return "Bool";
		if(node->val == "NIL") return "List(Nat)";
		if(INT(node)) return "Nat";
		cout << "ERROR : Invalid Atom "<<endl;
		exit(1);
	}
	TreeNode* f = car(node);
	TreeNode* x = cdr(node);
	if(f->val == "COND"){
		vector<string> types;
		condCheck(cdr(node), types);
		if(types.empty()){
			cout << "ERROR : COND TYPE EMPTY "<<endl;
			exit(1);
		}
		const int n = types.size();
		for(int i = 1; i < n; i++)
			if(types[0] != types[i]){
				cout << "ERROR : COND TYPE "<<endl;
				exit(1);
			}
		return types[0];
	}
	if(f->val == "CAR"){
		if(!null(cdr(x))){
			cout << "ERROR : CAR PARAMS GREATER THAN ONE "<<endl;
			exit(1);
		}
		if(checker(car(x)) == "List(Nat)") return "Nat";
		cout << "ERROR : CAR TYPE "<<endl;
		exit(1);
	}
	if(f->val == "CDR"){
		if(!null(cdr(x))){
			cout << "ERROR : CDR PARAMS GREATER THAN ONE "<<endl;
			exit(1);
		}
		if(checker(car(x)) == "List(Nat)") return "List(Nat)";
		cout << "ERROR : CDR TYPE "<<endl;
		exit(1);
	} 
	if(f->val == "CONS"){
		if(!null(cdr(cdr(x)))){
			cout << "ERROR : CONS PARAMS GREATER THAN TWO "<<endl;
			exit(1);
		}
		if(checker(car(x)) == "Nat" && checker(car(cdr(x))) == "List(Nat)") return "List(Nat)";
		cout << "ERROR : CONS TYPE "<<endl;
		exit(1);
	} 
	if(f->val == "ATOM" || f->val == "INT"){
		if(!null(cdr(x))){
			cout << "ERROR : ATOM/INT PARAMS GREATER THAN ONE "<<endl;
			exit(1);
		}
		return "Bool";
	}
	if(f->val == "EQ" || f->val == "LESS"){
		if(!null(cdr(cdr(x)))){
			cout << "ERROR : EQ/LESS PARAMS GREATER THAN TWO "<<endl;
			exit(1);
		}
		if(checker(car(x)) == "Nat" && checker(car(cdr(x))) == "Nat") return "Bool";
		cout << "ERROR : EQ/LESS TYPE "<<endl;
		exit(1);	
	}
	if(f->val == "NULL"){
		if(!null(cdr(x))){
			cout << "ERROR : NULL PARAMS GREATER THAN ONE "<<endl;
			exit(1);
		}
		if(checker(car(x)) == "List(Nat)") return "Bool";
		cout << "ERROR : NULL TYPE "<<endl;
		exit(1);
	}
	if(f->val == "PLUS"){
		if(!null(cdr(cdr(x)))){
			cout << "ERROR : PLUS PARAMS GREATER THAN TWO "<<endl;
			exit(1);
		}
		if(checker(car(x)) == "Nat" && checker(car(cdr(x))) == "Nat") return "Nat";
		cout << "ERROR : PLUS LESS TYPE "<<endl;
		exit(1);
	}
	cout << "ERROR : Undefined Function"<<endl;
	exit(1);
}

void Typechecker::condCheck(TreeNode* node, vector<string>& types){
	if(null(node)) return;
	if(checker(car(car(node))) != "Bool"){
		cout << "ERROR : COND PARAMS GREATER THAN TWO"<<endl;
		exit(1);
	}
	types.push_back(checker(car(cdr(car(node)))));
	condCheck(cdr(node), types);
}

bool Typechecker::atom(TreeNode* node){
	if(!node){
		cout << "ERROR : NULL "<<endl;
		exit(1);
	}
	if(!node->left && !node->right) return true;
	return false;
}

bool Typechecker::INT(TreeNode* node){
	if(!atom(node)) return false;
	string s = node->val;
	int n = s.size();
	for(int i = 0; i < n; i++)
		if(!((s[i] >= '0' && s[i] <= '9') || (i == 0 && s[i] == '-'))) return false;
	return true;
}

bool Typechecker::null(TreeNode* node){
	if(!node){
		cout << "ERROR : NULL "<<endl;
		exit(1);
	}
	if(atom(node) && node->val == "NIL") return true;
	return false;
}

TreeNode* Typechecker::car(TreeNode* node){
	if(!node){
		cout << "ERROR : NULL "<<endl;
		exit(1);		
	}
	if(atom(node)){
		cout << "ERROR : CAR "<<endl;
		exit(1);
	}
	return node->left;
}

TreeNode* Typechecker::cdr(TreeNode* node){
	if(!node){
		cout << "ERROR : NULL "<<endl;
		exit(1);		
	}
	if(!node->right){
		cout << "ERROR : CDR "<<endl;
		exit(1);
	}
	return node->right;
}
