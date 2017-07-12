#include "acReader.h"
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

AcReader::AcReader(const char* filename,
		   const LmapReader& MapReader) :
  _fileName(filename), _MapReader(MapReader) {

  string line;
  ifstream acFstr;
  try {
    acFstr.open(filename, ifstream::in);
  } catch (ifstream::failure f) {
    cout << "error reading file " << filename << endl;
    exit(1);
  }

  getline(acFstr, line);
  istringstream ss0(line);
  int nVars = 0, nEdges = 0, nNodes = 0;
  string nnf;
  ss0 >> nnf >> nNodes >> nEdges >> nVars;
  if (ss0.fail()) {
    cout << "error reading the header" << endl;
    exit (EXIT_FAILURE);
  }

  _type.resize(nNodes, nUndef);
  _parent.resize(nNodes, -1);
  _index.resize(nNodes, -1);

  vector<size_t> nVarVals;
  _MapReader.getNVarVals (nVarVals);
  int numVars = nVarVals.size();
  _varVals.resize(numVars);
  for (int varCounter = 0; varCounter < numVars; varCounter++)
    _varVals[varCounter].resize(nVarVals[varCounter], false);

  vector<string> components;
  int lineCounter = 0;
  int rootNode = -1;
  try {

    while (getline(acFstr,line)) {
      components.clear();
      split(components, line, is_any_of(" \t"));

      char lineType = components[0].front();
      switch(lineType){
      case 'L':
	this->read_literal(components, lineCounter);
	break;

      case 'A':
	this->read_and(components, lineCounter);
	rootNode = lineCounter;
	break;

      case 'O':
	this->read_or(components, lineCounter);
	rootNode = lineCounter;
	break;
      }
      lineCounter++;
    }

  } catch( bad_lexical_cast const& ) {
    cout << "Error: invalid number of nodes in line " 
	 << lineCounter << " of lamp file." << endl;
    exit (EXIT_FAILURE);
  }

  _rootNode = rootNode;
  acFstr.close();
}

void AcReader::read_literal(vector<string> components, int lineNo){
  assert (components.size() == 2);
  _type[lineNo] = nLiteral;
  int literal = lexical_cast<int> (components[1]);
  _literal.push_back(literal);
  _index[lineNo] = _literal.size() - 1;
  _literalToNode [literal] = lineNo;

  LitType type = _MapReader.getLitType (literal);
  if (type == lVariable) {
    int varId = _MapReader.getVarForLiteral (literal);
    int valId = _MapReader.getValForLiteral (literal);
    _varVals [varId][valId] = true;
  }
}

void AcReader::read_and(vector<string> components, int lineNo){
  assert (components.size() >= 2);
  size_t childNo = lexical_cast<int> (components[1]);
  assert(components.size() == childNo + 2);

  if(childNo == 0){ //True
    _type[lineNo] = nTrue;
    return;
  }

  _type[lineNo] = nAnd;
  vector<int> children;
  for (size_t counter = 2; counter < components.size(); counter++) {
    int currentChild = lexical_cast<int> (components[counter]);
    children.push_back(currentChild);
    _parent[currentChild] = lineNo;
  }
  
  _and.push_back(children);
  _index[lineNo] = _and.size() - 1;

}

void AcReader::read_or(vector<string> components, int lineNo){
  assert (components.size() >= 3);
  size_t childNo = lexical_cast<int> (components[2]);
  assert(components.size() == childNo + 3);

  if(childNo == 0){ //False
    _type[lineNo] = nFalse;
    return;
  }

  _type[lineNo] = nOr;
  vector<int> children;
  for (size_t counter = 3; counter < components.size(); counter++) {
    int currentChild = lexical_cast<int> (components[counter]);
    children.push_back(currentChild);
    _parent[currentChild] = lineNo;
  }
  
  _or.push_back(children);
  _index[lineNo] = _or.size() - 1;


}

string AcReader::getFileName() const {
  return this->_fileName;
}

void AcReader::getTree(vector<acNType>& types,
		       vector<int>& literals,
		       vector<int>& indices,
		       vector<vector<int> >& ands,
		       vector<vector<int> >& ors,
		       vector<int>& parents) const {
  types = this->_type;
  literals = this->_literal;
  indices = this->_index;
  ands = this->_and;
  ors = this->_or;
  parents = this->_parent;
}

int AcReader::getNVars() const {
  return (this->_MapReader).getNVars();
}

void AcReader::getWeights(vector<double>& weights) const{
  (this->_MapReader).getWeights(weights);
}

int AcReader::getNNodes() const {
  return (this->_type).size();
}

acNType AcReader::getNodeType (int node) const {
  assert (node < this->getNNodes());
  return (this->_type)[node];
}

void AcReader::getChildrenNodes (int node, vector<int>& children) const {
  assert (node < this->getNNodes());
  acNType nodeType = (this->_type)[node];
  int index = (this->_index)[node];
  assert (nodeType == nAnd || nodeType == nOr);
  if (nodeType == nAnd)
    children =  (this->_and)[index];
  if (nodeType == nOr)
    children = (this->_or)[index];
}

int AcReader::getParentNode (int node) const {
  return (this->_parent)[node];
}

int AcReader::getLiteralForNode (int node) const {
  assert (node < this->getNNodes());
  acNType nodeType = (this->_type)[node];
  assert (nodeType == nLiteral);
  int index = (this->_index)[node];
  return (this->_literal)[index];
}

int AcReader::getVarForNode (int node) const {
  int literal = this->getLiteralForNode(node);
  return (this->_MapReader).getVarForLiteral(literal);
}

int AcReader::getValForNode (int node) const {
  int literal = this->getLiteralForNode(node);
  return (this->_MapReader).getValForLiteral(literal);
}  

double AcReader::getWeightForNode (int node) const {
  int literal = this->getLiteralForNode(node);
  return (this->_MapReader).getWeightForLiteral(literal);
}

bool AcReader::isNodeVariable (int node) const {
  assert (node < this->getNNodes());
  if (this->getNodeType(node) != nLiteral)
    return false;
  int literal = this->getLiteralForNode(node);
  return ((this->_MapReader).getLitType(literal) == lVariable);
}

int AcReader::getRootNode () const {
  return _rootNode;
}

void AcReader::getNVarVals (vector<size_t>& varValNums) const {
  (this->_MapReader).getNVarVals (varValNums);
}

int AcReader::getNVals (int variable) const {
  return (this->_MapReader).getNVals(variable);
}

int AcReader::getNodeForVarVal (int variable, int value) const{
  cout << "finding node for var:" << variable << " and val:" << value << endl;
  int literal = (this->_MapReader).getLiteralForVarVal (variable, value);
  cout << "literal:" << literal << endl;
  map<int,int>::const_iterator itr = _literalToNode.find(literal);
  assert (itr != _literalToNode.end());
  return itr->second;
}

const LmapReader& AcReader::getLmapReader() const {
  return this->_MapReader;
}

vector<pair<int,int> > AcReader::getFixedVarVals() const {
  int numVars = _varVals.size();
  vector<pair<int,int> > fixedVarVals;
  for (int varCounter = 0; varCounter < numVars; varCounter++) {
    int numVals = _varVals[varCounter].size();
    int fixedId = -1;
    bool foundFirst = false;
    bool foundAnother = false;
    for (int valCounter = 0; !foundAnother && valCounter < numVals; valCounter++) 
      if (_varVals[varCounter][valCounter]) {
	if (!foundFirst) {
	  foundFirst = true;
	  fixedId = valCounter;
	}
	else
	  foundAnother = true;
      }
    if (!foundAnother)
      fixedVarVals.push_back(make_pair(varCounter, fixedId));
  }
  return fixedVarVals;
}
