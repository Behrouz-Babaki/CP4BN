package edu.ucla.belief.ace;
import java.util.*;

/**
 * A finitely-valued network variable.  A variable cannot be constructed
 * directly and must be obtained from an OnlineEngine.  Each variable has a
 * name and a finite number of values, each of which has a name.  If a
 * variable has N values, then those values are 0, 1, ..., N-1.  The variable
 * name, number of values, order of values, and value names, will be the same
 * as in the network from which the variable originated.
 * <p>
 * This class conforms to the immutable design pattern.
 * 
 * @author Mark Chavira
 */

public class Variable {

  //=========================================================================
  // Private
  //=========================================================================  

  /**
   * The variable name.
   */
  
  private String fName;
  
  /**
   * The domain names.
   */
  
  private List<String> fDomainNames;
  
  //=========================================================================
  // Protected
  //=========================================================================  

  /**
   * Constructs a variable from the given name and given value names.
   * 
   * @param name the given name.
   * @param domainNames the given value names.
   */
  
  protected Variable (String name, String[] domainNames) {
    fName = name;
    fDomainNames = new ArrayList<String> ();
    for (String n : domainNames) {fDomainNames.add (n);}
    fDomainNames = Collections.unmodifiableList (fDomainNames);
  }
  
  //=========================================================================
  // Public
  //=========================================================================  

  /**
   * Returns the variable's name.  This method runs in constant time.
   * 
   * @return the variable's name.
   */
  
  public String name () {
    return fName;
  }
  
  /**
   * Returns an <em>unmodifiable</em> list of the value names.  The name of
   * value i will be the ith string in the list.  This method runs in
   * constant time.
   * 
   * @return the list.
   */
  
  public List<String> domainNames () {
    return fDomainNames;
  }
  
  /**
   * @see java.lang.Object
   */
  
  public String toString () {
    return name ();
  }

}
