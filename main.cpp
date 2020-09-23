/* strtok example */
#include <cstdio>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

template<typename T>
void printV(vector<T> v);
vector<string> getToken(char buf[]);

int main (int argc, char * argv[])
{ 
  static const char * filename = argv[1];
  static char buf[512];

  // read file into buf
  cout << "read " << filename << " file" << endl;

  ifstream infile(filename);

  while (infile.good()) {
        infile.read(buf, sizeof(buf));
        cout << buf << endl;
  }

  cout << " " << endl;

  // tokenizer
  vector<string> tokens = getToken(buf);

  printV(tokens);

  infile.close();

  return 0;
}

vector<string> getToken(char buf[])
{
  vector<string> tokens;
  char * token = strtok(buf, " ");

  while (token != NULL) {
    tokens.push_back(token);
    token = strtok(NULL, " ");
  }

  return tokens;
}

template<typename T>
void printV(vector<T> v)
{
  for (int i = 0; i < v.size(); i++) {
    cout << v[i] << endl;
  }

}