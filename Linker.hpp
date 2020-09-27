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

struct Module
{
  int len;
  int start;
  map<string, int> symToVal;
  map<char, int> typeToAddr;
};

// print a pair
template <typename T1, typename T2>
void print_pair(pair<T1, T2> &p)
{
  cout << p.first << ": " << p.second << endl;
}

// print a map
template <typename T>
void print_map(T &m)
{
  if (m.empty())
    return;
  for (auto x : m)
    print_pair(x);
}

// print a list
template <typename T>
void print_list(vector<T> &v)
{
  if (v.empty())
    return;
  for (T &i : v)
    cout << i << " ";
  cout << endl;
}