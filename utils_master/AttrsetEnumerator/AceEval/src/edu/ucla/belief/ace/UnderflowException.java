package edu.ucla.belief.ace;


/**
 * An exception that {@link OnlineEngine#assertEvidence
 * OnlineEngine.assertEvidence()} throws when underflow occurs.
 * 
 * @author Mark Chavira
 */

public class UnderflowException extends Exception {

  /**
   * Just to get rid of warning.
   */
  
  private static final long serialVersionUID = 1L;

  /**
   * A constructor that accepts no parameters.
   */
  
  public UnderflowException () {}
  
}
