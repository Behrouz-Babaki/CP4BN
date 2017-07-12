#ifndef LIM_H
#define LIM_H

#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"

#include <vector>
#include <gecode/float.hh>
#include <gecode/search.hh>
#include <gecode/int.hh>
#include "acReader.h"
#include "lmapReader.h"

#ifdef USE_GIST
#include <gecode/gist.hh>
#endif //USE_GIST

class Lim : public Gecode::Space {

 public:
  Lim(double);
  Lim(const AcReader&, double);
  Lim(bool, Lim&);
  virtual Gecode::Space* copy(bool);
  void branching();
  void addConstraints(const AcReader&,
		      size_t,
		      const std::vector<Gecode::IntVar>&, 
		      const std::vector<size_t>&,
		      Gecode::FloatVar&);
  virtual void print(std::ostream&) const;
  void getSetnProb(vector<size_t>&, 
		   double&,
		   double&) const;

 protected:
  double _threshold;
  size_t _numItems;
  vector<size_t> _nVarVals;

  std::vector<Gecode::IntVar> Items;
  Gecode::FloatVar prob;

  void channelItem(const Gecode::BoolVar&, const Gecode::FloatVar&);
  void addItemLeafConstraint (const Gecode::IntVar&, 
			      const Gecode::BoolVarArray&, 
			      size_t);
  void buildTree(const std::vector<Gecode::FloatVarArray>&, 
		 const AcReader&,
		 Gecode::FloatVar&) ;
  void setupLiteralNode (const std::vector<Gecode::FloatVarArray>&, 
			 Gecode::FloatVarArray&, 
			 int, const AcReader&);
  void setupAndNode (Gecode::FloatVarArray&, 
		     int, const AcReader&);
  void setupOrNode (Gecode::FloatVarArray&, 
		    int, const AcReader&);
  void setupTrueNode (Gecode::FloatVarArray&, int);
  void setupFalseNode (Gecode::FloatVarArray&, int);
};

#endif //LIM_H
