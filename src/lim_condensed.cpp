#pragma GCC diagnostic ignored "-Wunused-local-typedefs"

#include <vector>
#include <iomanip>
#include <gecode/float.hh>
#include <gecode/int.hh>
#include <gecode/search.hh>
#include <cstdlib>
#include "lim_condensed.h"
#include "acReader.h"


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

Lim_Condensed::Lim_Condensed(const AcReader& NetReader, double threshold, int condType) :
  Lim(NetReader, threshold) , _condType(condType)
{
  _numItems = NetReader.getNVars();
  Items = BoolVarArray (*this, _numItems, 0, 1);
  negItems = BoolVarArray (*this, _numItems, 0, 1);
  prob = FloatVar (*this, 0, 1.01);

  this->addConstraints(NetReader);
  this->branching();
}

Lim_Condensed::Lim_Condensed(bool share, Lim_Condensed& s) : Lim(share, s){
  Items.update(*this, share, s.Items);
  negItems.update(*this, share, s.negItems);
  prob.update(*this, share, s.prob);
}

Space* Lim_Condensed::copy(bool share) {
  return new Lim_Condensed(share, *this);
}

void Lim_Condensed::branching() {
  
  branch(*this, Items,
	 INT_VAR_SIZE_MIN(),
	 INT_VAL_MIN());
}

void Lim_Condensed::print(ostream& os) const {
  try{
    for (int counter = 0, n = Items.size(); counter<n; counter++)
      os << Items[counter];
    os << "$" << prob;
    os << endl;

  } catch (Exception e) {
    cerr << "ERROR \n" << e.what() << endl;
  }
}

void Lim_Condensed::getSetnProb (vector<size_t>& attrValues,
		       double& minPr, double& maxPr) const {
  attrValues.clear();
  for (int counter=0, n = Items.size(); counter < n; counter++)
    if (Items[counter].val())
      attrValues.push_back(1);
    else
      attrValues.push_back(2);
  minPr = prob.min();
  maxPr = prob.max();
}

int Lim_Condensed::getItemsetSize () const {
  size_t itemsetSize = 0;
  for (int counter = 0, n = Items.size(); counter < n; counter++)
    if (Items[counter].val())
      itemsetSize++;
  return itemsetSize;
}

void Lim_Condensed::addConstraints(const AcReader& NetReader) {

  Gecode::FloatVarArray _Items (*this, Items.size(), 0, 1);

  // Item != negItem
  for (int counter=0; counter<_numItems; counter++) 
    rel(*this, Items[counter], IRT_NQ, negItems[counter]);
  
  buildTree(negItems,NetReader);

  /// evidence constraints
  addEvidenceConstriants (NetReader);

  /// defining the constraint over the root node
  rel (*this, prob, FRT_GQ, _threshold);
  
  /// define the closed/maximal constraints
  for (int counter =0; counter < _numItems; counter++)
    addCondConstraint (counter, NetReader);
}

void Lim_Condensed::buildTree(const BoolVarArray& negItems, const AcReader& NetReader) {

  int acNNodes = NetReader.getNNodes();
  acNodeVars = FloatVarArray (*this, acNNodes);
  
  // creating FloatVars for internal nodes
  // remember to reflect number of children in the domain for OR nodes
  for (int nodeNumber = 0; nodeNumber < acNNodes; nodeNumber++){
    acNType nodeType = NetReader.getNodeType(nodeNumber);
    switch (nodeType){
    case nLiteral:
      setupLiteralNode (negItems, acNodeVars, nodeNumber, NetReader);
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
  rel(*this, acNodeVars[rootNode] , FRT_EQ, prob);

}

void Lim_Condensed::addCondConstraint (int itemNumber, const AcReader& reader) {

  int rootNode = reader.getRootNode();
  pair<FloatVar, bool> root_copy = getCpVarForAffectedAcNode (rootNode, itemNumber, reader, reader.getLmapReader());
  if (root_copy.second) {
      BoolVar r (*this, 0, 1);
      rel (*this, negItems[itemNumber] , IRT_LQ, r);
      if (_condType == 1) { // maximal 
	// Item[itemNumber] == 0 -> root_copy < _threshold
	rel (*this, root_copy.first, FRT_LE, _threshold, r);
      }
      if (_condType == 2) { //closed
	int rootNode = reader.getRootNode();
	// Item[itemNumber] == 0 -> root_copy < root
	rel (*this, root_copy.first, FRT_LE, acNodeVars[rootNode], r);
      }
  }
}

void Lim_Condensed::setupLiteralNode (const BoolVarArray& negItems, 
			    FloatVarArray& acNodeVars,
			    int nodeNumber, 
			    const AcReader& reader) {
  double weight = reader.getWeightForNode (nodeNumber);
  if (reader.isNodeVariable(nodeNumber)){
    int variable = reader.getVarForNode (nodeNumber);
    int value = reader.getValForNode (nodeNumber);
    assert(variable < this->_numItems);
    assert (value == 0 || value == 1);
    if (value == 1) 
      acNodeVars [nodeNumber] = FloatVar (*this, weight, weight);
    else if (value == 0) {
      acNodeVars [nodeNumber] = FloatVar (*this, 0, weight);
      // negItems[variable] <=> acNodeVars[nodeNumber] = weight
      dom (*this, acNodeVars[nodeNumber], weight, negItems[variable]);
      dom (*this, acNodeVars[nodeNumber], 0, Items[variable]);
    }
  }
  else 
    acNodeVars [nodeNumber] = FloatVar (*this, weight, weight);
}

void Lim_Condensed::setupAndNode (FloatVarArray& acNodeVars,
			int nodeNumber, 
			const AcReader& reader) {

  vector<int> childrenNodes;
  reader.getChildrenNodes (nodeNumber, childrenNodes);
  double domainMax = 1;
  for (auto child : childrenNodes)
    domainMax *= acNodeVars[child].max();
  acNodeVars[nodeNumber] = FloatVar (*this, 0, domainMax);

  int childrenCnt = childrenNodes.size();

  if (childrenCnt > 2){

    FloatVarArray temp;
    temp = FloatVarArray (*this, childrenCnt-2);
    temp[0] = FloatVar(*this, 0, acNodeVars[childrenNodes[0]].max() 
		       * acNodeVars[childrenNodes[1]].max());
    for (int counter = 1; counter < childrenCnt-2; counter++)
      temp [counter] = FloatVar (*this, 0, acNodeVars[childrenNodes[counter+1]].max()
				 *temp[counter-1].max());

    
    mult(*this, acNodeVars[childrenNodes[0]], acNodeVars[childrenNodes[1]], temp[0]);
    for (int counter = 2; counter < childrenCnt-1; counter++) 
      mult (*this, temp[counter-2], acNodeVars[childrenNodes[counter]], temp[counter-1]);

    mult (*this, temp[childrenCnt-3], acNodeVars[childrenNodes[childrenCnt-1]], acNodeVars[nodeNumber]);
  }
  else if (childrenCnt == 2) 
    mult (*this, acNodeVars[childrenNodes[0]], acNodeVars[childrenNodes[1]], acNodeVars[nodeNumber]);

}

void Lim_Condensed::setupOrNode (FloatVarArray& acNodeVars,
		       int nodeNumber, 
		       const AcReader& reader) {

  vector<int> childrenNodes;
  reader.getChildrenNodes (nodeNumber, childrenNodes);

  int childrenCnt = childrenNodes.size();
  // The upper bound for an addition node is #children
  double maxDomain  = 0;
  for (auto child : childrenNodes)
    maxDomain += acNodeVars[child].max();
  acNodeVars[nodeNumber] = FloatVar (*this, 0, maxDomain);
  FloatVarArgs expression (childrenCnt);
  for (int counter=0; counter < childrenCnt; counter++)
    expression [counter] = acNodeVars [childrenNodes[counter]];
  linear (*this, expression, FRT_EQ, acNodeVars[nodeNumber]);
    
}

void Lim_Condensed::setupTrueNode (FloatVarArray& acNodeVars,
			 int nodeNumber) {
  acNodeVars[nodeNumber] = FloatVar (*this, 1, 1);
}

void Lim_Condensed::setupFalseNode (FloatVarArray& acNodeVars,
			  int nodeNumber) {
  acNodeVars[nodeNumber] = FloatVar (*this, 0, 0);
}


pair<FloatVar,bool> Lim_Condensed::getCpVarForAffectedAcNode (int nodeNumber, int itemNumber, const AcReader& acreader, const LmapReader& lmapreader) {
  /* returns the cp var for polluted ac node per item */

  pair<int,int> copyKey = make_pair(nodeNumber, itemNumber);
  if (copyMap.find(copyKey) != copyMap.end()) 
    return copyMap[copyKey];

  pair<FloatVar, bool> affectedNode;
  acNType nodeType = acreader.getNodeType(nodeNumber);

  switch (nodeType){
  case nLiteral:
    affectedNode = getCpVarForAffectedLiteral (nodeNumber, itemNumber, acreader, lmapreader);
    break;
  case nAnd:
    affectedNode = getCpVarForAffectedAnd (nodeNumber, itemNumber, acreader, lmapreader);
    break;
  case nOr:
    affectedNode = getCpVarForAffectedOr (nodeNumber, itemNumber, acreader, lmapreader);
    break;
  case nTrue:
  case nFalse:
    affectedNode = make_pair(getCpVarForOriginalAcNode (nodeNumber), false);
    break;
  }
  
  copyMap[copyKey] = affectedNode;
  return affectedNode;
}


FloatVar Lim_Condensed::getCpVarForOriginalAcNode (int nodeNumber) {
  return acNodeVars[nodeNumber];
}

pair<FloatVar,bool> Lim_Condensed::getCpVarForAffectedLiteral (int nodeNumber, int itemNumber, const AcReader& acreader, const LmapReader& lmapreader) {
  if (!acreader.isNodeVariable(nodeNumber))
    return make_pair(getCpVarForOriginalAcNode(nodeNumber), false);

  int var = acreader.getVarForNode(nodeNumber);
  int val = acreader.getValForNode(nodeNumber);
  // IMPORTANT: I'm assuming that the order of values is 0)false -> 1) true
  if (var != itemNumber || val == 1)
    return make_pair (getCpVarForOriginalAcNode(nodeNumber), false);
  
  FloatVar affectedLiteral = FloatVar (*this, 0, 0);
  return make_pair(affectedLiteral, true);
}

pair<FloatVar, bool> Lim_Condensed::getCpVarForAffectedOr (int nodeNumber, int itemNumber, const AcReader& acreader, const LmapReader& lmapReader) {
  
  vector<int> childrenNodes;
  acreader.getChildrenNodes (nodeNumber, childrenNodes);

  int childrenCnt = childrenNodes.size();
  vector<FloatVar> affectedChildren;
  pair<FloatVar,bool> current_child;
  FloatVar current_child_var;
  bool affected = false;
  // The upper bound for an addition node is #children
  double maxDomain  = 0;
  for (auto child : childrenNodes) {
    current_child = getCpVarForAffectedAcNode (child, itemNumber, acreader, lmapReader);
    current_child_var = current_child.first;
    affectedChildren.push_back(current_child_var);
    maxDomain += current_child_var.max();
    if (current_child.second)
      affected = true;
  }

  if (!affected)
    return make_pair(getCpVarForOriginalAcNode (nodeNumber), false);
  
  FloatVar affectedOr = FloatVar (*this, 0, maxDomain);
  FloatVarArgs expression (childrenCnt);
  for (int counter=0; counter < childrenCnt; counter++)
    expression [counter] = affectedChildren[counter];
  linear (*this, expression, FRT_EQ, affectedOr);

  return make_pair(affectedOr, true);
}

pair<FloatVar, bool> Lim_Condensed::getCpVarForAffectedAnd (int nodeNumber, int itemNumber, const AcReader& acreader, const LmapReader& lmapReader) {
  vector<int> childrenNodes;
  acreader.getChildrenNodes (nodeNumber, childrenNodes);
  double domainMax = 1;
  vector<FloatVar> affectedChildren;

  pair<FloatVar,bool> current_child;
  FloatVar current_child_var;
  bool affected = false;
  for (auto child : childrenNodes) {
    current_child = getCpVarForAffectedAcNode (child, itemNumber, acreader, lmapReader);
    current_child_var = current_child.first;
    affectedChildren.push_back(current_child_var);
    domainMax *= acNodeVars[child].max();
    if (current_child.second)
      affected = true;
  }

  if (!affected)
    return make_pair(getCpVarForOriginalAcNode (nodeNumber), false);

  FloatVar affectedAnd = FloatVar (*this, 0, domainMax);
  int childrenCnt = childrenNodes.size();

  if (childrenCnt > 2){
    FloatVarArray temp;
    temp = FloatVarArray (*this, childrenCnt-2);
    temp[0] = FloatVar(*this, 0, affectedChildren[0].max() 
		       * affectedChildren[1].max());
    for (int counter = 1; counter < childrenCnt-2; counter++)
      temp [counter] = FloatVar (*this, 0, affectedChildren[counter+1].max()
				 *temp[counter-1].max());

    
    mult(*this, affectedChildren[0], affectedChildren[1], temp[0]);
    for (int counter = 2; counter < childrenCnt-1; counter++) 
      mult (*this, temp[counter-2], affectedChildren[counter], temp[counter-1]);

    mult (*this, temp[childrenCnt-3], affectedChildren[childrenCnt-1], affectedAnd);
  }
  else if (childrenCnt == 2) 
    mult (*this, affectedChildren[0], affectedChildren[1], affectedAnd);

  return make_pair(affectedAnd, true);
}

void Lim_Condensed::addEvidenceConstriants (const AcReader& acreader) {
  vector<pair<int,int> > fixedVarVals;
  fixedVarVals = acreader.getFixedVarVals();
  for (auto fixedVarVal : fixedVarVals)
    if (fixedVarVal.second == 1)
      rel (*this, Items [fixedVarVal.first], IRT_EQ, 1);
}
