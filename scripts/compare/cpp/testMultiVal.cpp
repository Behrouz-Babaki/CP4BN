#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include "attrSetWriter.h"

using std::cin;
using std::cout;
using std::vector;
using std::reverse;

unsigned char numBits (size_t);
unsigned char getByte(vector<bool>::const_iterator, vector<bool>::const_iterator);
void boolVecRep (const vector<size_t>&, const vector<size_t>&, vector<bool>&);

int main(void) {
  unsigned char numVals_[] = {8,3,9};
  unsigned char entries_[][3] = {
    {8, 2, 7},
    {1, 0, 8},
    {7, 2, 2}
  };
  double values[] = {0.23, 0.34, 0.45};
  vector<size_t> numVals (numVals_, numVals_+(sizeof(numVals_)/sizeof(unsigned char)));
  vector<vector<size_t> > entries (3);
  for (size_t counter = 0 ; counter < 3; counter++)
    entries [counter] = vector<size_t> (entries_[counter], entries_[counter] + (sizeof(entries_[counter])/sizeof(unsigned char)));
  
  double threshold = 0.77;
  SetWriter w (numVals, threshold, "tt.out");
  vector<unsigned char> v;
  for (size_t counter = 0, es = entries.size(); counter < es; counter++) {
    w.write(entries[counter], values[counter]);
    w.getByteVec(entries[counter],v);
    cout << v.size() << "-" << v.front()+0 << "." << v.back()+0 << "\n";
  }

  vector<size_t> numbits;
  for (auto x:numVals)
    numbits.push_back(numBits(x));

  for (size_t counter = 0; counter < 3; counter++) {
    vector<bool> boolRep;
    boolVecRep (entries[counter], numbits, boolRep);
    for (auto itr = boolRep.begin(), e_itr = boolRep.end(); itr!=e_itr; itr++)
      cout << *itr;
    cout << "\n";

    size_t s = boolRep.size();
    for (size_t counter = 0; counter < s/8; counter++){
      vector<bool>::iterator itr = boolRep.begin() + (8*counter);
      unsigned char bb = getByte(itr, itr+8);
      cout << bb+0 << "\n";
    }
  }
}

unsigned char numBits (size_t value){
  if (!value)
    return 0;

  unsigned char highest = 0;
  size_t mask = 1;
  for (size_t counter = 0; counter < 8; counter++, mask <<= 1){
    if (mask & value)
      highest = counter;
  }
  return highest + 1;
}

unsigned char getByte(vector<bool>::const_iterator begin, vector<bool>::const_iterator end){
  assert (end-begin == 8);
  unsigned char result = 0;
  vector<bool>::const_iterator itr = begin;
  for (size_t counter=0; counter < 8; counter++, itr++){
    result |= (unsigned char)(*itr);
    if (counter < 7)
      result <<= 1;
  }
  return result;
}

// returns a bool vector (with its size a multiple of 8)
// which is a concatenation of boolean representation of entries
void boolVecRep (const vector<size_t>& entry, 
		 const vector<size_t>& numbits, 
		 vector<bool>& boolRep){

  size_t bitCnt = 0;
  for (auto n:numbits)
    bitCnt += n;
  size_t extra = bitCnt%8;
  size_t zeroBits = 0;
  if (extra)
    zeroBits = 8-extra;

  boolRep.clear();
  boolRep.resize(zeroBits, 0);
  size_t numVals = entry.size();
  for (size_t counter=0; counter < numVals; counter++){
    unsigned char mask = 1;
    unsigned char e = entry[counter];
    vector<bool> tempVec;
    for (size_t bitCounter=0; bitCounter < numbits[counter]; bitCounter++){
      tempVec.push_back(e & mask);
      mask <<= 1;
    }
    boolRep.insert(boolRep.end(), tempVec.rbegin(), tempVec.rend());
  }
}


