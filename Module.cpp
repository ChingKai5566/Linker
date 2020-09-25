#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

class Module
{
public:
  int defCount;
  int useCount;
  int codeCount;
  vector<string> symUseList;
};