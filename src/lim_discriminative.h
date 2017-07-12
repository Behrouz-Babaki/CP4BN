#ifndef LIM_DISCRIMINATIVE
#define LIM_DISCRIMINATIVE

#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"

#include <vector>
#include <iostream>
#include <gecode/float.hh>
#include <gecode/search.hh>
#include <gecode/int.hh>
#include "acReader.h"
#include "lmapReader.h"
#include "pairReader.h"
#include "lim.h"

class Lim_Discriminative : public Lim {
 public:
  Lim_Discriminative(const AcReader&,
		     const AcReader&,
		     const PairReader&, 
		     double);
  Lim_Discriminative(bool, Lim_Discriminative&);
  virtual Gecode::Space* copy(bool);
  virtual void print(std::ostream&) const;
  void branching();
  
 protected:
  size_t _numItems2;
  vector<size_t> _nVarVals2;
  std::vector<Gecode::IntVar> Items2;
  Gecode::FloatVar prob2;

  void addPairingConstraints(const PairReader&);
  void addThresholdConstraint();

};

#endif //LIM_DISCRIMINATIVE
