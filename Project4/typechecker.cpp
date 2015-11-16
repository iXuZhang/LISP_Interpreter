/***
typechecker.cpp
LISP Interpreter Project
@Author Xu Zhang
11/16/2015
***/
 
#include "typechecker.h"

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

Evaluator::Evaluator(vector<TreeNode*>& SExpressions){
	int n = SExpressions.size();
	map<string,TreeNode*> a;
	map<string,TreeNode*> d;
	for(int i = 0; i < n; i++ ){
		TreeNode* curr = eval(SExpressions[i], a, d);
		if(curr) SValues.push_back(curr);
	}
}

TreeNode* Evaluator::eval(TreeNode* node, map<string,TreeNode*> a, map<string,TreeNode*>& d){
	if(atom(node)){
		if(node->val == "T")
			return copy(node);
		if(node->val == "NIL")
			return copy(node);
		if(INT(node))
			return copy(node);
		if(a.count(node->val) > 0)
			return copy(a[node->val]);
		cout << "ERROR : Invalid Atom in Eval"<<endl;
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
		return evcon(cdr(node), a, d);
	}
	if(car(node)->val == "DEFUN"){
		if(!defunCheck(node)){
			cout << "ERROR : DEFUN PARAMS ERROR "<<endl;
			exit(1);
		}
		d[car(cdr(node))->val] = cdr(cdr(node));
		TreeNode* funName = new TreeNode(car(cdr(node))->val);
		return funName;
	}
	return apply(car(node), evlist(cdr(node),a,d), a, d); 
}

bool Evaluator::defunCheck(TreeNode* node){
	TreeNode* curr = cdr(node);
	if(!atom(car(curr)) || car(curr)->val == "QUOTE" || car(curr)->val  == "COND" || car(curr)->val  == "DEFUN") return false;
	if(car(curr)->val == "CAR" || car(curr)->val  == "CDR" || car(curr)->val  == "EQ" || car(curr)->val  == "CONS") return false;
	if(car(curr)->val == "ATOM" || car(curr)->val  == "NULL" || car(curr)->val  == "INT" || car(curr)->val  == "MINUS") return false;
	if(car(curr)->val == "TIMES" || car(curr)->val  == "GREATER" || car(curr)->val  == "LESS" || car(curr)->val  == "QUOTIENT") return false;
	if(car(curr)->val == "REMAINDER" || car(curr)->val  == "TIMES" ||  car(curr)->val  == "PLUS") return false;
	curr = car(cdr(curr));
	if(!null(curr) && atom(curr)) return false;
	set<string> table;
	while(!null(curr)){
		if(!atom(car(curr)) || INT(car(curr)) ||car(curr)->val == "T" || car(curr)->val == "NIL") return false;
		if(table.count(car(curr)->val) > 0) return false;
		table.insert(car(curr)->val);
		curr = cdr(curr);
	}
	if(!null(cdr(cdr(cdr(cdr(node)))))) return false;
	return true;
}

bool Evaluator::atom(TreeNode* node){
	if(!node){
		cout << "ERROR : NULL "<<endl;
		exit(1);		
	}
	if(!node->left && !node->right) return true;
	return false;
}

bool Evaluator::INT(TreeNode* node){
	if(!node){
		cout << "ERROR : NULL "<<endl;
		exit(1);		
	}
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

int Evaluator::mystoi(string s){
	int res = 0;
	int flag = 1;
	int i = 0;
	if(s[i] == '-'){
		i++;
		flag = -1;
	} 
	int n = s.size();
	for (; i < n; i++)
        res = res*10 + (s[i] - '0');
    return res*flag;
}

string Evaluator::myto_string(int n){
	string res;
	string flag;
	if(n == 0) return "0";
	if(n < 0){
		flag = '-';
		n = -n;
	}
	while(n > 0){
		res = char(n%10 + '0') + res;
		n /= 10;
	}
	return flag + res;
}

TreeNode* Evaluator::copy(TreeNode* node){
	if(!node) return NULL;
	TreeNode* newnode = new TreeNode(node->val);
	newnode->left = copy(node->left);
	newnode->right = copy(node->right);
	return newnode;
}

TreeNode* Evaluator::car(TreeNode* node){
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

TreeNode* Evaluator::cdr(TreeNode* node){
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

void Evaluator::bondCheck(TreeNode* node){
	if(null(node)) return;
	if(!null(cdr(cdr(car(node))))){
		cout << "ERROR : COND PARAMS GREATER THAN TWO"<<endl;
		exit(1);
	}
	bondCheck(cdr(node));
}

TreeNode* Evaluator::evcon(TreeNode* node, map<string,TreeNode*> a, map<string,TreeNode*>& d){
	if(null(node)){
		cout << "ERROR : NULL "<<endl;
		exit(1);
	}
	if(eval(car(car(node)), a, d)->val != "T" && eval(car(car(node)), a, d)->val != "NIL"){
		cout << "ERROR : COND PARAMS IS NOT BOOL"<<endl;
		exit(1);		
	}
	if(eval(car(car(node)), a, d)->val == "T")
		return eval(car(cdr(car(node))), a, d);
	return evcon(cdr(node), a, d);
}

TreeNode* Evaluator::evlist(TreeNode* x, map<string,TreeNode*> a, map<string,TreeNode*>& d){
	if(null(x)) return copy(x);
	return cons(eval(car(x), a, d), evlist(cdr(x), a, d));
}

TreeNode* Evaluator::cons(TreeNode* node1, TreeNode* node2){
	TreeNode* node = new TreeNode(".");
	node->left = node1;
	node->right = node2;
	return node;
}

void Evaluator::addpairs(TreeNode* node, TreeNode* x, map<string,TreeNode*>& a, map<string,TreeNode*>& d){
	if(null(node) && null(x)) return;
	if(null(node) || null(x) || !atom(car(node)) || INT(car(node))){
		cout << "ERROR : ADDPAIRS" <<endl;
		exit(1);
	}
	a[car(node)->val] = car(x);
	addpairs(cdr(node), cdr(x), a, d);
}

TreeNode* Evaluator::apply(TreeNode* f, TreeNode* x, map<string,TreeNode*> a, map<string,TreeNode*>& d){
	if(!atom(f)){
		vector<TreeNode*> temp;
		temp.push_back(f);
		Printer myPrinter(temp);
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
		int b = mystoi(car(x)->val);
		int c = mystoi(car(cdr(x))->val);
		node->val = myto_string(b + c);
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
		int b = mystoi(car(x)->val);
		int c = mystoi(car(cdr(x))->val);
		node->val = myto_string(b - c);
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
		int b = mystoi(car(x)->val);
		int c = mystoi(car(cdr(x))->val);
		node->val = myto_string(b * c);
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
		int b = mystoi(car(x)->val);
		int c = mystoi(car(cdr(x))->val);
		if(c == 0){
			cout << "ERROR : Invalid QUOTIENT"<<endl;
			exit(1);
		}
		node->val = myto_string(b / c);
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
		int b = mystoi(car(x)->val);
		int c = mystoi(car(cdr(x))->val);
		if(c == 0){
			cout << "ERROR : Invalid QUOTIENT"<<endl;
			exit(1);
		}
		node->val = myto_string(b % c);
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
		int b = mystoi(car(x)->val);
		int c = mystoi(car(cdr(x))->val);
		node->val = b < c ? "T" : "NIL";
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
		int b = mystoi(car(x)->val);
		int c = mystoi(car(cdr(x))->val);
		node->val = b > c ? "T" : "NIL";
		return node;
	}
	addpairs(car(d[f->val]), x, a, d);
	return eval(car(cdr(d[f->val])), a, d);
}
