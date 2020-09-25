#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

class Module
{
public:
  Module();
  int defCount;
  int useCount;
  int codeCount;
  unordered_map<string, int> symToVal;
  vector<string> symUseList;
  unordered_map<char, int> typeToAddr;
}