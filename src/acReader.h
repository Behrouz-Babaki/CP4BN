#ifndef ACREADER_H
#define ACREADER_H

#include <vector>
#include <string>
#include <map>
#include <utility>
#include "lmapReader.h"

using std::vector;
using std::string;
using std::map;
using std::pair;

enum acNType {nLiteral, nAnd, nOr, nTrue, nFalse, nUndef};

class AcReader {
 public:
  AcReader(const char*, 
	   const LmapReader&);

  string getFileName() const;
  void getTree(vector<acNType>&,                /**<  types */
	       vector<int>&,	                /**<  literals */
	       vector<int>&,	                /**<  indices */
	       vector<vector<int> >&,           /**<  and nodes */
	       vector<vector<int> >&,           /**<  or nodes */
	       vector<int>& parents             /**<  parent nodes */
	       ) const ;   
  int getNVars() const ;
  void getWeights (vector<double>&) const;
  int getNNodes() const;
  acNType getNodeType (int) const;
  void getChildrenNodes (int, vector<int>&) const;
  int getParentNode (int) const;
  int getLiteralForNode (int) const;
  int getVarForNode (int) const;
  int getValForNode (int) const;
  int getNVals(int) const;
  int getNodeForVarVal (int, int) const;
  double getWeightForNode (int) const;
  bool isNodeVariable (int) const;
  int getRootNode () const;
  void getNVarVals (vector<size_t>&) const;
  const LmapReader& getLmapReader() const;
  vector<pair<int,int> > getFixedVarVals() const;

 private:
  string _fileName;
  vector<acNType> _type;
  vector<int> _parent;
  vector<int> _index;
  vector<vector<int> > _and;
  vector<vector<int> > _or;
  vector<int> _literal;
  map<int,int> _literalToNode;
  int _rootNode;
  const LmapReader& _MapReader;
  vector<vector<bool> > _varVals;

  void read_literal(vector<string>, int);
  void read_and(vector<string>, int);
  void read_or(vector<string>, int);

};

#endif //ACREADER_H
