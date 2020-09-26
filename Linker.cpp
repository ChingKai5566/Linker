#include "Linker.hpp"

/**
 * Global variable
 */
static ifstream infile;
static int line;
static int offset;
static map<string, int> symToVal; // definition list

static string errstr[] = {
    "NUM_EXPECTED",           // Number expect
    "SYM_EXPECTED",           // Symbol expected
    "ADDR_EXPECTED",          // Addressing Expected which is A/E/I/R
    "SYM_TOO_LONG",           // Symbol Name is too long
    "TOO_MANY_DEF_IN_MODULE", // > 16
    "TOO_MANY_USE_IN_MODULE", // > 16
    "TOO_MANY_INSTR"          // total num_instr exceeds memory size (512)
};

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

    if (defCount > 16)
    {
      offset -= (calculateDigit(defCount) - 1);
      parseError(4);
    }

    // finish parsing
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

    if (useCount == -1)
    {
      parseError(0);
    }

    for (int i = 0; i < useCount; i++)
    {
      string sym = readSymbol();
    }

    // program text
    int codeCount = readInt();

    for (int i = 0; i < codeCount; i++)
    {
      char addressMode = readIEAR();
      int operand = readAddr();
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
  int count = 0;
  char c;

  while (isValid(infile.peek()))
  {
    infile.get(c);
    offset++;
    if (c < '0' || c > '9')
    {
      parseError(0);
    }
    count = count * 10 + (c - '0');
  }

  return count;
}

/**
 * read next address
 */
int readAddr()
{
  // find next valid char
  moveToToken();

  // last run will be here
  if (infile.eof())
  {
    parseError(2);
  }

  // calculate defcount
  int count = 0;
  char c;
  while (isValid(infile.peek()))
  {

    infile.get(c);
    offset++;
    if (c < '0' || c > '9')
    {
      parseError(2);
    }
    count = count * 10 + (c - '0');
  }

  return count;
}

/**
 * read next symbol
 * symbol: [a-Z][a-Z0-9]*, up to 16 characters
 */
string readSymbol()
{
  // find next valid char
  moveToToken();

  // if eof
  if (infile.eof())
  {
    parseError(1);
  }

  // build symbol
  string symbol;
  char c;
  while (isValid(infile.peek()))
  {
    infile.get(c);
    offset++;
    symbol += c;

    regex reg("[a-zA-Z]");
    if (symbol.length() == 1 && !regex_match(symbol, reg))
    {
      parseError(1);
    }
  }

  if (symbol.length() > 16)
  {
    parseError(3);
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

  // check eof
  if (infile.eof())
  {
    parseError(2);
  }

  // get char
  char c;
  infile.get(c);
  offset++;

  // check error
  if (c != 'I' && c != 'E' && c != 'A' && c != 'R')
  {
    parseError(2);
  }

  return c;
}

/**
 * check the character is valid
 *
 * @param character
 */
bool isValid(char ch)
{
  return ch != ' ' && ch != '\n' && ch != '\t' && ch != -1;
}

/**
 * jump to next valid character
 */
void moveToToken()
{
  char c;
  // memorize current location
  int tmpLine = line;
  int tmpOffset = offset;

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
      line = line - 2;
      offset = line == tmpLine ? tmpOffset + 1 : 1;
      break;
    }
  }
}

/**
 * handle parsing error
 * @param errcode
 */
void parseError(int errcode)
{
  cout << "Parse Error line " << line << " offset " << offset << ": " << errstr[errcode] << endl;
  exit(0);
}

/**
 * calculate integer digits
 * @param number
 */
int calculateDigit(int num)
{
  int digits = 0;
  if (num < 0)
  {
    digits = 1;
  }

  while (num != 0)
  {
    num /= 10;
    digits++;
  }

  return digits;
}

/**
 * program starts from here
 */
int main(int argc, char *argv[])
{
  tokenizer(argv[1]);

  return 0;
}
