#include "Linker.hpp"

/**
 * Global variable
 */
ifstream infile;
unordered_map<string, int> symbolToVal;

/**
 * tokenizer
 * @param name input file name
 */
void tokenizer(string filename)
{
  infile.open(filename);

  while (!infile.eof())
  {
    int defcount = readInt();

    for (int i = 0; i < defcount; i++)
    {
      string sym = readSymbol();
      int val = readInt();
      symbolToVal[sym] = val;
      print_map(symbolToVal);
    }

    break;
  }

  infile.close();
}

/**
 * read next integer
 */
int readInt()
{
  // find next valid char
  moveToToken();

  // calculate defcount
  int defcount = 0;
  char c;
  while (isValid(infile.peek()))
  {
    infile.get(c);
    defcount = defcount * 10 + (c - '0');
  }

  return defcount;
}

/**
 * read next symbol
 * symbol: [a-Z][a-Z0-9]*, up to 16 characters
 */
string readSymbol()
{
  // find next valid char
  moveToToken();

  // build symbol
  string symbol;
  char c;
  while (isValid(infile.peek()))
  {
    infile.get(c);
    symbol += c;
  }

  return symbol;
}

/**
 * check the Symbol is valid
 */
bool isValidSymbol(string sym)
{
  if (sym.length() > 16)
  {
    return false;
  }

  regex reg("[a-zA-Z][a-zA-Z0-9]*");
  return regex_match(sym, reg);
}

/**
 * check the character is valid
 *
 * @param character
 */
bool isValid(char ch)
{
  return ch != ' ' && ch != '\n' && ch != '\t';
}

/**
 * jump to next valid character
 */
void moveToToken()
{
  char c;
  while (!isValid(infile.peek()))
  {
    infile.get(c);
  }
}

/**
 * program start from here
 */
int main(int argc, char *argv[])
{
  tokenizer(argv[1]);

  return 0;
}
