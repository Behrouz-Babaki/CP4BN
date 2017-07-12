#pragma GCC diagnostic ignored "-Wunused-local-typedefs"

#include <vector>
#include <iomanip>
#include <gecode/float.hh>
#include <gecode/int.hh>
#include <gecode/search.hh>
#include <cstdlib>
#include "lim_condensed.h"
#include "lim_discriminative.h"
#include "lim.h"
#include "acReader.h"
#include "pairReader.h"

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

Lim_Discriminative::Lim_Discriminative(const AcReader& NetReader1,
				       const AcReader& NetReader2,
				       const PairReader& PReader,
				       double threshold) : Lim(threshold) {
  _numItems = NetReader1.getNVars();
  NetReader1.getNVarVals(_nVarVals);
  for (size_t counter = 0; counter < _numItems; counter++)
    Items.push_back(IntVar(*this, 0, _nVarVals[counter]));
  
  _numItems2 = NetReader2.getNVars();
  NetReader2.getNVarVals(_nVarVals2);
  for (size_t counter = 0; counter < _numItems2; counter++)
    Items2.push_back(IntVar(*this, 0, _nVarVals2[counter]));
  
  this->addConstraints(NetReader1, _numItems, Items, _nVarVals, prob);
  this->addConstraints(NetReader2, _numItems2, Items2, _nVarVals2, prob2);
  this->addPairingConstraints(PReader);
  this->addThresholdConstraint();
  this->branching();
}
				     
Lim_Discriminative::Lim_Discriminative(bool share, Lim_Discriminative& s): Lim(share, s) {
  _numItems2 = s._numItems2;
  _nVarVals2 = s._nVarVals2;
  Items2.resize(s.Items2.size());
  for (size_t counter = 0; counter < s._numItems2; counter++)
    Items2[counter].update(*this, share, s.Items2[counter]);
  
  prob2.update(*this, share, s.prob2);
}

Space* Lim_Discriminative::copy(bool share) {
  return new Lim_Discriminative(share, *this);
}

void Lim_Discriminative::branching() {
  Lim::branching();
  for (size_t counter = 0; counter < _numItems2; counter++)
    branch(*this, Items2[counter],
	   INT_VAL_MIN());
}

void Lim_Discriminative::addPairingConstraints(const PairReader& PReaderd) {
  vector<pair<size_t, size_t> > pairs;
  PReaderd.getVarPairs(pairs);
  for (auto pair : pairs)
    rel (*this, Items[pair.first], IRT_EQ, Items2[pair.second]);

  vector<size_t> margVars1;
  PReaderd.getMarg1(margVars1);
  for (auto index : margVars1)
    rel (*this, Items[index], IRT_EQ, _nVarVals[index]);

  vector<size_t> margVars2;
  PReaderd.getMarg2(margVars2);
  for (auto index : margVars2)
    rel (*this, Items2[index], IRT_EQ, _nVarVals2[index]);
  
}

void Lim_Discriminative::addThresholdConstraint() {
  FloatValArgs a(2);
  a[0] = 1;
  a[1] = -1;

  FloatVarArgs v(2);
  v[0] = prob;
  v[1] = prob2;

  FloatVar diff(*this, -1, 1);
  linear (*this, a , v, FRT_EQ, diff);

  FloatVar absDiff(*this, 0, 1);
  abs(*this, diff, absDiff);

  rel (*this, absDiff, FRT_GQ, _threshold);
}

void Lim_Discriminative::print(ostream& os) const {
  try{
    for (size_t counter = 0; counter<_numItems; counter++)
      os << Items[counter];
    os << " -- " << prob 
       << " -- " << prob2;
    os << endl;

  } catch (Exception e) {
    cerr << "ERROR \n" << e.what() << endl;
  }
}
