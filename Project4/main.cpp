/***
main.cpp
@Author Xu Zhang
11/16/2015
***/

#include "typechecker.h"

using namespace std;

int main(){
	Scanner myScanner;
	Parser myParser(myScanner.tokens);
	Typechecker myTypechecker(myParser.SExpressions);
	Printer myPrinter(myParser.SExpressions);
	return 0;
}