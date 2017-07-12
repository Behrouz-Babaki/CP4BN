#include "lmapReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using std::ifstream;
using std::ofstream;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::istringstream;
using std::vector;

using boost::split;
using boost::is_any_of;
using boost::lexical_cast;
using boost::bad_lexical_cast;


LmapReader::LmapReader(const char* filename): _fileName(filename){

  ifstream lmFstr;
  try{
    lmFstr.open(filename, ifstream::in);
  } catch (ifstream::failure f){
    cerr << "error reading file " << filename << endl;
    exit(1);
  }
  
  vector<string> components;
  string line;
  int lineCounter = 1;
  try {

    for (int counter = 0; counter < 3; counter++, lineCounter++)
      getline(lmFstr, line);
    split(components, line, is_any_of("$"));
    assert (components.size() == 3 && components[1].front() == 'N');
    _numLiterals = lexical_cast<int>(components[2]);
    _weights.resize(_numLiterals*2, 0);
    _types.resize(_numLiterals*2, lUndef);
    _vars.resize(_numLiterals*2, -1);
    _vals.resize(_numLiterals*2, -1);

    while (getline(lmFstr, line)){
      lineCounter++;
      components.clear();
      split(components, line, is_any_of("$"));

      if (!components.front().compare("cc"))
	if (components.size() >= 3) {
	  char lineType = components[1].front();
	  switch (lineType) {
	  case 'T':
	    this->line_pot_name(components);
	    break;

	  case 'V':
	    this->line_var_name(components);
	    break;

	  case 'I':
	    this->line_var_weight(components);
	    break;

	  case 'P':
	    this->line_pot_weight(components);
	    break;

	  case 'A':
	    this->line_const_weight(components);
	    break;
	  }
	}

    } //getline

  } catch( bad_lexical_cast const& ) {
    cout << "Error: invalid number of nodes in line " 
	 << lineCounter << " of lamp file." << endl;
    exit (EXIT_FAILURE);
  }

  
  lmFstr.close();
}

string LmapReader::getFileName() const {
  return this->_fileName;
}

void LmapReader::getLiterals(vector<double>& weights, 
			     vector<int>& vars,
			     vector<int>& vals
			     ) const {
  weights = this->_weights;
  vars = this->_vars;
  vals = this->_vals;

}

int LmapReader::getNVars() const {
  return this->_varValMaps.size();
}

LitType LmapReader::getLitType (int literal) const {
  int index = (literal > 0 ? 2*literal-1 : 2*-literal-2);
  return _types[index];
}

int LmapReader::getVarForLiteral (int literal) const {
  int index = (literal > 0 ? 2*literal-1 : 2*-literal-2);
  assert (this->getLitType(literal) == lVariable);
  return _vars[index];
}

int LmapReader::getValForLiteral (int literal) const {
  int index = (literal > 0 ? 2*literal-1 : 2*-literal-2);
  assert (this->getLitType(literal) == lVariable);
  return _vals[index];
}

double LmapReader::getWeightForLiteral (int literal) const {
  int index = (literal > 0 ? 2*literal-1 : 2*-literal-2);
  return _weights[index];
}

void LmapReader::getWeights(vector<double>& weights) const {
  weights = _weights;
}

void LmapReader::line_pot_name(vector<string>& components){
  // "cc" "T" srcPotName parameterCnt.
  assert (components.size() == 4);
  _potNames.push_back(components[2]);
  int parameterCnt = lexical_cast<int> (components[3]);
  _potParameterCnt.push_back (parameterCnt);
}

void LmapReader::line_var_name(vector<string>& components){

  assert(components.size() >= 5);
  int varIndex = _varNameMap.size();
  _varNameMap.insert(make_pair(components[2], varIndex));
  _varNames.push_back(components[2]);

  size_t valNo = lexical_cast<int> (components[3]);
  map<string,int> varValMap;
  vector<string> valNames;
  for (size_t counter = 0; counter < valNo; counter++){
    string valName = components[4+counter];
    varValMap.insert(make_pair(valName,counter));
    valNames.push_back(valName);
  }

  _varValMaps.push_back(varValMap);
  _varValNames.push_back(valNames);
}

void LmapReader::line_var_weight(vector<string>& components){

  assert(components.size() == 7);
  int literal;
  double weight;
  literal = lexical_cast<int> (components[2]);
  weight = lexical_cast<double> (components[3]);

  int index = (literal > 0 ? 2*literal-1 : 2*-literal-2);
  _types [index] = lVariable;
  int  varIndex = _varNameMap.at(components[4]);
  _vars[index] = varIndex;
  int valIndex = _varValMaps[varIndex].at(components[5]);
  _vals[index] = valIndex;
  this->_varValToLiteral [make_pair(varIndex,valIndex)] = literal; 
  this->_weights[index] = weight;
}

void LmapReader::line_pot_weight(vector<string>& components){
  assert(components.size() >= 5);
  int literal;
  double weight;

  literal = lexical_cast<int> (components[2]);
  weight = lexical_cast<double> (components[3]);
  int index = (literal > 0 ? 2*literal-1 : 2*-literal-2);
  _types [index] = lPotential;
  this->_weights[index] = weight;
  
}

void LmapReader::line_const_weight(vector<string>& components){

  assert(components.size() == 4);
  int literal;
  double weight;

  literal = lexical_cast<int> (components[2]);
  weight = lexical_cast<double> (components[3]);


  int index = (literal > 0 ? 2*literal-1 : 2*-literal-2);
  _types [index] = lConstant;
  this->_weights[index] = weight;
}

void LmapReader::writeVarValFile(string filename) const {
  ofstream vvFstr;
  try{
    vvFstr.open(filename, ifstream::out);
  } catch (ifstream::failure f){
    cerr << "error opening file " << filename << endl;
    exit(1);
  }
  size_t number_of_variables = _varNames.size();
  vvFstr << number_of_variables << "\n";
  for (size_t counter=0; counter < number_of_variables; counter++){
    vvFstr << _varNames[counter] << "\n";
    vector<string> values = _varValNames[counter];
    size_t number_of_values = values.size();
    vvFstr << number_of_values << "\n";
    for (size_t valCntr=0; valCntr < number_of_values; valCntr++)
      vvFstr << values[valCntr] << "\n";
  }
  vvFstr.close();
}

void LmapReader::getNVarVals(vector<size_t>& varValNums) const {
  varValNums.clear();
  for (auto varValName : _varValNames)
    varValNums.push_back(varValName.size());
}

int LmapReader::getNVals (int var) const {
  return _varValNames[var].size();
}

int LmapReader::getLiteralForVarVal (int var, int val) const {
  pair<int,int> key = make_pair(var,val);
  map<pair<int,int>,int>::const_iterator itr = _varValToLiteral.find(key);
  assert (itr != _varValToLiteral.end());
  return itr->second;
}

void LmapReader::getVarNames (vector<string>& varNames) const {
  varNames.clear();
  varNames = _varNames;
}

void LmapReader::getValNames (vector<vector<string> >& valNames) const {
  valNames.clear();
  valNames = _varValNames;
}
