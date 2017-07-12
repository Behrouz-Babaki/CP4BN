#ifndef LIM_CONDENSED_H
#define LIM_CONDENSED_H

#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"

#include <vector>
#include <utility>
#include <map>
#include <gecode/float.hh>
#include <gecode/search.hh>
#include <gecode/int.hh>
#include "acReader.h"
#include "lmapReader.h"
#include "lim.h"

#ifdef USE_GIST
#include <gecode/gist.hh>
#endif //USE_GIST


class Lim_Condensed : public Lim {

 public:
  Lim_Condensed(const AcReader&, double, int);
  Lim_Condensed(bool, Lim_Condensed&);
  virtual Gecode::Space* copy(bool);
  void branching();
  void addConstraints(const AcReader&);
  void print(std::ostream&) const;
  void getSetnProb(vector<size_t>&, 
		   double&,
		   double&) const;
  int getItemsetSize() const;

 protected:
  double _threshold;
  int _numItems;
  int _condType;

  Gecode::BoolVarArray Items;
  Gecode::BoolVarArray negItems;
  Gecode::FloatVar prob;
  Gecode::FloatVarArray acNodeVars;
  std::map<std::pair<int,int>, std::pair<Gecode::FloatVar,bool> > copyMap;

  void channelItem(int, const Gecode::FloatVar&);
  void buildTree(const Gecode::BoolVarArray&, const AcReader&) ;
  void setupLiteralNode (const Gecode::BoolVarArray&, 
			 Gecode::FloatVarArray&, 
			 int, const AcReader&);
  void setupAndNode (Gecode::FloatVarArray&, 
		     int, const AcReader&);
  void setupOrNode (Gecode::FloatVarArray&, 
		    int, const AcReader&);
  void setupTrueNode (Gecode::FloatVarArray&, int);
  void setupFalseNode (Gecode::FloatVarArray&, int);
  void addCondConstraint (int, const AcReader&);
  std::pair<Gecode::FloatVar,bool> getCpVarForAffectedAcNode (int nodeNumber, int itemNumber, const AcReader& acreader, const LmapReader& lmapreader);
  Gecode::FloatVar getCpVarForOriginalAcNode (int nodeNumber);
  std::pair<Gecode::FloatVar,bool> getCpVarForAffectedLiteral (int nodeNumber, int itemNumber, const AcReader& acreader, const LmapReader& lmapReader);
  std::pair<Gecode::FloatVar, bool> getCpVarForAffectedOr (int nodeNumber, int itemNumber, const AcReader& acreader, const LmapReader& lmapReader);
  std::pair<Gecode::FloatVar, bool> getCpVarForAffectedAnd (int nodeNumber, int itemNumber, const AcReader& acreader, const LmapReader& lmapReader);
  void addEvidenceConstriants (const AcReader&);


};

#endif //LIM_CONDENSED_H
