#pragma GCC diagnostic ignored "-Wunused-local-typedefs"

#include <vector>
#include <iomanip>
#include <gecode/float.hh>
#include <gecode/int.hh>
#include <gecode/search.hh>
#include <cstdlib>
#include "lim.h"
#include "acReader.h"
#include "nary_mult.h"

#ifdef USE_GIST
#include <gecode/gist.hh>
#endif //USE_GIST

using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::ostream;
using std::setprecision;

using namespace Gecode;

Lim::Lim(double threshold) : 
  _threshold(threshold) {}

Lim::Lim(const AcReader& NetReader, double threshold) :
  _threshold(threshold)
{
  _numItems = NetReader.getNVars();
  NetReader.getNVarVals(_nVarVals);
  for (size_t counter = 0; counter < _numItems; counter++) 
    Items.push_back(IntVar (*this, 0, _nVarVals[counter]));

  this->addConstraints(NetReader, _numItems, Items, _nVarVals, prob);
  /// defining the constraint over the root node
  rel (*this, prob, FRT_GQ, _threshold);

  this->branching();
}

Lim::Lim(bool share, Lim& s) : Space(share, s) {
  _numItems = s._numItems;
  _nVarVals = s._nVarVals;
  Items.resize(s.Items.size());
  for (size_t counter = 0; counter < s._numItems; counter++) 
    Items[counter].update(*this, share, s.Items[counter]); 

  prob.update(*this, share, s.prob);
}

Space* Lim::copy(bool share) {
  return new Lim(share, *this);
}

void Lim::branching() {

  for (size_t counter = 0; counter < _numItems; counter++)
    branch(*this, Items[counter],
	   INT_VAL_MIN());
}

void Lim::print(ostream& os) const {
  try{
    for (size_t counter = 0; counter<_numItems; counter++)
      os << Items[counter];
    os << "$" << prob;
    os << endl;

  } catch (Exception e) {
    cerr << "ERROR \n" << e.what() << endl;
  }
}

void Lim::getSetnProb (vector<size_t>& attrValues,
		       double& minPr, double& maxPr) const {
  attrValues.clear();
  for (size_t counter=0; counter < _numItems; counter++)
    attrValues.push_back(Items[counter].val());

  minPr = prob.min();
  maxPr = prob.max();
}

void Lim::channelItem (const BoolVar& b, const FloatVar& f) {
  IntVar intChannel (*this, 0, 1);
  channel(*this, f, intChannel);
  channel(*this, intChannel, b);
}

void Lim::addConstraints(const AcReader& NetReader, 
			 size_t numItems,
			 const vector<IntVar>& Items,
			 const vector<size_t>& nVarVals,
			 FloatVar& prob) {
  // TODO compare with the case of having one long constraint
  std::vector<Gecode::BoolVarArray> leaves (numItems);
  for (size_t counter = 0; counter < numItems; counter++)
    leaves [counter] = BoolVarArray (*this, nVarVals[counter] , 0, 1);

  for (size_t counter =0; counter < numItems; counter++)
    addItemLeafConstraint (Items[counter], leaves[counter], nVarVals[counter]);

  std::vector<Gecode::FloatVarArray> _leaves (_numItems);
  for (size_t counter = 0; counter < numItems; counter++)
    _leaves [counter] = FloatVarArray (*this, nVarVals[counter], 0, 1);

  for (size_t itemCounter=0; itemCounter < numItems;  itemCounter++) 
    for (size_t valCounter = 0; valCounter < nVarVals[itemCounter]; valCounter++)
    channelItem(leaves[itemCounter][valCounter], _leaves[itemCounter][valCounter]);

  buildTree(_leaves, NetReader, prob);

}

void Lim::addItemLeafConstraint (const IntVar& Item, 
				 const BoolVarArray& leaves,
				 size_t nVals) {
  /* I = v <-> t_v = 1 & t_j = 0 */
  for (size_t counter = 0; counter < nVals; counter++) {
    /* iv <-> I = v*/
    BoolVar iv (*this, 0, 1);
    rel (*this, Item, IRT_EQ, counter, iv);

    BoolVarArgs others (nVals - 1);
    int index = 0;
    for (size_t valCounter = 0; valCounter < nVals; valCounter++)
      if (valCounter != counter) {
	others[index] = leaves [valCounter];
	index ++;
      }

    BoolVarArgs self (1);
    self [0] = leaves [counter];
    clause (*this, BOT_AND, self, others, iv);
  }

  /* I = n+1 <-> /\ t_i = 1 */
  BoolVar iMarg (*this, 0, 1);
  rel (*this, Item, IRT_EQ , nVals , iMarg);
  BoolVarArgs allVals (nVals);
  for (size_t counter = 0; counter < nVals; counter++) 
    allVals [counter] = leaves[counter];

  rel (*this, BOT_AND, allVals, iMarg);
  
}

void Lim::buildTree(const vector<FloatVarArray>& _leaves, 
		    const AcReader& NetReader,
		    FloatVar& prob) {

  size_t acNNodes = NetReader.getNNodes();
  FloatVarArray acNodeVars;
  acNodeVars = FloatVarArray (*this, acNNodes);
  
  // creating FloatVars for internal nodes
  // remember to reflect number of children in the domain for OR nodes
  for (size_t nodeNumber = 0; nodeNumber < acNNodes; nodeNumber++){
    acNType nodeType = NetReader.getNodeType(nodeNumber);
    switch (nodeType){
    case nLiteral:
      setupLiteralNode (_leaves, acNodeVars, nodeNumber, NetReader);
      break;
    case nAnd:
      setupAndNode (acNodeVars, nodeNumber, NetReader);
      break;
    case nOr:
      setupOrNode (acNodeVars, nodeNumber, NetReader);
      break;
    case nTrue:
      setupTrueNode (acNodeVars, nodeNumber);
      break;
    case nFalse:
      setupFalseNode (acNodeVars, nodeNumber);
      break;
    }
  }

  int rootNode = NetReader.getRootNode();
  prob = acNodeVars[rootNode];

}

void Lim::setupLiteralNode (const vector<FloatVarArray>& _leaves, 
			    FloatVarArray& acNodeVars,
			    int nodeNumber, 
			    const AcReader& reader) {

  if (reader.isNodeVariable(nodeNumber)){
    size_t variable = reader.getVarForNode (nodeNumber);
    int value = reader.getValForNode (nodeNumber);
    assert(variable < this->_numItems);

      acNodeVars [nodeNumber] = FloatVar (*this, 0, 1);
      rel(*this, acNodeVars[nodeNumber] , FRT_EQ, _leaves[variable][value]);
  }

  else {
    double weight = reader.getWeightForNode (nodeNumber);
    acNodeVars [nodeNumber] = FloatVar (*this, weight, weight);
  }
  
}

void Lim::setupAndNode (FloatVarArray& acNodeVars,
			int nodeNumber, 
			const AcReader& reader) {

  vector<int> childrenNodeNrs;
  reader.getChildrenNodes (nodeNumber, childrenNodeNrs);
  
  // preprocess, remove assigned ones
  vector<FloatVar> childrenNodes;
  FloatVal fixed = 1.0; // already assigned part
  for (auto child: childrenNodeNrs) {
    if (acNodeVars[child].assigned()) {
      fixed *= acNodeVars[child].val();
    } else {
      childrenNodes.push_back(acNodeVars[child]);
    }
  }
  
  if (childrenNodes.size() == 0) {
    // special case: all assigned
    acNodeVars[nodeNumber] = FloatVar (*this, fixed.min(), fixed.max()); // Rounding: just use max?
  } else {
    // init max domain value
    FloatNum domainMax = 1;
    for (auto child : childrenNodes)
      domainMax *= child.max();
    domainMax *= fixed.max();
    
    if (childrenNodes.size() == 1) {
        if (fixed == 1.0) {
          acNodeVars[nodeNumber] = childrenNodes[0];
        } else {
          acNodeVars[nodeNumber] = FloatVar (*this, 0, domainMax);
          FloatVar tmp(*this, fixed.min(), fixed.max());
          mult (*this, childrenNodes[0], tmp, acNodeVars[nodeNumber]);
        }
    } else {

      int childrenCnt = childrenNodes.size();
      FloatVarArgs expression (childrenCnt);
      for (int counter=0; counter < childrenCnt; counter++)
        expression[counter] = childrenNodes[counter];
      
      acNodeVars[nodeNumber] = FloatVar (*this, 0, domainMax);
      nary_mult(*this, acNodeVars[nodeNumber], expression, fixed);
    }
  }

}

void Lim::setupOrNode (FloatVarArray& acNodeVars,
		       int nodeNumber, 
		       const AcReader& reader) {
  
  vector<int> childrenNodeNrs;
  reader.getChildrenNodes (nodeNumber, childrenNodeNrs);
  
  // preprocess, remove assigned ones
  vector<FloatVar> childrenNodes;
  FloatVal fixed = 0.0; // already assigned part
  for (auto child: childrenNodeNrs) {
    if (acNodeVars[child].assigned()) {
      fixed += acNodeVars[child].val();
    } else {
      childrenNodes.push_back(acNodeVars[child]);
    }
  }
  
  if (childrenNodes.size() == 0) {
    // special case: all assigned
    acNodeVars[nodeNumber] = FloatVar (*this, fixed.min(), fixed.max()); // Rounding: just use max?
  } else {
    // init max domain value
    FloatNum domainMax = 1;
    for (auto child : childrenNodes)
      domainMax += child.max();
    domainMax += fixed.max();
    
    if (childrenNodes.size() == 1 && fixed == 0.0) {
      acNodeVars[nodeNumber] = childrenNodes[0];
    } else {
        size_t childrenCnt = childrenNodes.size();
        
        FloatVarArgs expression (childrenCnt);
        for (size_t counter=0; counter < childrenCnt; counter++)
          expression[counter] = childrenNodes[counter];
        
        if (fixed != 0.0)
          expression << FloatVar (*this, fixed.min(), fixed.max());
        
        acNodeVars[nodeNumber] = FloatVar (*this, 0, domainMax);
        linear (*this, expression, FRT_EQ, acNodeVars[nodeNumber]);
    }
  }
}

void Lim::setupTrueNode (FloatVarArray& acNodeVars,
			 int nodeNumber) {
  acNodeVars[nodeNumber] = FloatVar (*this, 1, 1);
}

void Lim::setupFalseNode (FloatVarArray& acNodeVars,
			  int nodeNumber) {
  acNodeVars[nodeNumber] = FloatVar (*this, 0, 0);
}
