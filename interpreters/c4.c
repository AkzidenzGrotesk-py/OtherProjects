#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "linkdict.h"
#define int long long

/**************** FEATURES ****************/
// arithmetic w/ BEDMAS
// variable declarations
// ---- syntax: identifier = value;
// ---- ---- example: a = 5;
// ---- limitations:
// ---- ---- types supported: int
// ---- ---- max identifier length: 16
// built-in functions
// ---- print to console: PUTS
// ---- ---- puts value; || puts(value);
// ---- ---- limits: int

/**************** INTERPRETER ****************/
/****** TOKENS ******/
// token types and display strings
typedef enum ttype {INTEGR, ARPLUS, ARMINS, ARMULT, ARDIVS, OPSEMI, ENDOFF, PARENL, PARENR, AREQUL, IDENTF, STRING} ttype;
static const char *ttypes[] = {
	"INTEGR", // numerical values --> 0 1 2 3 ...
	"ARPLUS", "ARMINS", "ARMULT", "ARDIVS", // arithmetic --> + - * /
	"OPSEMI", "ENDOFF", // end of line and end of file --> ; 0
	"PARENL", "PARENR", "AREQUL", // parentheses and equals --> ( ) =
	"IDENTF", "STRING" // identifier token (any string), and string --> puts "value"/'value'
};
// token structure
struct tokenData {
	int dType;
	int ivalue; // 0
	float fvalue; // 1
	char *svalue; // 2
};
typedef struct token {
	ttype type;
	struct tokenData *value;
} Token;

/****** SYNTAX TREE NODES ******/
struct DataNode {
	int nType;
	//int value;
	struct tokenData *value;
	struct DataNode *left;
	struct DataNode *right;
};
struct MultiNode {
	int nType;
	int statementl;
	struct DataNode *statements[256];
};

/***** GENERAL *****/
struct MultiNode actions;	// main program functions
char cchar;								// current char (for lexer)
Token token;							// current token (for parser)
char *src, *old_src;			// base file text
int poolsize;							// arbitrary max value
int line;									// current line
int linec;								// current character in line
LLItem *dhint = NULL;			// linked list dicitionary (for int variables)
int dflags[4] = { 0 };		// debug flags

// error handling
void error(int etype, char* emsg) {
	switch(etype) {
		case 1:
			printf("\x1b[31merror: %s, at line %d, char ~%d\x1b[0m\n", emsg, line, linec);
			exit(0);
			break;
		case 2:
			printf("\x1b[33mwarning: %s, at line %d, char ~%d\x1b[0m\n", emsg, line, linec);
			break;
		default:
			printf("\x1b[34mdebug: %s, at line %d, char ~%d\x1b[0m\n", emsg, line, linec);
			break;
	}
}


/****** LEXER --> Tokens ******/
void advance() {cchar = *src++;linec++;} // next char
void backtrack() {cchar = *src--;linec--;} // back char
char peek() {return *src+1;} // peek to next char

// create tokenData
struct tokenData* ciTD(int data) {
	struct tokenData *tData = (struct tokenData *) malloc (sizeof(struct tokenData));
	tData->dType = 0; tData->ivalue = data;
	return tData;
}

struct tokenData* cfTD(float data) {
	struct tokenData *tData = (struct tokenData *) malloc (sizeof(struct tokenData));
	tData->dType = 1; tData->fvalue = data;
	return tData;
}

struct tokenData* csTD(char *data) {
	struct tokenData *tData = (struct tokenData *) malloc (sizeof(struct tokenData));
	tData->dType = 2; tData->svalue = data;
	return tData;
}

// ignore some chars and advance line counter
void throwchars() {
	while (cchar == ' ' && cchar > 0 || cchar == '\n' && cchar > 0) {
		if (cchar == '\n') {line++;linec = 0;}
		advance();
	}
}

// convert 'xyz' of digits into integer
int numbers() {
	char res[] = "";
	while (isdigit(cchar) != 0 && cchar > 0 || cchar == '.' && cchar > 0) {
		strncat(res, &cchar, 1);
		advance();
	} backtrack();
	return atof(res);
}
// convert 'xyz' to identifier
char* id() {
	char *res = malloc(2 * sizeof(char)); // allocate memory

	int c = 0;
	while (isalpha(cchar) != 0 && cchar > 0) {
		res[c] = cchar;
		advance();
		c++;
		realloc(res, 2 + c);
	} backtrack();
	res[c] = '\0';

	return res;
}
// convert "xyz" to string
char* string(char initi) {
	char *res = malloc(2 * sizeof(char));

	int c = 0;
	advance();
	while (cchar != initi && cchar > 0) {
		res[c] = cchar;
		advance();
		c++;
		realloc(res, 2 + c);
	} res[c] = '\0';

	return res;
}

// get next token
void next() {
  advance();

	// Handle whitespace/newlines and eof
	if (cchar == ' ' || cchar == '\n') {throwchars();}
	if (cchar == 0) {
		token.type = ENDOFF;
		token.value = ciTD(0);
		return;
	}

	// Handle integers and return INTEGR type
	if (isdigit(cchar) != 0) {
		token.type = INTEGR;
		token.value = ciTD(numbers());
		return;
	}

	// Handle alpha and return IDENTF
	if (isalpha(cchar) != 0) {
		token.type = IDENTF;
		token.value = csTD(id());
		return;
	}

	// Handle strings
	if (cchar == '\'' || cchar == '\"') {
		token.type = STRING;
		token.value = csTD(string(cchar));
		return;
	}

	// Handle special characters and return type
	switch (cchar) {
		case '+':
			token.type = ARPLUS;
			token.value = ciTD('+');
			break;
		case '-':
			token.type = ARMINS;
			token.value = ciTD('-');
			break;
		case '*':
			token.type = ARMULT;
			token.value = ciTD('*');
			break;
		case '/':
			token.type = ARDIVS;
			token.value = ciTD('/');
			break;
		case '(':
			token.type = PARENL;
			token.value = ciTD('(');
			break;
		case ')':
			token.type = PARENR;
			token.value = ciTD(')');
			break;
		case '=':
			token.type = AREQUL;
			token.value = ciTD('=');
			break;
		case ';':
			token.type = OPSEMI;
			token.value = ciTD(';');
			break;
		default: // error case
			error(1, "token not identified in next()");
			break;
	}

  return;
}

/****** PARSER --> Syntax Tree ******/
struct DataNode *factor();
struct DataNode *term();
struct DataNode *expr();

// next token if token is of right type
void consume(ttype token_type) {
	if (dflags[0] == 1) printf("%s = %s\n", ttypes[token.type], ttypes[token_type]);
	if (token.type == token_type) {
		if (dflags[0] == 1) printf("\x1b[33mdebug: (%s, %d) --> ", ttypes[token.type], token.value);
		next();
		if (dflags[0] == 1) printf("(%s, %d)\x1b[0m\n", ttypes[token.type], token.value);
	} else {
		error(1, "token not eaten correctly in consume()");
	}
}

// Factor out unary operators and integers
struct DataNode *factor() {
	Token ctoken = token;
	struct DataNode *operation = (struct DataNode*) malloc (sizeof(struct DataNode));
	switch ((int)ctoken.type) {
		case 0:
			consume(INTEGR);
			operation->nType = 2;
			operation->value = ciTD(ctoken.value->ivalue);
			return operation;
			break;
		case 1: // ARPLUS
			consume(ARPLUS);
			operation->nType = 3;
			operation->value = ciTD('+');
			operation->right = factor();
			return operation;
			break;
		case 2: // ARMINS
			consume(ARMINS);
			operation->nType = 3;
			operation->value = ciTD('-');
			operation->right = factor();
			return operation;
			break;
		case 7: // LPAREN
			consume(PARENL);
			operation = expr();
			consume(PARENR);
			return operation;
			break;
		case IDENTF: // IDENTF
			consume(IDENTF);
			operation->nType = 5;
			operation->value = ctoken.value;
			return operation;
			break;
		default:
			error(1, "unrecognized symbol in factor()");
			break;
	}
}

// Operate multiplication and division terms
struct DataNode *term() {
	struct DataNode *node = factor();
	while (token.type == ARMULT || token.type == ARDIVS) {
		Token ctoken = token;
		switch ((int)ctoken.type) {
			case 3: // ARMULT
				consume(ARMULT);
				break;
			case 4: // ARDIVS
				consume(ARDIVS);
				break;
			default:
				error(1, "unrecognized symbol in term()");
				break;
		}

		struct DataNode *xnode = (struct DataNode*) malloc (sizeof(struct DataNode));
		xnode->nType = 1;
		xnode->left = node;
		xnode->value = ciTD(ctoken.value->ivalue);
		xnode->right = factor();
		node = xnode;
	}
	return node;
}

// Operate with plus and minus operators
struct DataNode *expr() {
	struct DataNode *node = term();
	while (token.type == ARPLUS || token.type == ARMINS) {
		Token ctoken = token;
		switch ((int)ctoken.type) {
			case 1: // ARPLUS
				consume(ARPLUS);
				break;
			case 2: // ARMINS
				consume(ARMINS);
				break;
			default:
				error(1, "unrecognized symbol in expr()");
				break;
		}

		struct DataNode *xnode = (struct DataNode*) malloc (sizeof(struct DataNode));
		xnode->nType = 1;
		xnode->left = node;
		xnode->value = ciTD(ctoken.value->ivalue);
		xnode->right = term();
		node = xnode;
	}
	return node;
}

// Construct and return string objects
struct DataNode *strconstr() {
	Token ctoken = token;
	struct DataNode *operation = (struct DataNode*) malloc (sizeof(struct DataNode));

	switch ((int)ctoken.type) {
		case STRING: // IDENTF
			consume(STRING);
			operation->nType = 7;
			operation->value = ctoken.value;
			return operation;
			break;
		default:
			error(1, "unrecognized symbol in strconstr()");
			break;
	}
}

// Build expression with strings
struct DataNode *strexpr() {
	struct DataNode *node = strconstr();

	while (token.type == ARPLUS) {
		Token ctoken = token;
		consume(ARPLUS);

		struct DataNode *xnode = (struct DataNode*) malloc (sizeof(struct DataNode));
		xnode->nType = 1;
		xnode->left = node;
		xnode->right = strconstr();
		xnode->value = ciTD('+');
		node = xnode;
	}

	return node;
}

// str/int difference
struct DataNode *strintdiff() {
	if (token.type == STRING) {
		return strexpr();
	} else {
		return expr();
	}
}

// parse lines/commands
void parser() {
  next();
	actions.nType = 1;
	actions.statementl = -1;
  while (cchar > 0) {
		// Arithemetic
		if (token.type == INTEGR) {
			struct DataNode *operat = expr();
			consume(OPSEMI);
			actions.statementl++;
			actions.statements[actions.statementl] = operat;
		}
		// Tokens
		if (token.type == IDENTF) {
			Token ctoken = token;
			struct DataNode *assign = (struct DataNode*) malloc (sizeof(struct DataNode));
			consume(IDENTF);

			// PUTS --> token
			if (strcmp(ctoken.value->svalue, "puts") == 0) {
				assign->nType = 6;
				assign->value = token.type == STRING ? ciTD(1) : ciTD(0);
				// Optional parentheses
				if (token.type == PARENL) {
					consume(PARENL);
					assign->right = strintdiff();
					consume(PARENR);
				} else {
					assign->right = strintdiff();
				}
			}

			// ASSIGNMENT --> token
			else if (token.type == AREQUL) {
				assign->nType = 4;
				assign->value = csTD(ctoken.value->svalue);
				consume(AREQUL);
				assign->left = 0;
				assign->right = strintdiff();
			}

			// ERROR
			else {
				error(1, "unknown identifier in parser()");
			}

			consume(OPSEMI);
			actions.statementl++;
			actions.statements[actions.statementl] = assign;
		}
		else {
			error(1, "unrecognized pattern in parser()");
		}
  }
	// error(0, "EOF reached");
}

/****** LEXER --> Program output ******/
int* mtraverse(struct MultiNode *node);
int dtraverse(struct DataNode *node);
char* dtraverses(struct DataNode *node);

// Traverse multi-statement
int* mtraverse(struct MultiNode *node) {
	int *outputs = (int *) malloc((node->statementl + 1) * sizeof(int));
	switch (node->nType) {
		case 1: // statements type
			for (int k = 0;k <= node->statementl;k++) outputs[k] = dtraverse(node->statements[k]);
			break;
		default:
			error(1, "unrecognized multi node type in mtraverse()");
			break;
	}
	return outputs;
}

// Traverse data nodes that return strings
char* dtraverses(struct DataNode *node) {
	LLValue * tv;
	char* sside;
	if (node->nType == 4) sside = dtraverses(node->right);
	if (node->nType == 5) tv = llget(dhint, node->value->svalue);
	switch (node->nType) {
		case 1:
			if (node->nType == 1) {
				/*char *k = (char *) malloc(256 * sizeof(char));
				memset(k, '\x00', 3);
				strcat(k, dtraverses(node->left));
				strcat(k, dtraverses(node->right));*/
				char *k = dtraverses(node->left);
				strcat(k, dtraverses(node->right));
				return k;
			}

			break;
		case 4: // set variable
			llappend(dhint, csLLV(sside), node->value->svalue);
			return sside;
			break;
		case 5: // get variable
			if (tv->llType == 2) {
				return tv->svalue;
			}
			break;
		case 7:
			return node->value->svalue;
			break;
		default:
			error(1, "unrecognized data node type in dtraverse()");
			break;
	}
}

// Traverse data nodes
int dtraverse(struct DataNode *node) {
	int dside;
	LLValue * tv;
	if (node->nType == 4) dside = dtraverse(node->right);
	if (node->nType == 5) tv = llget(dhint, node->value->svalue);
	switch (node->nType) {
		case 1: // operator type
			switch(node->value->ivalue) {
				case '+':
					return dtraverse(node->left) + dtraverse(node->right);
					break;
				case '-':
					return dtraverse(node->left) - dtraverse(node->right);
					break;
				case '*':
					return dtraverse(node->left) * dtraverse(node->right);
					break;
				case '/':
					return dtraverse(node->left) / dtraverse(node->right);
					break;
				default:
					printf("\x1b[31merror: unrecognized (data) node operator.\x1b[0m\n");
					break;
			}
			break;
		case 2: // num type
			return node->value->ivalue;
			break;
		case 3: // unary type
			switch(node->value->ivalue) {
				case '+':
					return +(dtraverse(node->right));
					break;
				case '-':
					return -(dtraverse(node->right));
					break;
				default:
					printf("\x1b[31merror: unrecognized (unary) node operator.\x1b[0m\n");
					break;
			}
			break;
		case 4: // set variable
			llappend(dhint, ciLLV(dside), node->value->svalue);
			return dside;
			break;
		case 5: // get variable
			if (tv->llType == 0) {
				return tv->ivalue;
			}
			if (tv->llType == 1) {
				return tv->fvalue;
			}
			break;
		case 6: // puts function
			if (node->value->ivalue == 0) {
				return printf("%d\n", dtraverse(node->right));
			}
			else {
				return printf("%s\n", dtraverses(node->right));
			}
			break;
		case 7: // string type
			return -1;
			break;
		default:
			error(1, "unrecognized data node type in dtraverse()");
			break;
	}
}

// Evaluate/traverse syntax tree
int eval() {
	int* outs = mtraverse(&actions);
	llpop(&dhint);
	if (dflags[1] == 1) llprintf(dhint);
	if (dflags[2] == 1) {
		for (int l = 0; l <= actions.statementl; l++)
			printf("\x1b[33m--> %d: \x1b[0m %d\n", l, *(outs + l));
	}

  free(outs);
	return 0;
}

/****** EXECUTION -> Get file contents and evaluate******/
// main
int main(int argc, char **argv) {
  int i;
  FILE *fd;

  argc--;
  argv++;

  poolsize = 256 * 1024;
  line = 1; linec = 1;
	dhint = (LLItem *) malloc(sizeof(LLItem));
	dhint->val = ciLLV(0);
	dhint->key = "RESERVED";
	dhint->next = NULL;

  /* Handle file input */
  if ((fd = fopen(*argv, "r")) == NULL) {
    printf("could not fopen(%s)\n", *argv);
    return -1;
  }

  fseek(fd, 0, SEEK_END);
  long fsize = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  if (!(src = old_src = malloc(poolsize))) {
    printf("could not malloc(%d) for source area\n", poolsize);
    return -1;
  }
  if ((i = fread(src, 1, fsize, fd)) <= 0) {
    printf("fread() returned %d\n", i);
    return -1;
  }

  src[i] = 0;
  fclose(fd);
  /* End of file input */

	// if (argc >= 2) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(*(argv+i), "--debug") == 0)
			dflags[0] = 1;
		if (strcmp(*(argv+i), "--int") == 0)
			dflags[1] = 1;
		if (strcmp(*(argv+i), "--line") == 0)
			dflags[2] = 1;
	}

  parser();
  return eval();
}
