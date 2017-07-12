#include <iostream>
#include <vector>
using std::cout;
using std::endl;
using std::vector;

int main(void) {
  unsigned short c, mask, result;
  vector<bool> v;
  c  = 255;

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
  return 0;
}	
