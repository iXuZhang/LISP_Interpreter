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

Evaluator::Evaluator(vector<TreeNode*>& SExpressions){
	int n = SExpressions.size();
	for(int i = 0; i < n; i++ )
		SValues.push_back(eval(SExpressions[i]));
}

TreeNode* Evaluator::eval(TreeNode* node){
	if(atom(node)){
		if(node->val == "T") return node;
		if(node->val == "NIL") return node;
		if(INT(node)) return node;
		cout << "ERROR : Invalid Atom"<<endl;
		exit(1);
	}
	if(car(node)->val == "QUOTE") {
		if(!null(cdr(cdr(node)))){
			cout << "ERROR : QUOTE PARAMS GREATER THAN ONE "<<endl;
			exit(1);
		}
		return car(cdr(node));
	} 
	if(car(node)->val == "COND"){
		bondCheck(cdr(node));
		return evcon(cdr(node));
	} 
	return apply(car(node), evlist(cdr(node))); 
}

bool Evaluator::atom(TreeNode* node){
	if(!node->left && !node->right) return true;
	return false;
}

bool Evaluator::INT(TreeNode* node){
	string s = node->val;
	int n = s.size();
	for(int i = 0; i < n; i++){
		if(!((s[i] >= '0' && s[i] <= '9') || (i == 0 && s[i] == '-')))
			return false;
	}
	return true;
}

bool Evaluator::null(TreeNode* node){
	if(!node){
		cout << "ERROR : NULL "<<endl;
		exit(1);
	}
	if(node->val == "NIL") return true;
	return false;
}

TreeNode*  Evaluator::car(TreeNode* node){
	if(atom(node)){
		cout << "ERROR : CAR "<<endl;
		exit(1);
	}
	return node->left;
}

TreeNode* Evaluator::cdr(TreeNode* node){
	if(!node->right){
		cout << "ERROR : CDR "<<endl;
		exit(1);
	}
	return node->right;
}

void Evaluator::bondCheck(TreeNode* node){
	if(null(node)) return;
	if(!null(cdr(cdr(car(node))))){
		cout << "ERROR : COND PARAMS GREATER THAN TWO"<<endl;
		exit(1);
	}
	bondCheck(cdr(node));
}

TreeNode* Evaluator::evcon(TreeNode* node){
	if(null(node)){
		cout << "ERROR : NULL "<<endl;
		exit(1);
	}
	if(eval(car(car(node)))->val != "T" && eval(car(car(node)))->val != "NIL"){
		cout << "ERROR : COND PARAMS IS NOT BOOL"<<endl;
		exit(1);		
	}
	if(eval(car(car(node)))->val == "T")
		return eval(car(cdr(car(node))));
	return evcon(cdr(node));
}

TreeNode* Evaluator::evlist(TreeNode* node){
	if(null(node)) return node;
	return cons(eval(car(node)), evlist(cdr(node)));
}

TreeNode* Evaluator::cons(TreeNode* node1, TreeNode* node2){
	TreeNode* node = new TreeNode(".");
	node->left = node1;
	node->right = node2;
	return node;
}

TreeNode* Evaluator::apply(TreeNode* f, TreeNode* x){
	if(!atom(f)){
		cout << "ERROR : FUNCTION NAME IS NOT ATOM"<<endl;
		exit(1);
	}
	TreeNode* node = new TreeNode("");
	if(f->val == "CAR"){
		if(!null(cdr(x))){
			cout << "ERROR : CAR PARAMS GREATER THAN ONE "<<endl;
			exit(1);
		}
		if(atom(car(x))){
			cout << "ERROR : CAR PARAMS IS ATOM "<<endl;
			exit(1);
		}
		return car(car(x));
	}
	if(f->val == "CDR"){
		if(!null(cdr(x))){
			cout << "ERROR : CDR PARAMS GREATER THAN ONE "<<endl;
			exit(1);
		}
		if(atom(car(x))){
			cout << "ERROR : CDR PARAMS IS ATOM "<<endl;
			exit(1);
		}		
		return cdr(car(x));
	} 
	if(f->val == "CONS"){
		if(!null(cdr(cdr(x)))){
			cout << "ERROR : CONS PARAMS GREATER THAN TWO "<<endl;
			exit(1);
		}
		return cons(car(x),car(cdr(x)));
	} 
	if(f->val == "ATOM"){
		if(!null(cdr(x))){
			cout << "ERROR : ATOM PARAMS GREATER THAN ONE "<<endl;
			exit(1);
		}
		node->val = atom(car(x)) ? "T" : "NIL";
		return node;
	}
	if(f->val == "EQ"){
		if(!null(cdr(cdr(x)))){
			cout << "ERROR : EQ PARAMS GREATER THAN TWO "<<endl;
			exit(1);
		}
		if(!atom(car(x)) || !atom(car(cdr(x)))){
			cout << "ERROR : EQ PARAMS ARE NOT ATOM "<<endl;
			exit(1);			
		}
		node->val = car(x)->val == car(cdr(x))->val ? "T" : "NIL";
		return node;		
	}
	if(f->val == "INT") {
		if(!null(cdr(x))){
			cout << "ERROR : INT PARAMS GREATER THAN ONE "<<endl;
			exit(1);
		}
		node->val = INT(car(x)) ? "T" : "NIL";
		return node;
	}
	if(f->val == "NULL"){
		if(!null(cdr(x))){
			cout << "ERROR : NULL PARAMS GREATER THAN ONE "<<endl;
			exit(1);
		}		
		node->val = null(car(x)) ? "T" : "NIL";
		return node;		
	}
	if(f->val == "PLUS"){
		if(!null(cdr(cdr(x)))){
			cout << "ERROR : PLUS PARAMS GREATER THAN TWO "<<endl;
			exit(1);
		}
		if(!(INT(car(x)) && INT(car(cdr(x))))){
			cout << "ERROR : PLUS PARAMS ARE NOT INT"<<endl;
			exit(1);			
		}
		int a = stoi(car(x)->val);
		int b = stoi(car(cdr(x))->val);
		node->val = to_string(a + b);
		return node;
	}
	if(f->val == "MINUS"){
		if(!null(cdr(cdr(x)))){
			cout << "ERROR : MINUS PARAMS GREATER THAN TWO "<<endl;
			exit(1);
		}
		if(!(INT(car(x)) && INT(car(cdr(x))))){
			cout << "ERROR : MINUS PARAMS ARE NOT INT"<<endl;
			exit(1);			
		}
		int a = stoi(car(x)->val);
		int b = stoi(car(cdr(x))->val);
		node->val = to_string(a - b);
		return node;
	}
	if(f->val == "TIMES"){
		if(!null(cdr(cdr(x)))){
			cout << "ERROR : TIMES PARAMS GREATER THAN TWO "<<endl;
			exit(1);
		}
		if(!(INT(car(x)) && INT(car(cdr(x))))){
			cout << "ERROR : TIMES PARAMS ARE NOT INT"<<endl;
			exit(1);			
		}
		int a = stoi(car(x)->val);
		int b = stoi(car(cdr(x))->val);
		node->val = to_string(a * b);
		return node;		
	}
	if(f->val == "QUOTIENT"){
		if(!null(cdr(cdr(x)))){
			cout << "ERROR : QUOTIENT PARAMS GREATER THAN TWO "<<endl;
			exit(1);
		}
		if(!(INT(car(x)) && INT(car(cdr(x))))){
			cout << "ERROR : QUOTIENT PARAMS ARE NOT INT"<<endl;
			exit(1);			
		}
		int a = stoi(car(x)->val);
		int b = stoi(car(cdr(x))->val);
		if(b == 0){
			cout << "ERROR : Invalid QUOTIENT"<<endl;
			exit(1);
		}
		node->val = to_string(a / b);
		return node;		
	}
	if(f->val == "REMAINDER"){
		if(!null(cdr(cdr(x)))){
			cout << "ERROR : REMAINDER PARAMS GREATER THAN TWO "<<endl;
			exit(1);
		}
		if(!(INT(car(x)) && INT(car(cdr(x))))){
			cout << "ERROR : REMAINDER PARAMS ARE NOT INT"<<endl;
			exit(1);			
		}
		int a = stoi(car(x)->val);
		int b = stoi(car(cdr(x))->val);
		if(b == 0){
			cout << "ERROR : Invalid QUOTIENT"<<endl;
			exit(1);
		}
		node->val = to_string(a % b);
		return node;	
	}
	if(f->val == "LESS"){
		if(!null(cdr(cdr(x)))){
			cout << "ERROR : LESS PARAMS GREATER THAN TWO "<<endl;
			exit(1);
		}
		if(!(INT(car(x)) && INT(car(cdr(x))))){
			cout << "ERROR : LESS PARAMS ARE NOT INT"<<endl;
			exit(1);			
		}
		int a = stoi(car(x)->val);
		int b = stoi(car(cdr(x))->val);
		node->val = a < b ? "T" : "NIL";
		return node;
	}
	if(f->val == "GREATER"){
		if(!null(cdr(cdr(x)))){
			cout << "ERROR : GREATER PARAMS GREATER THAN TWO "<<endl;
			exit(1);
		}
		if(!(INT(car(x)) && INT(car(cdr(x))))){
			cout << "ERROR : GREATER PARAMS ARE NOT INT"<<endl;
			exit(1);			
		}
		int a = stoi(car(x)->val);
		int b = stoi(car(cdr(x))->val);
		node->val = a > b ? "T" : "NIL";
		return node;
	}
	cout << "ERROR : Invalid Function"<<endl;
	exit(1);
}
