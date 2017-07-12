#ifndef PAIRREADER_H
#define PAIRREADER_H

#include "lmapReader.h"
#include <vector>
#include <utility>
#include <string>

using std::pair;
using std::vector;
using std::string;

class PairReader {
 public:
  PairReader(const char*, 
	     const LmapReader&,
	     const LmapReader&);
  void getVarPairs(vector<pair<size_t, size_t> >&) const;
  void getMarg1(vector<size_t>&) const;
  void getMarg2(vector<size_t>&) const;
  void test(void) const;

 private:
  const char* _fileName;
  vector<string> _varNames1;
  vector<string> _varNames2;
  vector<vector<string> > _valNames1;
  vector<vector<string> > _valNames2;
  vector<pair<size_t, size_t> > _varPairs;
  vector<size_t> _margVars1;
  vector<size_t> _margVars2;

  bool getPairIndex (string, string,
		     pair<size_t, size_t>&) const;
  int findIndex(const vector<string>&, string) const;
};

#endif //PAIRREADER_H
