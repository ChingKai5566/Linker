#include "Linker.hpp"

/**
 * Global variable
 */
static ifstream infile;
static int line;
static int offset;
static unordered_map<string, int> symToVal; // definition list

static string errstr[] = {
    "NUM_EXPECTED",           // Number expect
    "SYM_EXPECTED",           // Symbol expected
    "ADDR_EXPECTED",          // Addressing Expected which is A/E/I/R
    "SYM_TOO_LONG",           // Symbol Name is too long
    "TOO_MANY_DEF_IN_MODULE", // > 16
    "TOO_MANY_USE_IN_MODULE", // > 16
    "TOO_MANY_INSTR"};        // total num_instr exceeds memory size (512)

/**
 * tokenizer
 * @param name input file name
 */
void tokenizer(string filename)
{
  pass1(filename);
}

/**
 * first time go through the text, check syntax and grammer error
 * @param name input file name
 */
void pass1(string filename)
{
  infile.open(filename);
  int len = 0;
  line = 1;
  offset = 0;

  while (!infile.eof())
  {
    // definition list
    int defCount = readInt();

    if (defCount == -1)
    {
      break;
    }

    for (int i = 0; i < defCount; i++)
    {
      string sym = readSymbol();
      int val = readInt();
      symToVal[sym] = val + len;
    }

    // use list
    int useCount = readInt();

    for (int i = 0; i < useCount; i++)
    {
      string sym = readSymbol();
    }

    // program text
    int codeCount = readInt();

    for (int i = 0; i < codeCount; i++)
    {
      char addressMode = readIEAR();
      int operand = readInt();
    }

    len += codeCount;
  }

  print_map(symToVal);

  infile.close();
}

/**
 * read next integer
 */
int readInt()
{
  // find next valid char
  moveToToken();

  // last run will be here
  if (infile.eof())
  {
    return -1;
  }

  // calculate defcount
  int defcount = 0;
  char c;
  while (isValid(infile.peek()))
  {
    try
    {
      infile.get(c);
      offset++;
      if (c < '0' || c > '9')
      {
        throw(0);
      }
      defcount = defcount * 10 + (c - '0');
    }
    catch (int errCode)
    {
      parseError(errCode);
      exit(0);
    }
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
    offset++;
    symbol += c;
  }

  return symbol;
}

/**
 * read IEAR address mode
 */
char readIEAR()
{
  // find next valid char
  moveToToken();

  // get char
  char c;
  infile.get(c);
  offset++;

  return c;
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
    offset++;

    if (c == '\n')
    {
      line++;
      offset = 0;
    }

    // check file status
    if (infile.eof())
    {
      break;
    }
  }
}

void parseError(int errcode)
{
  cout << "Parse Error line " << line << " offset " << offset << ": " << errstr[errcode] << endl;
}

/**
 * program start from here
 */
int main(int argc, char *argv[])
{
  tokenizer(argv[1]);

  return 0;
}
