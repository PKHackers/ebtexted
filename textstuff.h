/////////////////////////////////////////////////////////////////////////////////////////////////
// textstuff.h: definitions for stuff :D


#define TYPE_CODE 0
#define TYPE_ARGLIST 1


struct CCNode {
	int level;		// depth, i.e., CC byte # (sorta)
	int value;		// byte value (-1 for arguments)

	int type;		// code byte or argument
	int num_arg;	// number of argument bytes
	int args[32];	// argument list
	bool arg_multiplier;	// if true, number of arguments remaining is multiplied by the
							// value of the first

	bool isTerminator;

	int num_nodes;
	CCNode *nodes;
};

struct String {
	int address;

	char* str;
	int len;
};



void CreateCCTable(CCNode table[32]);
CCNode* AddNode(CCNode *node, int level, int value, int type, int num_arg, bool multi, bool terminate);

void CreateCompressionTable(FILE* rom, char* tbl[]);


String ReadString(FILE* rom, unsigned long address, CCNode ccTable);
char* ParseString(char str[], int len, CCNode ccTable, char* comprTable[]);
char* ParseHeader(char str[], int len, CCNode ccTable, char* comprTable[]);
char* ParseGrouping(char str[]);

int GetStringLength(char str[]);
char* DeparseString(char str[]);

int WriteString(FILE* rom, String s, int address);

int SearchNode(CCNode *node, int val);
void AddCode(char* str, int val, int pos);
bool isCC(char ch);

int LoadRawText(FILE* rom, String strlist[], CCNode ccTable, char* comprTable[]);
int LoadEXPText(FILE *rom, String strlist[], CCNode ccTable);
int LoadTPTText(FILE* rom, String strlist[], CCNode ccTable);


bool CheckFile(CString szFile);

unsigned int HexToSNES(unsigned int);
unsigned int SNESToHex(unsigned int);