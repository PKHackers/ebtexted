/////////////////////////////////////////////////////////////////////////////////////////////////
// textstuff.cpp: text manipulation functions

#include "stdafx.h"
#include "textstuff.h"


int LoadRawText(FILE* rom, String strlist[], CCNode ccTable, char* comprTable[])
{
	int address = 0x51D12;
	int istr; istr = 0;

	do
	{
		if(address < 0x0A012E || address >= 0x2F5020)
		{
			if(address < 0x08BE2D || address >= 0x8DC31)
			{
				String s = ReadString(rom, address, ccTable);

				strlist[istr] = s;

				address+=s.len;
				istr++;
			}
			else
				address = 0x8DC31;
		}
		else
			address = 0x2F5020;
	}while(address < 0x2FA57A);

	return istr;
}

int LoadEXPText(FILE *rom, String strlist[], CCNode ccTable)
{
	int address = 0x300200;
	int istr; istr = 0;

	do
	{
		String s = ReadString(rom, address, ccTable);

		strlist[istr] = s;

		address+=s.len;
		istr++;

	}while(address < 0x400200);

	return istr;
}



int LoadTPTText(FILE* rom, String strlist[], CCNode ccTable)
{
	int offset = 0;

	int istr = 0;

	int address;

	for(int i = 0; i < 1583; i++)
	{
		fseek(rom, 0xF8B9F+offset, SEEK_SET);

		address = fgetc(rom);
		address+= fgetc(rom) << 8;
		address+= fgetc(rom) << 16;

		if(address != 0)
		{
			String s = ReadString(rom, SNESToHex(address), ccTable);
			strlist[istr++] = s;
		}

		offset+=0x11;
	}
	return istr;
}


String ReadString(FILE* rom, unsigned long address, CCNode ccTable)
{
	// Reads a string from ROM at address.
	// Returns a struct containing a pointer to the string and
	// the string's length. This function does not do any formatting
	// or decompression. If the string is greater than 8192 characters
	// long, it will be truncated at that point.

	char buffer[8192];

	int pos = 0;
	int len = 0;

	CCNode* activeNode = &ccTable;

	bool opcode = false;
	int arglevel = 0;

	unsigned char ch;

	fseek(rom, address, SEEK_SET);

	for(int i = 0; i < 8192; i++)
	{
		ch = fgetc(rom);
		len++;

		if(arglevel > 0)
		{
			buffer[pos++] = ch;

			arglevel--;
			continue;
		}

		if(!opcode)
		{
			if(isCC(ch))
			{
				opcode = true;
			}
		}

		if(opcode)
		{
			int n = SearchNode(activeNode, ch);
			if(n == -1)
			{
				opcode = false;
				activeNode = &ccTable;
				buffer[pos++] = ch;
				continue;				
			}
			else
			{
				activeNode = &activeNode->nodes[n];
			}
			
			if(activeNode->type == TYPE_ARGLIST)
			{
				arglevel = activeNode->num_arg-1;
				buffer[pos++] = ch;
			}
			else if(activeNode->type == TYPE_CODE)
			{
				buffer[pos++] = ch;
			}

			if(activeNode->level == 0 && activeNode->value == 0x02)
				break;

			if(activeNode->isTerminator)
			{
				opcode = false;
				activeNode = &ccTable;
			}
		}
		else
		{
			buffer[pos++] = ch;
		}
	}

	buffer[pos++] = 0;

	String s;
	s.address = address;
	s.str = new char[pos];
	s.len = len;
	
	for(i = 0; i < pos; i++)
		s.str[i] = buffer[i];

	return s;
}



int WriteString(FILE* rom, String s, int address)
{
	fseek(rom, address, SEEK_SET);

	for(int i = 0; i < s.len; i++)
	{
		fputc(s.str[i], rom);
	}
	fflush(rom);

	return 0;
}







char* ParseString(char str[], int len, CCNode ccTable, char* comprTable[])
{
	char buffer[8192];

	int pos = 0;	// position in returned string
	int ipos = 0;

	CCNode* activeNode = &ccTable;

	bool opcode = false;
	int arglevel = 0;

	int curcode;		// initial byte of current control code

	unsigned char ch;

	for(int i = 0; i < len; i++)
	{
		ch = str[ipos++];
		//len++;

		// if there are arguments left to count, decrement arg level and continue
		if(arglevel > 0)
		{
			AddCode(buffer, ch, pos);
			pos+=4;

			arglevel--;
			continue;
		}

		// if opcode was not previously set
		if(!opcode)
		{
			if(isCC(ch))
			{
				curcode = ch;
				opcode = true;
			}
		}


		// do more stuff

		// if opcode is true, ...

		// note that none of this will happen when there are arguments in the queue,
		// so the active node will still be the original argument node when the arguments
		// finally close, so everything will proceed in an orderly fashion.
		// (an argument node will just run through a few extra bytes and add them to the string,
		// whereas other nodes involve adding only one character to the string)

		if(opcode)
		{

			// search for a subnode matching the current byte
			// (as of initial iteration, activeNode is the root node)
			int n = SearchNode(activeNode, ch);
			if(n == -1)
			{
				// if no match was found, this is an invalid code
				opcode = false;
				activeNode = &ccTable;
			}
			else
			{
				// set the active node to a subnode of itself
				activeNode = &activeNode->nodes[n];
			}
			

			// okay, now we have a byte from the ROM, and have associated it
			// with a node. depending on the node type, do stuff

			if(activeNode->type == TYPE_ARGLIST)
			{
				// set arg level to num_arg - 1 because we've already read
				// one of the argument bytes
				arglevel = activeNode->num_arg-1;

				if(curcode == 0x15 || curcode == 0x16 || curcode == 0x17)
				{
					int c = ((curcode-0x15)*256)+ch;

					int p = 0;

					while(comprTable[c][p] != 0)
					{
						buffer[pos++] = comprTable[c][p++];
					}
				}
				else
				{
					AddCode(buffer, ch, pos);
					pos+=4;
				}
			}
			if(activeNode->type == TYPE_CODE)
			{
				// only add a bracketed code if the current code is not a compression code
				if(curcode != 0x15 && curcode != 0x16 && curcode != 0x17)
				{
					AddCode(buffer, ch, pos);
					pos+=4;
				}
			}



			// if the current node is a first-byte code with value 2, terminate text block
			if(activeNode->level == 0 && activeNode->value == 0x02)
				break;


			// if the active node is a terminator node,  stop stuff
			if(activeNode->isTerminator)
			{
				opcode = false;

				// reset the active node so we start from the beginning
				activeNode = &ccTable;
			}
		}
		else
		{
			// if the current byte is not a control code,
			// add it to the string as-is
			buffer[pos++] = ch-0x30;
		}
	}

	buffer[pos++] = 0;

	char* newstr = new char[pos+1];
	for(i = 0; i < pos; i++)
		newstr[i] = buffer[i];

	newstr[pos] = 0;

	return newstr;
}


char* ParseHeader(char str[], int len, CCNode ccTable, char* comprTable[])
{
	char buffer[8192];

	int pos = 0;
	int ipos = 0;

	CCNode* activeNode = &ccTable;

	bool opcode = false;
	int arglevel = 0;

	int curcode;

	unsigned char ch;

	for(int i = 0; i < len; i++)
	{
		ch = str[ipos++];

		if(arglevel > 0)
		{
			arglevel--;
			continue;
		}

		if(!opcode)
		{
			if(isCC(ch))
			{
				curcode = ch;
				opcode = true;
			}
		}

		if(opcode)
		{
			int n = SearchNode(activeNode, ch);
			if(n == -1)
			{
				opcode = false;
				activeNode = &ccTable;
			}
			else
			{
				activeNode = &activeNode->nodes[n];
			}
			
			if(activeNode->type == TYPE_ARGLIST)
			{
				arglevel = activeNode->num_arg-1;

				if(curcode == 0x15 || curcode == 0x16 || curcode == 0x17)
				{
					int c = ((curcode-0x15)*256)+ch;

					int p = 0;

					while(comprTable[c][p] != 0)
					{
						buffer[pos++] = comprTable[c][p++];
					}
				}
			}

			if(activeNode->level == 0 && activeNode->value == 0x02)
				break;

			if(activeNode->isTerminator)
			{
				opcode = false;
				activeNode = &ccTable;
			}
		}
		else
		{
			buffer[pos++] = ch-0x30;
		}
	}

	buffer[pos++] = 0;

	char* newstr = new char[pos+1];
	for(i = 0; i < pos; i++)
		newstr[i] = buffer[i];

	newstr[pos] = 0;

	return newstr;
}


char* ParseGrouping(char str[])
{
	int pos = 0;

	int len = strlen(str);

	do
	{
		char* s = strstr(str, "][");

		if(s == NULL)
		{
			break;
		}
		else
		{
			pos = s - str;

			str[pos] = ' ';

			for(int j = pos+1; j < len; j++)
			{
				str[j] = str[j+1];
			}
		}
	}while(pos != -1);

	return str;
}


int GetStringLength(char str[])
{
	// This is similar to DeparseString (below);
	// but this function does not deparse the string, it just counts
	// the number of characters.
	// Naturally, the value returned represents the string's un-
	// compressed length.
	// (xlen is the length of the code-formatted string)
	// (Returns -1 if a bracket error is encountered)

	int len = 0;
	int bracketlevel = 0;
	int ch;

	int xlen = strlen(str);

	for(int i = 0; i < xlen; i++)
	{
		if(str[i] == '\0')
		{
			CString x;
			x.Format("%d", i);
			AfxMessageBox(x);
			break;
		}

		if(str[i] == '[')
		{
			bracketlevel++;
			continue;
		}

		if(str[i] == ']')
		{
			bracketlevel--;
			continue;
		}

		if(bracketlevel < 0) {
			return -1;
		}


		if(bracketlevel > 0)
		{
			int j = 0;

			do
			{
				ch = str[i++];

				if(ch == ']')
				{
					len++;
					j = 0;
					i-=2;
					break;
				}

				if(ch == ' ' || ch == ']')
				{
					if(j != 0)
					{
						len++;
						j = 0;
					}

				}
				else
					j++;

				if(j == 32) break;

				
			}while(ch != ']');
		}

		else
		{
			len++;
		}

	}

	if(bracketlevel != 0)
	{
		return -1;
	}

	return len;
}


char* DeparseString(char str[])
{
	// This function takes a visually formatted string (like the user
	// input into the text editing box) and converts it into a string
	// that is ready to be written directly to the ROM.

	char buffer[8192];
	char *newstr;	// this gets allocated later

	int pos = 0;

	int bracketlevel = 0;

	int ch;

	int len = strlen(str);

	for(int i = 0; i < len; i++)
	{
		if(str[i] == '[')
		{
			bracketlevel++;
			continue;
		}

		if(str[i] == ']')
		{
			bracketlevel--;
			continue;
		}

		if(bracketlevel < 0) {
			AfxMessageBox("WARNING: Bracket error detected.");
			return NULL;
		}

		// if we're still inside brackets (if bracketlevel > 0),
		// get everything up to and including the next space, or
		// up to the next closing bracket.

		if(bracketlevel > 0)
		{
			char t[32];
			int j = 0;

			do
			{
				ch = str[i++];

				if(ch == ']')
				{
					t[j] = 0;

					int a = strtoul(t, NULL, 16)&0xFF;
					buffer[pos++] = a;

					j = 0;
					i-=2;
					break;
				}

				if(ch == ' ' || ch == ']')
				{
					if(j != 0)
					{
						t[j] = 0;

						int a = strtoul(t, NULL, 16)&0xFF;
						buffer[pos++] = a;

						j = 0;
					}

				}
				else
					t[j++] = ch;

				if(j == 32) break;

				
			}while(ch != ']');
		}

		else
			buffer[pos++] = str[i]+0x30;

	}

	if(bracketlevel != 0)
	{
		AfxMessageBox("WARNING: Bracket error detected.");
		return NULL;
	}


	buffer[pos++] = 0;

	newstr = new char[pos];
	for(i = 0; i < pos; i++)
		newstr[i] = buffer[i];

	return newstr;
}

int SearchNode(CCNode *node, int val)
{
	// note: if there's only one subnode of the specified node,
	// and it's an argument list, return its index regardless of
	// the specified value.

	if(node->num_nodes == 1 && node->nodes[0].type == TYPE_ARGLIST)
	{
		return 0;
	}

	int retVal = -1;
	for(int i = 0; i < node->num_nodes; i++)
	{
		if(node->nodes[i].value == val)
			retVal = i;
	}

	return retVal;
}


bool isCC(char ch)
{
	bool retVal = false;

	if(ch >= 0 && ch <= 0x1F)
		retVal = true;

	return retVal;
}

void AddCode(char* str, int val, int pos)
{
	char b[5];
	if(val < 0x10)
		sprintf(b, "[0%X]", val);
	else
		sprintf(b, "[%X]", val);

	str[pos++] = b[0];
	str[pos++] = b[1];
	str[pos++] = b[2];
	str[pos++] = b[3];
}



void CreateCompressionTable(FILE* rom, char* tbl[])
{
	// loads compression strings from rom.
	// assumes tbl is allocated with 768 pointers to strings

	char buffer[256];
	int adr;
	char ch;
	int pos;

	for(int i = 0; i < 768; i++)
	{
		fseek(rom, 0x8CFED+(i*4), SEEK_SET);

		pos = 0;

		adr = fgetc(rom);
		adr+= fgetc(rom) << 8;
		adr+= fgetc(rom) << 16;

		fseek(rom, SNESToHex(adr), SEEK_SET);
		for(int j = 0; j < 256; j++)
		{
			ch = fgetc(rom);
			buffer[pos++] = ch;
			if(ch == 0) break;
		}

		tbl[i] = new char[pos+1];

		for(j = 0; j < pos+1; j++)
			tbl[i][j] = buffer[j]==0 ? 0 : buffer[j]-0x30;
	}
}


void CreateCCTable(CCNode *table)
{
	FILE* file;
	char buffer[1024];
	unsigned char codestr[1024];
	unsigned char nodestr[1024];
	unsigned char descstr[1024];
	int bpos = 0;
	int pos = 0;
	int len = 0;

	file = fopen("codelist.txt", "r");

	if(!file) AfxMessageBox("Holy crap and a half!");

	if(file)
	{
		fseek(file, 0, SEEK_SET);

		while(!feof(file))
		{
			fgets(buffer, 1024, file);

			while(buffer[bpos] != ',' && bpos < 1024)
			{
				if(buffer[bpos] != ' ')
				{
					codestr[pos++] = buffer[bpos];
				}
				bpos++;
			}
			codestr[pos] = 0;
			len = pos;
			pos = 0;
			bpos++;

			while(buffer[bpos] != ';' && bpos < 1024)
			{
				if(buffer[bpos] != '"')
				{
					descstr[pos++] = buffer[bpos];
				}
				bpos++;
			}
			descstr[pos] = 0;
			bpos = pos = 0;

			//printf("%s\t:\t", codestr);

			// now convert the string into a series of single bytes
			for(int i = 0; i < len; i)
			{
				char t[3];
				t[0] = codestr[i++];
				t[1] = codestr[i++];
				t[2] = '\0';

				nodestr[pos++] = (unsigned char)strtoul(t, 0, 16);
			}
			nodestr[pos] = 0;
			len = pos;
			pos = 0;


			//printf("%s\n", nodestr);

			// now loop through the code string and add a series of nodes

			int prev = 0;
			bool arg_counting = false;
			int arg_count = 0;
			int args[32];

			CCNode* activeNode = table;
			int nodeLevel = 0;

			for(i = 0; i < len; i++)
			{
				//printf("%X: ", nodestr[i]);
				// if we encounter an argument value (0xAA, 0xBB, etc.),
				// start counting arguments

				// stop counting when the last code no is no longer equal to the current
				if(arg_counting)
				{
					if(prev != nodestr[i])
					{
						//printf("STOP! %d arguments!\n", arg_count);

						// now add latent node
						activeNode = AddNode(activeNode, nodeLevel, nodestr[i-1], TYPE_ARGLIST, arg_count, false, false);
						nodeLevel++;
						arg_count = 0;
						arg_counting = false;
					}
				}
				if((nodestr[i] - 0xAA)%0x11 == 0 && nodestr[i] != 0 && !arg_counting)
				{
					//printf("COUNTING ARGUMENTS!\n");
					arg_counting = true;
				}

				if(arg_counting)
				{
					// if we're still counting arguments, increment the argument count
					// and add the current value to the argument list
					args[arg_count] = nodestr[i];
					arg_count++;
				}
				else
				{
					// add default code node here
					activeNode = AddNode(activeNode, nodeLevel, nodestr[i], TYPE_CODE, NULL, false, (i+1 == len));
					nodeLevel++;
					//printf("\n");
				}
				
				
				prev = nodestr[i];
			}
			if(arg_counting)
			{
				// if we're still counting arguments, tidy up
				
				//printf("STOP! %d arguments!\n", arg_count);

				AddNode(activeNode, nodeLevel, nodestr[i-1], TYPE_ARGLIST, arg_count, false, true);

				arg_counting = false;
				arg_count = 0;
			}

			//printf("\n");
		}
	}
}


CCNode* AddNode(CCNode *node, int level, int value, int type, int num_arg, bool multi, bool terminate)
{
	// Adds a subnode with specified properties to the provided node

	// if a node of the specified value already exists, return a pointer to it
	for(int i = 0; i < node->num_nodes; i++)
	{
		if(node->nodes[i].value == value)
			return &node->nodes[i];
	}


	CCNode *temp = new CCNode[node->num_nodes];

	for(i = 0; i < node->num_nodes; i++)
		temp[i] = node->nodes[i];

	node->nodes = new CCNode[node->num_nodes+1];

	for(i = 0; i < node->num_nodes; i++)
		node->nodes[i] = temp[i];

	int n = node->num_nodes;

	node->nodes[n].level = level;
	node->nodes[n].value = value;
	node->nodes[n].type = type;
	node->nodes[n].num_arg = num_arg;
	node->nodes[n].arg_multiplier = multi;
	node->nodes[n].isTerminator = terminate;

	node->nodes[n].num_nodes = 0;

	delete temp;
	node->num_nodes++;

	// return a pointer to the node we just created
	return &node->nodes[n];

}









/* UTILITY FUNCTIONS */

unsigned int HexToSNES(unsigned int address)
{
   // Converts good-ol regular hex addresses into the SNES-style addresses.
   // In this case, it's assumed the ROM is a hi-rom with a header.

   unsigned int retVal;

   retVal = (address + 0xc00000) - 0x200;

   return retVal;
}

unsigned int SNESToHex(unsigned int address)
{
   // Converts an SNES-style address (hi-rom, with header) to a nice
   // handy hex address.

   unsigned int retVal;

   retVal = (address - 0xc00000) + 0x200;

   return retVal;
}


bool CheckFile(CString szFile)
{
	// This function checks a file against various
	// criteria to make sure it is valid before loading.
	// There's the basic stuff, like making sure the file
	// can be opened and written to, or check to make sure
	// it's not a folder or some stupid thing like that.

	FILE* stream;
	char str[12];
	int idiot;

	stream = fopen(szFile, "rb+");

	if(stream) {
		fseek(stream, 0x0101C0, SEEK_SET);
		for(int i = 0; i < 11; i++)
			str[i] = fgetc(stream);
		str[11] = 0;
		if(strcmp(str, "EARTH BOUND"))
			idiot = AfxMessageBox("This doesn't look like an EarthBound ROM. Continuing may cause damage to your ROM. Do you want to continue anyway?", MB_YESNO | MB_ICONQUESTION);
		else
			return true;
		if(idiot == IDYES)
			return true;

		fclose(stream);
	}
	else {
		AfxMessageBox("An error occurred while attempting to open this file. Check to make sure the file is not read-only, and is not currently in use by another application.");
	}

	return false;
}