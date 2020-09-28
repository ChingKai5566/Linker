#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <map>
#include <set>

using namespace std;

void tokenizer(string filename);
int readInt();
string readSymbol();
bool isValidSymbol(string sym);
bool isValid(char ch);
void moveToToken();
void pass1(string filename);
void pass2(string filename);
char readIEAR();
void parseError(int errcode);
int readAddr();
int calculateDigit(int num);
void printMemoryTable(int addr, int instr);

struct Module
{
  int len;
  int start;
  map<string, int> symToVal;
};
