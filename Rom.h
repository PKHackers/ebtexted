// Rom.h: interface for the CRom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROM_H__82AD4F92_0241_40D0_AEEB_43F9C6DF5A53__INCLUDED_)
#define AFX_ROM_H__82AD4F92_0241_40D0_AEEB_43F9C6DF5A53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "textstuff.h"


class CRom  
{
public:
	CRom();
	virtual ~CRom();


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

};

#endif // !defined(AFX_ROM_H__82AD4F92_0241_40D0_AEEB_43F9C6DF5A53__INCLUDED_)
