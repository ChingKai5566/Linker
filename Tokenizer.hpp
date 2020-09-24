#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class Tokenizer
{
public:
  Tokenizer(string filename);
  int readInt();
  bool isValid(char ch);
};