package edu.ucla.belief.ace;


/**
 * A network potential.  The only purpose of a potential is to identify a
 * particular parameter weight in the network, which can be done by
 * identifying the potential and the position within the potential.  Note
 * that some parameters will not have correspodning weights.  See
 * {@link OnlineEngine OnlineEngine} for more information.  A potential
 * cannot be constructed directly and must be obtained from an OnlineEngine.
 * Each potential has a name and a finite number of positions.  If a
 * potential has N positions, then those positions are 0, 1, ..., N-1.  In
 * the network from which the potential originated, the potential was
 * associated with a list of network variables.  If this network was a
 * Bayesian network, then the potential's name is the same as the name of the
 * last associated variable.  The positions correspond to instantiations of
 * the associated variables in a standard way.  For example, if the variables
 * are A, B, C, in that order, and each of these variables has two values,
 * then the positions correspond to instantiations as follows:
 * <xmp>
 *   Pos  A  B  C
 *   ---  -  -  -
 *     0  0  0  0
 *     1  0  0  1
 *     2  0  1  0
 *     3  0  1  1
 *     4  1  0  0
 *     5  1  0  1
 *     6  1  1  0
 *     7  1  1  1
 * </xmp>
 * This class conforms to the immutable design pattern.  
 * 
 * @author Mark Chavira
 */

public class Potential {

  //=========================================================================
  // Private
  //=========================================================================  

  /**
   * The potential name.
   */
  
  private String fName;
  
  /**
   * The domain names.
   */
  
  private int fNumPositions;
  
  //=========================================================================
  // Protected
  //=========================================================================  

  /**
   * Constructs a potential from the given name and given number of
   * positions.
   * 
   * @param name the given name.
   * @param numPositions the given number of positions.
   */
  
  protected Potential (String name, int numPositions) {
    fName = name;
    fNumPositions = numPositions;
  }
  
  //=========================================================================
  // Public
  //=========================================================================  

  /**
   * Returns the potential's name.  This method runs in constant time.
   * 
   * @return the variable's name.
   */
  
  public String name () {
    return fName;
  }
  
  /**
   * Returns the number of positions.  This method runs in constant time.
   * 
   * @return the number of positions.
   */
  
  public int numPositions () {
    return fNumPositions;
  }
  
  /**
   * @see java.lang.Object
   */
  
  public String toString () {
    return "T (" + name () + ")";
  }

}
