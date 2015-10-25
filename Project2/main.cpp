/***
main.cpp
LISP Interpreter Project
@Author Xu Zhang
10/02/2015
***/

#include "interpreter.h"

using namespace std;

int main(){
	Scanner myScanner;
	Parser myParser(myScanner.tokens);
	Evaluator myEvaluator(myParser.SExpressions);
	Printer myPrinter(myEvaluator.SValues);
	return 0;
}