#include "Tokenizer.hpp"

ifstream infile;

Tokenizer::Tokenizer(string filename)
{
  infile.open(filename);
  
  while (!infile.eof()) {
    int defcount = readInt();
    
  }
  
  infile.close();
}


int Tokenizer::readInt()
{
  int defcount = 0;
  char c;
  while (Tokenizer::isValid(infile.peek())) {
    infile.get(c);
    defcount = defcount * 10 + (c - '0');
  }

  return defcount;
}

bool Tokenizer::isValid(char ch)
{
  return ch != ' ' && ch != '\n' && ch != '\t';
}

