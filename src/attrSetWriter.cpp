#include "attrSetWriter.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>

using std::vector;
using std::ofstream;

SetWriter::SetWriter () {}

SetWriter::SetWriter (const vector<size_t>& number_of_values, double threshold, char* file_name) : _number_of_values (number_of_values), _threshold (threshold) {
  this->_number_of_variables = _number_of_values.size();
  _output.open (file_name, ofstream::out | ofstream::binary);
  this->writeHeader();
}

SetWriter::~SetWriter() {
  _output.close();
}

void SetWriter::getNumVals (vector<size_t>& numVals) {
  numVals = this->_number_of_values;
}

size_t SetWriter::getNumVars (void) {
  return this->_number_of_variables;
}

void SetWriter::getByteVec (const vector<size_t>& attrValues, vector<unsigned char>& attrValBytes) {
  attrValBytes.clear();
  vector<bool> boolRep;
  this->boolVecRep(attrValues, this->_number_of_values, boolRep);
  size_t s = boolRep.size();
  assert (s%8 == 0);
  for (size_t counter = 0; counter < s/8; counter++){
    vector<bool>::iterator itr = boolRep.begin() + (8*counter);
    unsigned char avbyte = getByte(itr, itr+8);
    attrValBytes.push_back(avbyte);
  }
}

void SetWriter::write (const vector<size_t>& attrValues, double min, double max){
  vector<unsigned char> avbytes;
  this->getByteVec(attrValues, avbytes);
  for (auto b:avbytes)
    _output.write((char*)&b, 1); 
  _output.write((char*)&min, 8);
  _output.write((char*)&max, 8);
}

void SetWriter::boolVecRep (const vector<size_t>& entry, 
		 const vector<size_t>& number_of_values, 
		 vector<bool>& boolRep){

  vector<unsigned char> numbits;
  for (auto x:number_of_values)
    numbits.push_back(numBits(x));

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
    size_t mask = 1;
    size_t e = entry[counter];
    vector<bool> tempVec;
    for (size_t bitCounter=0; bitCounter < numbits[counter]; bitCounter++){
      tempVec.push_back(e & mask);
      mask <<= 1;
    }
    boolRep.insert(boolRep.end(), tempVec.rbegin(), tempVec.rend());
  }
}


unsigned char SetWriter::numBits (size_t value){
  if (!value)
    return 0;

  unsigned char highest = 0;
  unsigned char  mask = 1;
  for (unsigned char counter = 0; counter < 8; counter++, mask <<= 1){
    if (mask & value)
      highest = counter;
  }
  return highest + 1;
}

unsigned char SetWriter::getByte(vector<bool>::const_iterator begin, vector<bool>::const_iterator end){
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

void SetWriter::writeHeader (void) {
  _output.write((char*)&_threshold, 8);
  _output.write((char*)&_number_of_variables, 8); 

  for (auto v: _number_of_values)
    _output.write((char*)&v, 8); 
}
