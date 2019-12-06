#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <cctype> //for isalpha, isdigit, and isspace
#include <stack>  //for stack
using namespace std;

//initial tokens
#define LETTER 0
#define DIGIT 1
#define SPLIT 2
#define EOTF 3
#define SPACE 4

//tokens
#define IDENTIFIER 5
#define INTEGER 6
#define KEYWORD 7

//punctuation 
#define LEFT_PAREN 10
#define RIGHT_PAREN 11
#define LEFT_BRACE 12
#define RIGHT_BRACE 13
#define LEFT_BRACKET 14
#define RIGHT_BRACKET 15
#define COMMA 16
#define PERIOD 17
#define COLON 18
#define SEMI_COLON 19

// Operators
#define PRODUCT 20
#define SUM 21
#define DIFFERENCE 22
#define EQUALITY 23
#define QUOTIENT 24
#define GREATER_THAN 25
#define LESS_THAN 26
#define MODULUS 27

//more tokens
#define REAL 28

// Undefined
#define UNDEFINED 99

//Syntax Terminals
#define STerm 100
#define ETerm 101
#define QTerm 102
#define TTerm 103
#define RTerm 104
#define FTerm 105


class readInfile {
private:
	string filename_;
	string outFilename_;

	int numberTable[5][3] = {
		{ 1,	2,	0 },
	{ 1,	2,	3 },
	{ 2,	4,	4 },
	{ 0,	0,	0 },
	{ 0,	0,	0 }
	};



	int currNumberState = 0;


public:
	//public so can be accessed in different functions as characters come in
	ifstream ifs;
	ofstream ofs;
	int charClass;
	char lexeme[100];
	char inChar;	// get single character
	int length;
	int token;
	int nextToken;

	readInfile(const std::string& filename, const std::string& outfilename)
		: filename_(filename), outFilename_(outfilename) {}

	void readFile() {
		ifs.open(filename_);
		ofs.open(outFilename_);

		if (!ifs.is_open()) {
			cerr << "Could not open file: " << filename_ << "\n";
			exit(0);
		}
		else if (!ofs.is_open()) {
			cerr << "Could not open file: " << outFilename_ << "\n";
			exit(0);
		}
		else {
			getChar();

			ofs << "Tokens\t\t\tLexemes" << endl;
			do {
				nextToken = 0;
				lexical();
				printToken();
			} while (nextToken != EOTF);

			ofs.close();
			ifs.close();
		}
	}

	//Checks and runs all procedures on characters 
	int lexical() {
		length = 0;

		switch (charClass) {

		case LETTER:
			addChar();
			getChar();

			while (charClass == LETTER || charClass == DIGIT || inChar == '$') {
				addChar();
				getChar();
			}
			if (isKeyword() == (true))
				nextToken = KEYWORD;
			else
				nextToken = IDENTIFIER;;

			break;

		case DIGIT:
			addChar();
			getChar();

			while (currNumberState < 3) {
				addChar();
				getChar();
				if (isdigit(inChar)) {
					currNumberState = numberTable[currNumberState][0];
				}
				else if (inChar == '.') {
					currNumberState = numberTable[currNumberState][1];
				}
				else {
					currNumberState = numberTable[currNumberState][2];
				}
			}

			if (currNumberState == 3) {
				nextToken = INTEGER;
			}
			else if (currNumberState == 4) {
				nextToken = REAL;
			}
			else {
				nextToken = UNDEFINED;
			}

			currNumberState = 0;
			break;

		case SPLIT:
			clarify(inChar);
			getChar();
			break;
		case SPACE:
			while (isspace(inChar))
				getChar();
			break;
		case EOTF:
			nextToken = EOTF;
			break;
		}

		return nextToken;

	}

	void printToken() {

		bool printed = false;

		if (nextToken == 5) {
			ofs << "IDENTIFIER      =       " << lexeme << endl;
			printed = true;
		}
		else if (nextToken == 6) {
			ofs << "NUMBER          =       " << lexeme << endl;
			printed = true;
		}
		else if (nextToken == 28) {
			ofs << "REAL            =       " << lexeme << endl;
			printed = true;
		}
		else if (nextToken == 7) {
			ofs << "KEYWORD         =       " << lexeme << endl;
			printed = true;
		}
		else if ((nextToken >= 10) && (nextToken <= 19)) {
			ofs << "SEPARATOR       =       " << lexeme << endl;
			printed = true;
		}
		else if ((nextToken >= 20) && (nextToken <= 27)) {
			ofs << "OPERATOR        =       " << lexeme << endl;
			printed = true;
		}
		else if (nextToken == 99) {
			ofs << "Undefined       =       " << lexeme << endl;
			printed = true;
		}

		if (printed) {
			syntaxAnalyzer();
		}
	}

	void addChar() {

		if (length <= 98) {
			lexeme[length++] = inChar;
			lexeme[length] = 0;
		}

		else
			cout << "lexeme is too long" << endl;

	}

	void getChar() {

		ifs.get(inChar);

		if (!ifs.eof()) {
			if (isalpha(inChar))
				charClass = LETTER;
			else if (isdigit(inChar))
				charClass = DIGIT;
			else if (ispunct(inChar))
				charClass = SPLIT;
			else if (isspace(inChar))
				charClass = SPACE;
		}
		else
			charClass = EOTF;
	}

	int clarify(char ch) {

		switch (ch) {

		case('!'):
			isComment();
			break;
		case('('):
			addChar();
			nextToken = LEFT_PAREN;
			break;
		case(')'):
			addChar();
			nextToken = RIGHT_PAREN;
			break;
		case('{'):
			addChar();
			nextToken = LEFT_BRACE;
			break;
		case('}'):
			addChar();
			nextToken = RIGHT_BRACE;
			break;
		case('['):
			addChar();
			nextToken = LEFT_BRACKET;
			break;
		case(']'):
			addChar();
			nextToken = RIGHT_BRACKET;
			break;
		case(','):
			addChar();
			nextToken = COMMA;
			break;
		case('.'):
			addChar();
			nextToken = PERIOD;
			break;
		case(':'):
			addChar();
			nextToken = COLON;
			break;
		case(';'):
			addChar();
			nextToken = SEMI_COLON;
			break;
		case('*'):
			addChar();
			nextToken = PRODUCT;
			break;
		case('+'):
			addChar();
			nextToken = SUM;
			break;
		case('-'):
			addChar();
			nextToken = DIFFERENCE;
			break;
		case('='):
			addChar();
			nextToken = EQUALITY;
			break;
		case('/'):
			addChar();
			nextToken = QUOTIENT;
			break;
		case('>'):
			addChar();
			nextToken = GREATER_THAN;
			break;
		case('<'):
			addChar();
			nextToken = LESS_THAN;
			break;
		case('%'):
			addChar();
			nextToken = MODULUS;
			break;
		}
		return nextToken;
	}

	void isComment() {
		//gets the next character after an ! was found
		ifs.get(inChar);

		//iterates through the following characters until it finds another !
		while (inChar != '!')
			ifs.get(inChar);
	}

	bool isKeyword() {
		if (!strcmp(lexeme, "int") || !strcmp(lexeme, "float")
			|| !strcmp(lexeme, "bool") || !strcmp(lexeme, "if")
			|| !strcmp(lexeme, "else") || !strcmp(lexeme, "then")
			|| !strcmp(lexeme, "do") || !strcmp(lexeme, "while")
			|| !strcmp(lexeme, "whileend") || !strcmp(lexeme, "do")
			|| !strcmp(lexeme, "doend") || !strcmp(lexeme, "for")
			|| !strcmp(lexeme, "and") || !strcmp(lexeme, "or")
			|| !strcmp(lexeme, "function") || !strcmp(lexeme, "void"))
			return (true);
		return (false);
	}

	stack <int> syntaxStack;
	stack <int> inputStack;
	bool termError = false;
	int getSyntaxError = 0;

	void syntaxAnalyzer() {
		inputStack.push(nextToken); //adds non terminal value to inputStack

		if (syntaxStack.size() == 0) {
			initializeStack();
			//run operations on fresh stack
		}

		if (nextToken == SEMI_COLON || termError == true) {
			//Stack should be able to clear itself using rules
			//if stacksize != 0 statement could not be identified
			stackOperations();

			if (syntaxStack.size() != 0) {
				ofs << "Statement could not be identified" << endl << endl;
			}
			else if (termError == true) {
				ofs << "Could not evaluate ";
				switch (getSyntaxError)
				{
				case STerm:
					ofs << "S -> I = E production rule.\n\n";
					break;
				case ETerm:
					ofs << "E -> TQ production rule.\n\n";
					break;
				case QTerm:
					ofs << "Q -> +TQ | -TQ | e production rule.\n\n";
					break;
				case TTerm:
					ofs << "T -> FR production rule.\n\n";
					break;
				case RTerm:
					ofs << "R -> +FR | -FR | e production rule.\n\n";
					break;
				case FTerm:
					ofs << "F -> (E) | id production rule.\n\n";
					break;
				default:
					ofs << "Unknown Error.\n\n";
					break;
				}
			}
			else {
				ofs << "Statement valid " << endl << endl;
			}
			getSyntaxError = 0;
			termError = false;
		}
		else {
			stackOperations();
		}
	}

	void stackOperations() {

		bool popped = false;

		while (popped == false) {
			if (syntaxStack.top() == inputStack.top()) {
				printLexValue();
				syntaxStack.pop();
				popped = true;
			}
			else if (syntaxStack.top() == STerm) {
				if (nextToken == IDENTIFIER) {
					//pop S
					syntaxStack.pop();
					//push i = E
					syntaxStack.push(ETerm);
					syntaxStack.push(EQUALITY);
					syntaxStack.push(IDENTIFIER);
					ofs << "S -> I = E\n";
				}
				else {
					//cant parse
					getSyntaxError = STerm;
					termError = true;
					break;
				}
			}
			else if (syntaxStack.top() == ETerm) {
				if (nextToken == IDENTIFIER) {
					//pop E
					syntaxStack.pop();
					//push TQ
					syntaxStack.push(QTerm);
					syntaxStack.push(TTerm);
					ofs << "E -> TQ\n";
				}
				else if (nextToken == LEFT_PAREN) {
					//pop E
					syntaxStack.pop();
					//push TQ
					syntaxStack.push(QTerm);
					syntaxStack.push(TTerm);
					ofs << "E -> TQ\n";
				}
				else {
					//cant parse
					getSyntaxError = ETerm;
					termError = true;
					break;
				}
			}
			else if (syntaxStack.top() == QTerm) {
				if (nextToken == SUM) {
					//pop Q
					syntaxStack.pop();
					//push +TQ
					syntaxStack.push(QTerm);
					syntaxStack.push(TTerm);
					syntaxStack.push(SUM);
					ofs << "Q -> +TQ\n";
				}
				else if (nextToken == DIFFERENCE) {
					//pop Q
					syntaxStack.pop();
					//push -TQ
					syntaxStack.push(QTerm);
					syntaxStack.push(TTerm);
					syntaxStack.push(DIFFERENCE);
					ofs << "Q -> -TQ\n";

				}
				else if (nextToken == RIGHT_PAREN) {
					//pop Q
					syntaxStack.pop();
					ofs << "Q -> e\n";
				}
				else if (nextToken == SEMI_COLON) {
					//pop Q
					syntaxStack.pop();
					ofs << "Q -> e\n";
				}
				else {
					//cant parse
					getSyntaxError = QTerm;
					termError = true;
					break;
				}
			}
			else if (syntaxStack.top() == TTerm) {
				if (nextToken == IDENTIFIER) {
					//pop T
					syntaxStack.pop();
					//push FR
					syntaxStack.push(RTerm);
					syntaxStack.push(FTerm);
					ofs << "T -> FR\n";
				}
				else if (nextToken == LEFT_PAREN) {
					//pop T
					syntaxStack.pop();
					//push FR
					syntaxStack.push(RTerm);
					syntaxStack.push(FTerm);
					ofs << "T -> FR\n";
				}
				else {
					//cant parse
					getSyntaxError = TTerm;
					termError = true;
					break;
				}
			}
			else if (syntaxStack.top() == RTerm) {
				if (nextToken == SUM) {
					//pop R
					syntaxStack.pop();
					ofs << "R -> e\n";
				}
				else if (nextToken == DIFFERENCE) {
					//pop R
					syntaxStack.pop();
					ofs << "R -> e\n";
				}
				else if (nextToken == PRODUCT) {
					//pop R
					syntaxStack.pop();
					//push *FR
					syntaxStack.push(RTerm);
					syntaxStack.push(FTerm);
					syntaxStack.push(PRODUCT);
					ofs << "R -> *FR\n";
				}
				else if (nextToken == QUOTIENT) {
					//pop R
					syntaxStack.pop();
					//push /FR
					syntaxStack.push(RTerm);
					syntaxStack.push(FTerm);
					syntaxStack.push(QUOTIENT);
					ofs << "R -> /FR\n";
				}
				else if (nextToken == RIGHT_PAREN) {
					//pop R
					syntaxStack.pop();
					ofs << "R -> e\n";
				}
				else if (nextToken == SEMI_COLON) {
					//pop R
					syntaxStack.pop();
					ofs << "R -> e\n";
				}
				else {
					//cant parse
					getSyntaxError = RTerm;
					termError = true;
					break;
				}
			}
			else if (syntaxStack.top() == FTerm) {
				if (nextToken == IDENTIFIER) {
					//pop F
					syntaxStack.pop();
					//push identifier
					syntaxStack.push(IDENTIFIER);
					ofs << "F -> id\n";
				}
				else if (nextToken == LEFT_PAREN) {
					//pop F
					syntaxStack.pop();
					//push (E)
					syntaxStack.push(RIGHT_PAREN);
					syntaxStack.push(ETerm);
					syntaxStack.push(LEFT_PAREN);
					ofs << "F -> (E)\n";
				}
				else
				{
					//cant parse
					getSyntaxError = FTerm;
					termError = true;
					break;
				}
			}
			else {
				//shouldnt happen i dont think...
				break;
			}
		}
	}

	void printLexValue() {
		switch (syntaxStack.top())
		{
		case 5:
			cout << "<IDENTIFIER> ";
			break;
		case 21:
			cout << "+ ";
			break;
		case 22:
			cout << "- ";
			break;
		case 20:
			cout << "* ";
			break;
		case 24:
			cout << "/ ";
			break;
		case 10:
			cout << "(";
			break;
		case 11:
			cout << ") ";
			break;
		case EQUALITY:
			cout << "= ";
			break;
		case 19:
			cout << ";" << endl;
			break;
		}
	}

	void initializeStack() {
		//initializes stack to be {$S}
		syntaxStack.push(SEMI_COLON); //Push $ or ;
		syntaxStack.push(STerm);		//Push S
	}

};

int main()
{
	readInfile sampleCode("input.txt", "outputfile.txt");
	sampleCode.readFile();

	system("PAUSE");
	return 0;
}

