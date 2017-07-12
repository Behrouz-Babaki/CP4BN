#include <iostream>
#include <fstream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::streamsize;
using std::vector;

void printItem(short);
int is_big_endian();

int main(void){
  ifstream input;
  ofstream output;
  
  output.open ("testBin" , ifstream::out | ifstream::binary);
  short i = 3;
  double val = 0.4;
  output.write ((char*)&i, 2);
  output.write ((char*)&val, 8);
  i = 5;
  val = 0.3;
  output.write ((char*)&i, 2);
  output.write ((char*)&val, 8);
  output.close();
  
  input.open ("testBin" , ifstream::in | ifstream::binary);
  input.read((char*) &i, 2);
  printItem(i);
  input.read((char*) &val, 8);
  cout << val << "\n";
  input.read((char*) &i, 2);
  printItem(i);
  input.read((char*) &val, 8);
  cout << val << "\n";
  input.close();

  cout << "is big endian? " << is_big_endian() << "\n";
  cout << "size of doulbe: " << sizeof(double) << "\n";
  cout << "size of size_t: " << sizeof(size_t) << "\n";
  return 0;
}

void printItem(short c){
  unsigned short mask, result;
  vector<bool> v;

  for (size_t i = 0; i < sizeof(unsigned short)*8; i++){
    mask = 1;
    result = 0;
    mask <<= i;
    result = mask & c;
    result >>= i;
    v.push_back(result == 1);
  }

  for (auto ritr=v.rbegin(), rend=v.rend(); ritr!=rend; ritr++)
    cout << *ritr;
  cout << "\n";

}

int is_big_endian()
{
  union {
    long int l;
    char c[sizeof (long int)];
  } u;

  u.l = 1;

  if (u.c[sizeof(long int)-1] == 1)
    {
      return 1;
    }
  else
    return 0;
}
