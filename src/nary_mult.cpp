#include "nary_mult.h"
#include <boost/iterator/iterator_concepts.hpp>

using namespace std;

void nary_mult(Home home, FloatVar x0, const FloatVarArgs& y0, FloatVal c0) {
  if (home.failed()) return;
  
  FloatView x(x0);
  ViewArray<FloatView> y(home, y0);
  
  GECODE_ES_FAIL((NaryMultPlus<FloatView>::post(home,x,y,c0)));
}

  /*
   * Bound consistent mult equation over positive floats
   * 
   * x = prod(y) * c
   *
   */

  template<class P>
  forceinline
  NaryMultPlus<P>::NaryMultPlus(Home home, FloatView& x0, ViewArray<P>& y0, FloatVal c0) //, ViewArray<N>& y
    : Propagator(home), x(x0), y(y0), c(c0) {
      x.subscribe(home,*this,PC_FLOAT_BND);
      y.subscribe(home,*this,PC_FLOAT_BND);      
  }
  
  template<class P>
  forceinline
  NaryMultPlus<P>::NaryMultPlus(Space& home, bool share, NaryMultPlus<P>& p)
    : Propagator(home,share,p), c(p.c) {
      x.update(home,share,p.x);
      y.update(home,share,p.y);
  }
  
  template<class P>
  PropCost
  NaryMultPlus<P>::cost(const Space&, const ModEventDelta&) const {
    int s = x.size();
    return PropCost::linear(PropCost::LO, 1+s);
  }
  
  template<class P>
  forceinline size_t
  NaryMultPlus<P>::dispose(Space& home) {
      x.cancel(home,*this,PC_FLOAT_BND);
      y.cancel(home,*this,PC_FLOAT_BND);
      (void) Propagator::dispose(home);
      return sizeof(*this);
  }
  
  template<class P>
  ExecStatus
  NaryMultPlus<P>::post(Home home, FloatView& x, ViewArray<P>& y, FloatVal c) {
    // TODO: simplify if some already assigned?
    (void) new (home) NaryMultPlus<P>(home,x,y,c);
    return ES_OK;
  }

  template<class P>
  Actor*
  NaryMultPlus<P>::copy(Space& home, bool share) {
    return new (home) NaryMultPlus<P>(home,share,*this);
  }

  template<class P>
  ExecStatus
  NaryMultPlus<P>::propagate(Space& home, const ModEventDelta& med) {
    Rounding r;
    
    // Eliminate singletons
    if (FloatView::me(med) == ME_FLOAT_VAL) {
      int n = y.size();
      for (int i = n; i--; ) {
        if (y[i].assigned()) {
          c *= y[i].val();  y[i] = y[--n];
        }
      }
      y.size(n);
    }
    
    // trivial: all assigned or c=0
    if (y.size() == 0 || c == 0) {
      GECODE_ME_CHECK(x.eq(home,c));
      return home.ES_SUBSUMED(*this);
    }
    
    // bounds from y's to x
    FloatVal sl = c;
    for (int i = y.size(); i--; )
      sl *= y[i].val();
    GECODE_ME_CHECK(x.eq(home,sl));
    
    // x to y[0] if only one y left
    if (y.size() == 1) {
      GECODE_ME_CHECK(y[0].eq(home, (x.val()/c) ));
      if (x.assigned())
        return home.ES_SUBSUMED(*this);
    }
        
    ExecStatus es = ES_FIX;
    
    // x to y's if one or less zero y's
    int zeroes = 0;
    for (int i = y.size(); i--; )
      zeroes += (y[i].min() <= 0);
    
    if (zeroes == 0) {
      for (int i = y.size(); i--; ) {
        // sl = c * prod_j y[j]. remove y[i] from that to get denominator
        FloatVal denom(sl.min()/y[i].min(), sl.max()/y[i].max());
        ModEvent me = y[i].eq(home, x.val()/denom);
        
        if (me_failed(me))
          return ES_FAILED;
        if (me_modified(me))
          es = ES_NOFIX;
      }
    }
    
    if (zeroes == 1) {
      FloatVal denom = c;
      int index = -1;
      for (int i = y.size(); i--; ) {
        if (y[i].min() > 0)
          denom *= y[i].val();
        else
          index = i;
      }
      ModEvent me = y[index].eq(home, x.val()/denom);
      
      if (me_failed(me))
        return ES_FAILED;
      if (me_modified(me))
        es = ES_NOFIX;
    }

    return es;
  }