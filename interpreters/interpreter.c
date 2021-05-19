#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define int long long

/****** LINKED LISTS ******/
// adapted: https://www.learn-c.org/en/Linked_lists
typedef struct ll_node {
	char *key;
	int val;
	struct ll_node * next;
} LLItem;

// Print linked list
void llprintf(LLItem * head) {
    LLItem * current = head;

    while (current != NULL) {
        printf("%s : %d\n", current->key, current->val);
        current = current->next;
    }
}

// Get item by key
int llget(LLItem * head, char* key) {
	int retval = -1;
	LLItem * current = head;
	while (current->next != NULL && strcmp(current->key, key) != 0) {
		current = current->next;
	}

	return current->val;
}

// Add item to end of list
void llappend(LLItem * head, int val, char* key) {
	LLItem * current = head;
	while (current->next != NULL) {
		current = current->next;
	}

	current->next = (LLItem *) malloc(sizeof(LLItem));
	current->next->val = val;
	current->next->key = key;
	current->next->next = NULL;
}

// Add item to beginning of list
void llpush(LLItem ** head, int val, char* key) {
	LLItem * new_node;
	new_node = (LLItem *) malloc(sizeof(LLItem));
	new_node->val = val;
	new_node->key = key;
	new_node->next = *head;
	*head = new_node;
}

// Remove first item returns value
int llpop(LLItem ** head) {
    int retval = -1;
    LLItem * next_node = NULL;

    if (*head == NULL) {
        return -1;
    }

    next_node = (*head)->next;
    retval = (*head)->val;

    free(*head);
    *head = next_node;

    return retval;
}

// Remove last item returns value
int llremlast(LLItem * head) {
    int retval = 0;
    /* if there is only one item in the list, remove it */
    if (head->next == NULL) {
        retval = head->val;
        free(head);
        return retval;
    }

    /* get to the second to last node in the list */
    LLItem * current = head;
    while (current->next->next != NULL) {
        current = current->next;
    }

    /* now current points to the second to last item of the list, so let's remove current->next */
    retval = current->next->val;
    free(current->next);
    current->next = NULL;
    return retval;
}

// Remove by index returns value
int llremindex(LLItem ** head, int n) {
    int i = 0;
    int retval = -1;
    LLItem * current = *head;
    LLItem * temp_node = NULL;

    if (n == 0) {
        return llpop(head);
    }

    for (i = 0; i < n-1; i++) {
        if (current->next == NULL) {
            return -1;
        }
        current = current->next;
    }

    temp_node = current->next;
    retval = temp_node->val;
    current->next = temp_node->next;
    free(temp_node);

    return retval;

}



/**************** INTERPRETER ****************/
/****** TOKENS ******/
// token types and display strings
typedef enum ttype {INTEGR, ARPLUS, ARMINS, ARMULT, ARDIVS, OPSEMI, ENDOFF, PARENL, PARENR, AREQUL, IDENTF} ttype;
static const char *ttypes[] = {
	"INTEGR", "ARPLUS", "ARMINS", "ARMULT", "ARDIVS", "OPSEMI", "ENDOFF", "PARENL", "PARENR", "AREQUL", "IDENTF"
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
int debugm;								// debug

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
// convert 'xyz' to string
char* id() {
	static char res[] = "";
	while (isalpha(cchar) != 0 && cchar > 0) {
		strncat(res, &cchar, 1);
		advance();
	} backtrack();

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

		printf("NEXT:\n");
		int c = 0;
		while (isalpha(cchar) != 0 && cchar > 0) {
			c++;
			advance();
		} backtrack();

		printf("\n");
		for (int i = 0; i <= c; i++) {
			backtrack();
		} advance();
		printf("\n");
		char *res = malloc(c-1);
		for (int i = 0; i < c; i++) {
			advance();
			res[i] = cchar;
		}
		printf("\n");

		/*char *res = (char *) malloc(0);
		int size = 0;

		while (isalpha(cchar) != 0 && cchar > 0) {
			size++;
			realloc(res, size);
			res[size-1] = cchar;
			// strncat(res, &cchar, 1);
			advance();
		} backtrack();*/

		token.value = csTD(res);
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
	if (debugm == 1) printf("%s = %s\n", ttypes[token.type], ttypes[token_type]);
	if (token.type == token_type) {
		if (debugm == 1) printf("\x1b[33mdebug: (%s, %d) --> ", ttypes[token.type], token.value);
		next();
		if (debugm == 1) printf("(%s, %d)\x1b[0m\n", ttypes[token.type], token.value);
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
			struct DataNode *operation = expr();
			consume(PARENR);
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

// parse lines/commands
void parser() {
  next();
	actions.nType = 1;
	actions.statementl = -1;
  while (cchar > 0) {
		if (token.type == INTEGR) {
			struct DataNode *operat = expr();
			consume(OPSEMI);
			actions.statementl++;
			actions.statements[actions.statementl] = operat;
		}
		else if (token.type == IDENTF) {
			struct DataNode *assign = (struct DataNode*) malloc (sizeof(struct DataNode));
			assign->nType = 4;
			Token ctoken = token;
			consume(IDENTF);
			assign->value = csTD(ctoken.value->svalue);
			consume(AREQUL);
			assign->left = 0;
			assign->right = expr();
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

// Traverse multi-statement
int* mtraverse(struct MultiNode *node) {
	int *outputs = malloc(node->statementl);
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

// Traverse data nodes
int dtraverse(struct DataNode *node) {
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
		case 4:
			printf("Var: %s = %d\n", node->value->svalue, dtraverse(node->right));
			return dtraverse(node->right);
			break;
		default:
			error(1, "unrecognized data node type in dtraverse()");
			break;
	}
}

// Evaluate/traverse syntax tree
int eval() {
	int* outs = mtraverse(&actions);
	for (int l = 0; l <= actions.statementl; l++) {
		printf("\n\x1b[33m--> %d: \x1b[0m %d", l, *(outs + l));
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
	dhint->val = -1;
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

	if (argc >= 2)
		if (strcmp(*(argv+1), "--debug") == 0)
			debugm = 1;

  parser();
  return eval();
}
