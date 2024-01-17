#include <iostream>
#include <vector>
#include <string>
using namespace std;


string input(string var) {
  cout << var;
  string ret = "";
  getline(cin, ret);
  return ret;
}

// Add more native function definitions here

int main() {
string a = input("name: ");
cout << a<< std::endl;    return 0;
}