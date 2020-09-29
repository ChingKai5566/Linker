#include "Linker.hpp"

/**
 * Global variable
 */
static ifstream infile;
static int line;
static int offset;
static map<string, int> symToVal; // definition list
static set<string> duplicateSym;
static map<string, int> symToModule;

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

  cout << endl;
  cout << "Memory Map" << endl;

  pass2(filename);
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
  int curModule = 0;

  while (!infile.eof())
  {
    // create module
    Module module;
    curModule++;

    // definition list
    int defCount = readInt();

    // check def count
    if (defCount > 16)
    {
      offset -= (calculateDigit(defCount) - 1);
      parseError(4);
    }

    // finish parsing
    if (defCount == -1)
    {
      curModule--;
      break;
    }

    for (int i = 0; i < defCount; i++)
    {
      string sym = readSymbol();
      int val = readInt();

      if (symToVal.count(sym) > 0)
      {
        duplicateSym.insert(sym);
      }
      else
      {
        symToVal[sym] = val + len;
        module.symToVal[sym] = val;
        symToModule[sym] = curModule; // for pass2 use to check the symbol used
      }
    }

    // use list
    int useCount = readInt();

    if (useCount == -1)
    {
      parseError(0);
    }

    // check use Count
    if (useCount > 16)
    {
      offset -= (calculateDigit(useCount) - 1);
      parseError(5);
    }

    for (int i = 0; i < useCount; i++)
    {
      string sym = readSymbol();
    }

    // program text
    int codeCount = readInt();

    // check total length < 512
    module.start = len;
    len += codeCount;
    if (len > 512)
    {
      offset -= (calculateDigit(codeCount) - 1);
      parseError(6);
    }

    module.len = codeCount;

    for (int i = 0; i < codeCount; i++)
    {
      char addressMode = readIEAR();
      int operand = readAddr();
      // module.typeToAddr[addressMode] = operand;
    }

    for (auto m : module.symToVal)
    {
      if (m.second >= codeCount)
      {
        cout << "Warning: Module " << curModule << ": " << m.first << " too big " << m.second << " (max=" << codeCount - 1 << ") assume zero relative" << endl;
        symToVal[m.first] = module.start;
      }
    }
  }

  // output Symbol Table
  cout << "Symbol Table" << endl;

  for (auto m : symToVal)
  {
    cout << m.first << "=" << m.second << " ";

    if (duplicateSym.count(m.first) > 0)
    {
      cout << "Error: This variable is multiple times defined; first value used";
    }

    cout << endl;
  }

  infile.close();
}

/**
 * second time go through the text, check run time error and warning
 * @param name input file name
 */
void pass2(string filename)
{
  infile.open(filename);
  int len = 0;
  int curModule = 0;

  while (!infile.eof())
  {
    curModule++;

    // definition list
    int defCount = readInt();

    // finish parsing
    if (defCount == -1)
    {
      curModule--;
      break;
    }

    for (int i = 0; i < defCount; i++)
    {
      string sym = readSymbol();
      int val = readInt();
    }

    // use list
    int useCount = readInt();
    set<string> usedSet;

    // addr to external
    static map<int, string> addrToSymExternal;
    int ref = 0;

    for (int i = 0; i < useCount; i++)
    {
      string sym = readSymbol();
      usedSet.insert(sym);
      addrToSymExternal[ref++] = sym;
    }

    // program text
    int codeCount = readInt();

    for (int i = 0; i < codeCount; i++)
    {
      char addressMode = readIEAR();
      int instr = readAddr();

      int opcode = instr / 1000;
      int operand = instr % 1000;
      int addr = len + i;

      // add operand into memoryMap
      if (addressMode == 'R')
      {
        if (opcode >= 10)
        {
          printMemoryTable(addr, 9999);
          cout << " Error: Illegal opcode; treated as 9999";
        }
        else
        {
          if (operand >= codeCount)
          {
            instr = opcode * 1000 + len;
            printMemoryTable(addr, instr);
            cout << " Error: Relative address exceeds module size; zero used";
          }
          else
          {
            instr += len;

            printMemoryTable(addr, instr);
          }
        }
      }

      if (addressMode == 'A')
      {
        if (opcode >= 10)
        {
          printMemoryTable(addr, 9999);
          cout << " Error: Illegal opcode; treated as 9999";
        }
        else
        {
          if (operand >= 512)
          {
            instr = opcode * 1000;
            printMemoryTable(addr, instr);
            cout << " Error: Absolute address exceeds machine size; zero used";
          }
          else
          {
            printMemoryTable(addr, instr);
          }
        }
      }

      if (addressMode == 'E')
      {
        if (opcode >= 10)
        {
          printMemoryTable(addr, 9999);
          cout << " Error: Illegal opcode; treated as 9999";
        }
        else
        {
          if (operand >= useCount)
          {
            printMemoryTable(addr, instr);
            cout << " Error: External address exceeds length of uselist; treated as immediate";
          }
          else
          {
            string curSym = addrToSymExternal[operand];

            // mark curSym as used
            if (usedSet.count(curSym))
            {
              usedSet.erase(curSym);
            }

            // check curSym is defined
            if (symToVal.count(curSym))
            {
              if (symToModule.count(curSym))
              {
                symToModule.erase(curSym);
              }

              instr = opcode * 1000 + symToVal[curSym];
              printMemoryTable(addr, instr);
            }
            else
            {
              instr = opcode * 1000;
              printMemoryTable(addr, instr);
              cout << " Error: " << curSym << " is not defined; zero used";
            }
          }
        }
      }

      if (addressMode == 'I')
      {
        if (instr > 9999)
        {
          printMemoryTable(addr, 9999);
          cout << " Error: Illegal immediate value; treated as 9999";
        }
        else
        {
          printMemoryTable(addr, instr);
        }
      }

      cout << endl;
    }

    for (string sym : usedSet)
    {
      cout << "Warning: Module " << curModule << ": " << sym << " appeared in the uselist but was not actually used" << endl;
    }

    len += codeCount;
  }

  cout << endl;

  for (auto m : symToModule)
  {
    cout << "Warning: Module " << m.second << ": " << m.first << " was defined but never used" << endl;
  }
}

/**
 * output memory table
 * @param address memory location
 * @param instruction operand
 */
void printMemoryTable(int addr, int instr)
{
  string address = "";
  string instruction = "";

  if (addr == 0)
  {
    address = "000";
  }
  else if (calculateDigit(addr) == 1)
  {
    address += "00";
    address += to_string(addr);
  }
  else
  {
    address += "0";
    address += to_string(addr);
  }

  if (instr == 0)
  {
    instruction = "0000";
  }
  else if (calculateDigit(instr) == 1)
  {
    instruction += "000";
    instruction += to_string(instr);
  }
  else if (calculateDigit(instr) == 2)
  {
    instruction += "00";
    instruction += to_string(instr);
  }
  else if (calculateDigit(instr) == 3)
  {
    instruction += "0";
    instruction += to_string(instr);
  }
  else
  {
    instruction += to_string(instr);
  }

  cout << address << ": " << instruction;
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
  int tmpOffset = offset;

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
    offset = tmpOffset + 1;
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
