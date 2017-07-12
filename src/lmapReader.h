#ifndef LMAPREADER_H
#define LMAPREADER_H

#include <vector>
#include <string>
#include <map>
#include <utility>

using std::vector;
using std::string;
using std::map;
using std::pair;
using std::make_pair;

typedef pair<const string, int> entry;
enum LitType {lVariable, lConstant, lPotential, lUndef};

class LmapReader {

 public:
  LmapReader(const char*);
  string getFileName() const;
  void getLiterals(vector<double>&,   /**<  weights of literals */
		   vector<int>&,      /**<  Bayesian variable identifiers */
		   vector<int>&       /**<  value identifiers for Bayesian variables */
		   ) const;
  int getNVars() const;
  void getNVarVals(vector<size_t>&) const;
  int getNVals (int) const;
  LitType getLitType (int) const;
  int getVarForLiteral(int) const;
  int getValForLiteral(int) const;
  double getWeightForLiteral (int) const;
  void getWeights(vector<double>&) const;
  void writeVarValFile(string) const;
  int getLiteralForVarVal(int, int) const;
  void getVarNames(vector<string>&) const;
  void getValNames(vector<vector<string> >&) const;

 private:

  int _numLiterals;
  string _fileName;

  vector<int> _vars;
  vector<int> _vals;
  vector<string> _potNames;
  vector<int> _potParameterCnt;
  vector<double> _weights;
  vector<LitType> _types;

  map<string,int> _varNameMap;
  map<pair<int,int>, int> _varValToLiteral;
  vector<string> _varNames;
  vector<map<string,int> > _varValMaps;
  vector<vector<string> > _varValNames;

  void line_pot_name(vector<string>&);
  void line_var_name(vector<string>&);
  void line_var_weight(vector<string>&);
  void line_pot_weight(vector<string>&);
  void line_const_weight(vector<string>&);

};

#endif //LMAPREADER_H
