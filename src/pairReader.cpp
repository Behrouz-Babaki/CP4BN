#include "pairReader.h"
#include "lmapReader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <utility>
#include <cstdlib>
#include <boost/algorithm/string.hpp>

using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;
using std::istringstream;
using std::vector;
using std::set;
using std::pair;
using std::make_pair;

using boost::trim;

PairReader::PairReader(const char* filename,
		       const LmapReader& MapReader1,
		       const LmapReader& MapReader2) {

  MapReader1.getVarNames(_varNames1);
  MapReader1.getValNames(_valNames1);
  MapReader2.getVarNames(_varNames2);
  MapReader2.getValNames(_valNames2);

  ifstream pFstr;
  try {
    pFstr.open(filename, ifstream::in);
  } catch (ifstream::failure f) {
    cerr << "error reading file " << filename << endl;
    exit(1);
  }

  pair<size_t, size_t> pairIndex;
  set<size_t> firstIndices;
  set<size_t> secondIndices;
  string line;
  while (getline(pFstr, line)) {
    trim(line);
    string first(line);
    getline(pFstr, line);
    trim(line);
    string second(line);
    if (!getPairIndex(first, second, pairIndex)) {
      cerr << "error: Number of values of variables " <<
	first << " and " << second << " dose not match." << endl;
      exit(1);
    }
    _varPairs.push_back(pairIndex);
    firstIndices.insert(pairIndex.first);
    secondIndices.insert(pairIndex.second);
    getline(pFstr, line);
  }

  for (size_t counter = 0, size = _varNames1.size(); 
       counter < size; counter++)
    if (firstIndices.find(counter) == firstIndices.end())
      _margVars1.push_back(counter);

  for (size_t counter = 0, size = _varNames2.size(); 
       counter < size; counter++)
    if (secondIndices.find(counter) == secondIndices.end())
      _margVars2.push_back(counter);

}

void PairReader::getVarPairs(vector<pair<size_t, size_t> >& pairs) const {
  pairs.clear();
  pairs = _varPairs;
}

void PairReader::getMarg1(vector<size_t>& vars) const {
  vars.clear();
  vars = _margVars1;
}

void PairReader::getMarg2(vector<size_t>& vars) const {
  vars.clear();
  vars = _margVars2;
}

bool PairReader::getPairIndex(string first, 
			      string second, 
			      pair<size_t, size_t>& pair) const {
  int index1, index2;
  index1 = findIndex(_varNames1, first);
  if (index1 < 0)
    return false;
  index2 = findIndex(_varNames2, second);
  if (index2 < 0)
    return false;
  if (_valNames1[index1].size() != _valNames2[index2].size())
    return false;

  pair = make_pair(index1, index2);

  for (size_t counter = 0, size = _valNames1[index1].size(); 
       counter < size; counter++)
    if (_valNames1[index1][counter].compare(_valNames2[index2][counter]))
      return false;

  return true;
}

int PairReader::findIndex(const vector<string>& vec, string str) const {
  int index;
  bool found = false;
  for (size_t counter = 0, size = vec.size();
       !found && counter < size; counter++)
    if (! vec[counter].compare(str)) {
      index = counter;
      found = true;
    }
  if (!found) 
    return -1;

  return index;
 }

void PairReader::test(void) const {
  cout << "pairs:" << endl;
  for (auto pair : _varPairs) 
    cout << _varNames1[pair.first] << 
      " -- " << _varNames2[pair.second] << endl;
  
  cout << endl << "marg1:" << endl;
  for (auto ind : _margVars1)
    cout << _varNames1[ind] << endl;
  
  cout << endl << "marg2:" << endl;
  for (auto ind : _margVars2)
    cout << _varNames2[ind] << endl;
  
}
