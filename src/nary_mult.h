#pragma GCC diagnostic ignored "-Wunused-local-typedefs"

#include <gecode/float.hh>  
  
using namespace Gecode;
using namespace Gecode::Float;
  
  /**
   * \brief Base-class for n-ary mult propagators
   *
   * Positive views are of type \a P, negative views not supported (yet?).
   */
  template<class P>
  class NaryMultPlus : public Propagator {
  protected:
    FloatView x;
    /// Array of positive views
    ViewArray<P> y;
    /// Constant value
    FloatVal c;
    
    /// Constructor for cloning \a p
    NaryMultPlus(Space& home, bool share, NaryMultPlus<P>& p);
  public:
    /// Constructor for creation
    NaryMultPlus(Home home, FloatView& x, ViewArray<P>& y, FloatVal c);
    /// Cost function (defined as low linear)
    virtual PropCost cost(const Space& home, const ModEventDelta& med) const;
    /// Delete propagator and return its size
    virtual size_t dispose(Space& home);
    /// Create copy during cloning
    virtual Actor* copy(Space& home, bool share);
    /// Perform propagation
    virtual ExecStatus propagate(Space& home, const ModEventDelta& med);
    /// Post propagator for x = prod(y) * c
    static ExecStatus
    post(Home home, FloatView& x, ViewArray<P>& y, FloatVal c);
  };
  
  void nary_mult(Home home, FloatVar x0, const FloatVarArgs& y0, FloatVal c0=1);
