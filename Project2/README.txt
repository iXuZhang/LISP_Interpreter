Xu Zhang

8 kinds of ERRORs:
ERROR : Empty input                     // No input
ERROR : Incompleted Input  				// such as ((A.B
ERROR : Lower Letters      				// such as (a.B)
ERROR : Ivalid Iput        				// such as  %#$
ERROR : Mixed Number and Letter 		// such as 123A
ERROR : Lack Atom or Opening Parentheses// such as (A.)
ERROR : Lack Dot	   		    		// such as (A B)
ERROR : Lack Closing Parentheses		// such as (A.B.A)

The project includes three class(Parser, Printer, Scanner)and a Struct(TreeNode).

Scanner includes two function getAllTokens and getNextToken. And they will throw different Errors according to different invalid input.

Parser includes two functions:
ParseStart: 
builds a tree root of the S-Expression, calls ParseSexp, finally sends the root to printer. If there are more tokens, call itself again.
ParseSexp:
It calls getNextToken to get the next token. If it is not Atom or OpenParenthesis, an error will be reported. If it is Atom, if it is NIL, isList is true, the function returns. If it is OpenParenthesis, the function will call itself, then will get the next token, report an error if it is not Dot, call itself again, get the next token, and report an error if it is not ClosingParenthesis. A inner node’s isList equals its right node’s isList. Use AND operation to all inner nodes, if there is one inner node’s isList is false, the whole value will be false, which is stored in PList.

Printer includes two functions: printNode and printList, which used to print in Node notation and List notation. If PList is true, printList, or printNode.

Contact:
xuzhang@xzcode.com
