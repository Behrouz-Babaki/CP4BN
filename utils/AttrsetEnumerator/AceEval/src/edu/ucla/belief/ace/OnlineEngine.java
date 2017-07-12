package edu.ucla.belief.ace;
import java.io.*;
import java.util.*;

/**
 * An inference engine based on an arithmetic circuit compiled for a Bayesian
 * network.  Once constructed, an OnlineEngine can answer multiple
 * probabilistic queries efficiently.  For given evidence e, we can compute
 * Pr (e) by performing a bottom-up traversal of the circuit, called
 * evaluating the circuit, in time that is linear in the size of the circuit.
 * We can also compute answers to many additional queries by performing a
 * second top-down pass, called differentiating the circuit, again in time
 * that is linear in the size of the circuit.  All of these queries can
 * therefore be computed in time that is linear in the size of the circuit,
 * though the constant factor can be significantly smaller when only
 * evaluating the circuit.  Moreover, this process may be repeated for as
 * many different evidence sets as desired.
 * <p>
 * When only evaluating the circuit and not differentiating, and when there
 * is a need to evaluate multiple times, it is possible to improve efficiency
 * by refraining from evaluating parts of the circuit known to have not
 * changed since the previous evaluation.  This behavior is not implemented
 * currently in this class. 
 * 
 * @author Mark Chavira
 */

public class OnlineEngine {

  //=========================================================================
  // Fields that define the arithmetic circuit.  Space becomes a serious
  // issue because of the large number of nodes and edges.  We therefore use
  // parallel arrays and byte rather than objects and enumerated values,
  // respectively.  Nodes are 0..(numNodes-1).  Edges are 0..(numEdges-1).
  //=========================================================================

  /**
   * A constant that identifies a node as being a constant.
   */

  protected static final byte CONSTANT = 0;

  /**
   * A constant that identifies a node as being a literal.
   */

  protected static final byte LITERAL = 1;

  /**
   * A constant that identifies a node as being a multiply node.
   */

  protected static final byte MULTIPLY = 2;

  /**
   * A constant that identifies a node as being an addition node.
   */

  protected static final byte ADD = 3;

  /**
   * A map from node to its type: CONSTANT, LITERAL, MULTIPLY, or ADD.  This
   * field is set when the engine is constructed and does not change
   * thereafter.
   */

  protected byte[] fNodeToType;

  /**
   * A map from node to the index of its last edge plus one.  If node n is a
   * sink and n = 0, then last[n] is 0; if node n is a sink and n \ne 0, then
   * last[n] = last[n-1]; if node n is not a sink, then last[n] is the last
   * edge outgoing n plus one.  This field is set when the engine is
   * constructed and does not change thereafter.
   */
   
  protected int[] fNodeToLastEdge;

  /**
   * A map from node to its literal.  A mapping for a node that is not a
   * literal is undefined.  This field is set when the engine is constructed
   * and does not change thereafter.
   */

  protected int[] fNodeToLit;
  
  /**
   * A map from edge to the node the edge enters.  This field is set when the
   * engine is constructed and does not change thereafter.
   */

  protected int[] fEdgeToTailNode;
  
  /**
   * A map from arithmetic circuit variable to the one node that contains the
   * variable's negative literal or to -1 if no node contains it.  Mapping
   * from 0 is undefined.  This field is set when the engine is constructed
   * and does not change thereafter.
   */
  
  protected int[] fVarToNegLitNode;

  /**
   * A map from arithmetic circuit variable to the one node that contains the
   * variable's positive literal or to -1 if no node contains it.  Mapping
   * from 0 is undefined.  This field is set when the engine is constructed
   * and does not change thereafter.
   */
  
  protected int[] fVarToPosLitNode;

  //=========================================================================
  // Fields that define the mapping between the source domain and the
  // arithmetic circuit domain.
  //=========================================================================

  /**
   * The pattern used to separate fields within a line when reading a literal
   * map file.
   */
  
  protected static final String READ_DELIMITER = "\\$";
  
  /**
   * The string to use to delimit fields within a line when writing a literal
   * map file.
   */
  
  protected static final String DELIMITER = "$";

  /**
   * An unmodifiable set of the source variables.
   */
  
  protected Set<Variable> fVariables;
  
  /**
   * An unmodifiable set of the source potentials.
   */
  
  protected Set<Potential> fPotentials;
  
  /**
   * A map from name to source variable.
   */
  
  protected HashMap<String,Variable> fNameToSrcVar;
  
  /**
   * A map from name to source potential.
   */
  
  protected HashMap<String,Potential> fNameToSrcPot;
  
  /**
   * A map from logic variable to default weight of its negative literal.
   * Map from 0  is undefined.
   */
  
  protected double[] fLogicVarToDefaultNegWeight;

  /**
   * A map from logic variable to default weight of its positive literal.
   * Map from 0 is undefined.
   */
  
  protected double[] fLogicVarToDefaultPosWeight;

  /**
   * A map from source variable to value to indicator.
   */
  
  protected HashMap<Variable,int[]> fSrcVarToSrcValToIndicator;
  
  /**
   * A map from source potetial to position to parameter.  Some positions
   * will not have a parameter and some positions will share a parameter.
   * We store 0 for these.
   */
  
  protected HashMap<Potential,int[]> fSrcPotToSrcPosToParameter;
  
  //=========================================================================
  // Fields that are used for performing the upward and downward pass.
  //=========================================================================

  /**
   * This object plays two roles.  If upwardPass () is called, then a map
   * from each node n to n's value.  If twoPasess () is called, then the
   * same, except when a MULTIPLY node where exactly one child has value 0,
   * maps to the product of the nonzero children.  Set in upwardPass () and
   * twoPasses ().
   */
   
  protected double[] fNodeToValue;

  /**
   * Maps each node to its derivative.  Set in twoPasses ().
   */

  protected double[] fNodeToDerivative;

  /**
   * Maps each node to whether it is an AND node and has exactly one child
   * whose value is zero.  Set in twoPasses ().
   * 
   */

  protected boolean[] fNodeToOneZero;

  /**
   * Whether the upward pass has completed.  This field is cleared when the
   * engine is constructed and set in assertEvidence ().
   */
  
  protected boolean fUpwardPassCompleted;

  /**
   * Whether the two passes have completed.  This field is cleared when the
   * engine is constructed, cleared by assertEvidence () when performing only
   * the upward pass, and set by assertEvidence () when performing both
   * passes.
   */
    
  protected boolean fTwoPassesCompleted;
  
  /**
   * The most recently asserted weights for negative literals.
   */
  
  protected double[] fAcVarToMostRecentNegWeight;
  
  /**
   * The most recently asserted weights for positive literals.
   */

  protected double[] fAcVarToMostRecentPosWeight;

  //=========================================================================
  // Private methods
  //=========================================================================

  /**
   * Clones the given double[] and returns the result.  Java's array clone is
   * known to be slow, so I wrote my own.
   * 
   * @param a the given array.
   * @return the clone.
   */
  
  protected static double[] clone (double[] a) {
    double[] ans = new double[a.length];
    System.arraycopy (a, 0, ans, 0, a.length);
    return ans;
  }
  
  /**
   * Reads the arithmetic circuit from the given reader.
   * 
   * @param br the given reader.
   */  
  
  protected void readArithmeticCircuit (Reader r) throws Exception {

    // Prepare to parse.

    BufferedReader br =
      r instanceof BufferedReader ?
      (BufferedReader)r : new BufferedReader (r);
    java.util.regex.Pattern p = java.util.regex.Pattern.compile ("\\s+");
    int numNodes = Integer.MAX_VALUE;
    int nextEdge = 0;
    int nextNode = 0;
    
    // Process each line.
    
    while (nextNode < numNodes) {
      
      // Read the line.  Quit if eof.  Skip if comment or blank line.
      // Otherwise, split into tokens.
      
      String line = br.readLine ();
      if (line == null) {break;} // eof
      if (line.startsWith ("c")) {continue;} // comment
      line = line.trim ();
      if (line.length () == 0) {continue;} // blank line
      String[] tokens = p.split (line);
      
      // A header line looks like: "nnf" numNodes numEdges numVars
      
      if (tokens[0].equals ("nnf")) {
        numNodes = Integer.parseInt (tokens[1]);
        int numEdges = Integer.parseInt (tokens[2]);
        int numAcVars = Integer.parseInt (tokens[3]);
        fEdgeToTailNode = new int[numEdges];
        fNodeToLastEdge = new int[numNodes];
        fNodeToType = new byte[numNodes];
        fNodeToLit = new int[numNodes];
        fVarToNegLitNode = new int[numAcVars + 1];
        fVarToPosLitNode = new int[numAcVars + 1];
        Arrays.fill (fVarToNegLitNode, -1);
        Arrays.fill (fVarToPosLitNode, -1);
        continue;
      }
      
      // This is not a header line, so it must be a node line, which looks
      // like one of the following:
      //   "A" numChildren child+
      //   "O" splitVar numChildren child+
      //   "L" literal
      
      char ch = tokens[0].charAt (0);
      if (ch == 'A') {
        fNodeToType[nextNode] = MULTIPLY;
        for (int chIndex = 2; chIndex < tokens.length; chIndex++) {
          fEdgeToTailNode[nextEdge++] = Integer.parseInt (tokens[chIndex]);
        }
      } else if (ch == 'O') {
        fNodeToType[nextNode] = ADD;
        for (int chIndex = 3; chIndex < tokens.length; chIndex++) {
          fEdgeToTailNode[nextEdge++] = Integer.parseInt (tokens[chIndex]);
        }
      } else /* ch == 'L' */ {
        fNodeToType[nextNode] = LITERAL;
        int l = Integer.parseInt (tokens[1]);
        fNodeToLit[nextNode] = l;
        (l < 0 ? fVarToNegLitNode : fVarToPosLitNode)[Math.abs (l)] =
          nextNode;
      }
      fNodeToLastEdge[nextNode] = nextEdge;
      nextNode++;
      
    }

  }
  
  /**
   * Reads the literal map from the given reader.
   * 
   * @param br the given reader.
   */  
  
  protected void readLiteralMap (Reader r) throws Exception {

    // Allocate some objects.
    
    fNameToSrcVar = new HashMap<String,Variable> ();
    fNameToSrcPot = new HashMap<String,Potential> ();
    fSrcVarToSrcValToIndicator = new HashMap<Variable,int[]> ();
    fSrcPotToSrcPosToParameter = new HashMap<Potential,int[]> ();
    
    // Prepare to parse.
    
    BufferedReader br =
      r instanceof BufferedReader ?
      (BufferedReader)r : new BufferedReader (r);
    java.util.regex.Pattern p =
      java.util.regex.Pattern.compile (READ_DELIMITER);
    int numLits = Integer.MAX_VALUE;
    int litsFinished = 0;

    // Process each line.
    
    while (litsFinished < numLits) {
      
      // Read the line.  Quit if eof.  Skip if comment (including blank
      // lines).  Otherwise, split into tokens.
      
      String line = br.readLine ();
      if (line == null) {break;} // eof
      if (!line.startsWith ("cc" + DELIMITER)) {continue;} // comment
      line = line.trim ();
      String[] tokens = p.split (line);
      
      // If the line is a header, it is of the form: "cc" "N" numLogicVars.

      String type = tokens[1];
      if (type.equals ("N")) {
        int n = Integer.parseInt (tokens[2]);
        numLits = n * 2;
        fLogicVarToDefaultNegWeight = new double[n + 1];
        fLogicVarToDefaultPosWeight = new double[n + 1];
        continue;
      }
      
      // If the line is a variable line, then it is of the form:
      // "cc" "V" srcVarName numSrcVals (srcVal)+

      if (type.equals ("V")) {
        String vn = tokens[2];
        int valCount = Integer.parseInt (tokens[3]);
        String[] valNames = new String[valCount];
        for (int i = 0; i < valCount; i++) {valNames[i] = tokens[4 + i];}
        Variable v = new Variable (vn, valNames);
        fSrcVarToSrcValToIndicator.put (v, new int[valCount]);
        fNameToSrcVar.put (vn, v);
        continue;
      }
      
      // If the line is a potential line, then it is of the form:
      // "cc" "T" srcPotName parameterCnt.
      
      if (type.equals ("T")) {
        String tn = tokens[2];
        int parmCount = Integer.parseInt (tokens[3]);
        Potential pot = new Potential (tn, parmCount);
        fSrcPotToSrcPosToParameter.put (pot, new int[parmCount]);
        fNameToSrcPot.put (tn, pot);
        continue;
      }

      // This is not a header line, a variable line, or potential line, so
      // it must be a literal description, which looks like one of the
      // following:
      //   "cc" "I" literal weight srcVarName srcValName srcVal
      //   "cc" "P" literal weight srcPotName pos+
      //   "cc" "A" literal weight
      
      int l = Integer.parseInt (tokens[2]);
      double w = Double.parseDouble (tokens[3]);
      (l < 0 ? fLogicVarToDefaultNegWeight : fLogicVarToDefaultPosWeight)
        [Math.abs (l)] = w;
      if (type.equals ("I")) {
        String vn = tokens[4];
        //String un = tokens[5];
        int u = Integer.parseInt (tokens[6]);
        fSrcVarToSrcValToIndicator.get (fNameToSrcVar.get (vn))[u] = l;
      } else if (type.equals ("P")) {
        String tn = tokens[4];
        String[] posStrings = tokens[5].split (",");
        if (posStrings.length == 1) {
          int pos = Integer.parseInt (posStrings[0]);
          fSrcPotToSrcPosToParameter.get (fNameToSrcPot.get (tn))[pos] = l;
        }
      } else if (type.equals ("A")) {
      } else {
        throw new Exception (
          "\"cc\" must be followed by " +
          "\"N\", \"V\", \"T\", \"I\", \"P\", or \"A\"");
      }
      ++litsFinished;
      
    }

    // Now create the variables, the map from variable name to variable, and
    // the map from variable to value to indicator.
    
    fVariables =
      Collections.unmodifiableSet (
        new HashSet<Variable> (fNameToSrcVar.values ()));
    fPotentials =
      Collections.unmodifiableSet (
        new HashSet<Potential> (fNameToSrcPot.values ()));
    
  }
    
  /**
   * Runs the upward pass.
   *
   * @param ev defines literal weights.
   */
  
  protected void upwardPass (Evidence ev) throws Exception {
    double[] negValues = ev.fVarToCurrentNegWeight;
    double[] posValues = ev.fVarToCurrentPosWeight;
    int numNodes = numAcNodes ();
    for (int n = 0; n < numNodes; n++) {
      byte type = fNodeToType[n];
      if (type == MULTIPLY) {
        double v = 1.0;
        int last = fNodeToLastEdge[n];
        for (int e = first (n); e < last; e++) {
          int ch = fEdgeToTailNode[e];
          double chVal = fNodeToValue[ch];
          if (chVal == 0.0) {v = 0.0; break;}
          v *= chVal;
          if (v == 0.0) {throw new UnderflowException ();}
        }
        fNodeToValue[n] = v;
      } else if (type == ADD) {
        double v = 0.0;
        int last = fNodeToLastEdge[n];
        for (int e = first (n); e < last; e++) {
          int ch = fEdgeToTailNode[e];
          v += fNodeToValue[ch];
        }
        fNodeToValue[n] = v;
      } else if (type == LITERAL) {
        int l = fNodeToLit[n];
        fNodeToValue[n] = (l < 0 ? negValues[-l] : posValues[l]);
      }
      // do nothing for a constant
    }
  }

  /**
   * Runs the two passes.
   *
   * @param negativeValues a map from variable to the value of its negative
   *   literal; index 0 is undefined. 
   * @param positiveValues a map from variable to the value of its positive
   *   literal; index 0 is undefined.
   */

  protected void twoPasses (Evidence ev) throws Exception {

    // Upward pass.
    
    double[] negValues = ev.fVarToCurrentNegWeight;
    double[] posValues = ev.fVarToCurrentPosWeight; 
    int numNodes = numAcNodes ();
    for (int n = 0; n < numNodes; n++) {
      fNodeToDerivative[n] = 0.0;
      byte type = fNodeToType[n];
      if (type == MULTIPLY) {
        int numZeros = 0;
        double v = 1.0;
        int last = fNodeToLastEdge[n];
        for (int e = first (n); e < last; e++) {
          int ch = fEdgeToTailNode[e];
          double chVal = computedValue (ch);
          if (chVal == 0.0) {
            if (++numZeros > 1) {v = 0; break;}
          } else {
            v *= chVal;
            if (v == 0.0) {new UnderflowException ();}
          }
        }
        fNodeToValue[n] = v;
        fNodeToOneZero[n] = numZeros == 1;
      } else if (type == ADD) {
        double v = 0.0;
        int last = fNodeToLastEdge[n];
        for (int e = first (n); e < last; e++) {
          int ch = fEdgeToTailNode[e];
          double chVal = computedValue (ch);
          v += chVal;
        }
        fNodeToValue[n] = v;
      } else if (type == LITERAL) {
        int l = fNodeToLit[n];
        fNodeToValue[n] = (l < 0 ? negValues[-l] : posValues[l]);
      }
      // do nothing for a constant
    }

    // Downward pass.
    
    fNodeToDerivative[numNodes - 1] = 1.0;
    for (int n = numNodes - 1; n >= 0; n--) {
      byte type = fNodeToType[n];
      if (type == LITERAL || type == CONSTANT) {continue;}
      int last = fNodeToLastEdge[n];
      if (type == MULTIPLY) {
        double value = fNodeToValue[n];
        if (value == 0.0) {continue;} // more than one zero
        double x = fNodeToDerivative[n];
        if (x == 0.0) {continue;}
        x *= value;
        if (x == 0.0) {throw new UnderflowException ();}
        if (fNodeToOneZero[n]) { // exactly one zero
          for (int e = first (n); e < last; e++) {
            int ch = fEdgeToTailNode[e];
            double chVal = computedValue (ch);
            if (chVal == 0.0) {fNodeToDerivative[ch] += x; break;}
          }
        } else { // no zeros
          for (int e = first (n); e < last; e++) {
            int ch = fEdgeToTailNode[e];
            double chVal = computedValue (ch);
            fNodeToDerivative[ch] += x / chVal;
          }
        }
      } else /* PLUS NODE */ {
        double x = fNodeToDerivative[n];
        for (int e = first (n); e < last; e++) {
          int ch = fEdgeToTailNode[e];
          fNodeToDerivative[ch] += x;
        }
      }
    }
    
  }

  /**
   * Returns the value of the node with the given index.
   * 
   * @param n the given index.
   * @return the value.
   */
  
  protected double computedValue (int n) {
    return fNodeToOneZero[n] ? 0 : fNodeToValue[n];
  }

  /**
   * Returns the first edge belonging to the given node.
   * 
   * @param n the given node.
   * @return the edge.
   */
  
  protected int first (int n) {
    return (n == 0) ? 0 : fNodeToLastEdge[n-1];
  }
  
  /**
   * Returns the root node.
   * 
   * @return the root.
   */
  
  protected int rootNode () {
    return fNodeToType.length - 1;
  }
  
  /**
   * Returns the number of nodes.
   * 
   * @return the number.
   */
  
  protected int numAcNodes () {
    return fNodeToType.length;
  }

  //=========================================================================
  // Construction
  //=========================================================================

  /**
   * Reads an arithmetic circuit from the file with the first given name,
   * reads a literal map from the file with the second given name, and
   * constructs an online engine linked to the read arithmetic circuit and to
   * the read literal map.  This method runs in time that is linear in the
   * size of the arithmetic circuit.  However, the constant factor is large,
   * since data is read from disk.
   * 
   * @param acFilename the first given name.
   * @param lmFilename the second given name.
   */
  
  public OnlineEngine (String acFilename, String lmFilename)
   throws Exception {
    this (
      new BufferedReader (new FileReader (acFilename)),
      new BufferedReader (new FileReader (lmFilename)));
  }
  
  /**
   * Reads an online engine from the first given reader, reads a literal map
   * from the second given reader, and constructs an online engine linked to
   * the read arithmetic circuit and to the read literal map.  This method
   * runs in time that is linear in the size of the arithmetic circuit.
   * However, the constant factor is large, since data is read from disk.
   * 
   * @param acReader the first given reader.
   * @param lmReader the second given reader.
   */
  
  public OnlineEngine (Reader acReader, Reader lmReader) throws Exception {
    readArithmeticCircuit (acReader);
    readLiteralMap (lmReader);
    fNodeToValue = new double[numAcNodes ()];
    fNodeToDerivative = new double[numAcNodes ()];
    fNodeToOneZero = new boolean[numAcNodes ()];
    fUpwardPassCompleted = false;
    fTwoPassesCompleted = false;
  }
  
  //=========================================================================
  // Retrieving variables and potentials
  //=========================================================================
  
  /**
   * Returns the variable having the given name.  This method is fairly
   * efficient, merely performing a lookup in a HashMap using a string key.
   * However, there is some overhead, so if at all possible, all calls to
   * this method should be made during initialization, and the method should
   * be called at most once per variable rather than once (or multiple times)
   * per evidence set.  This method runs in time that is linear in the size
   * of the given name.
   * 
   * @param n the given name.
   * @return the corresponding variable.
   */
  
  public Variable varForName (String n) {
    return fNameToSrcVar.get (n);
  }
  
  /**
   * Returns the potential having the given name.  This method is fairly
   * efficient, merely performing a lookup in a HashMap using a string key.
   * However, there is some overhead, so if at all possible, all calls to
   * this method should be made during initialization, and the method should
   * be called at most once per potential rather than once (or multiple
   * times) per evidence set.  This method runs in time that is linear in the
   * size of the given name.
   * 
   * @param n the given name.
   * @return the corresponding potential.
   */
  
  public Potential potForName (String n) {
    return fNameToSrcPot.get (n);
  }
  
  /**
   * Returns an <em>unmodifiable</em> set of the variables.  This method runs
   * in constant time.
   * 
   * @return the variables.
   */
  
  public Set<Variable> variables () {
    return fVariables;
  }
  
  /**
   * Returns an <em>unmodifiable</em> set of the potentials.  This method
   * runs in constant time.
   * 
   * @return the variables.
   */
  
  public Set<Potential> potentials () {
    return fPotentials;
  }

  //=========================================================================
  // Asserting evidence
  //=========================================================================
  
  /**
   * Evaluates the arithmetic circuit under the given evidence, computing
   * probability of evidence; if the given flag is set, also differentiates
   * the arithmetic circuit.  The results of the upward pass may be retrieved
   * by calling {@link #probOfEvidence probOfEvidence()}.  The result of the
   * second pass may be retrieved by calling any of the following:
   * <ul>
   * <li> {@link #varPartials(Variable) varPartials(Variable)}
   * <li> {@link #varMarginals(Variable) varMarginals(Variable)}
   * <li> {@link #varPosteriors(Variable) varPosteriors(Variable)}
   * <li> {@link #potPartials(Potential) potPartials(Potential)}
   * <li> {@link #potMarginals(Potential) potMarginals(Potential)}
   * <li> {@link #potPosteriors(Potential) potPosteriors(Potential)}
   * <li> {@link #varPartials(Set) varPartials(Set)}
   * <li> {@link #varMarginals(Set) varMarginals(Set)}
   * <li> {@link #varPosteriors(Set) varPosteriors(Set)}
   * <li> {@link #potPartials(Set) potPartials(Set)}
   * <li> {@link #potMarginals(Set) potMarginals(Set)}
   * <li> {@link #potPosteriors(Set) potPosteriors(Set)}
   * </ul>
   * This method is the only publically available method that performs online
   * inference.  The method runs in time that is linear in the size of the
   * arithmetic circuit.  The constant factor is larger when performing the
   * second pass.
   * 
   * @param e the given evidence.
   * @param secondPass the given flag.
   */
  
  public void assertEvidence (Evidence e, boolean secondPass)
   throws Exception {
    if (secondPass) {
      fAcVarToMostRecentNegWeight = clone (e.fVarToCurrentNegWeight);
      fAcVarToMostRecentPosWeight = clone (e.fVarToCurrentPosWeight);
      twoPasses (e);
    } else {
      fAcVarToMostRecentNegWeight = null;
      fAcVarToMostRecentPosWeight = null;
      upwardPass (e);
    }
    fUpwardPassCompleted = true;
    fTwoPassesCompleted = secondPass;
  }

  //=========================================================================
  // Retrieving probability of evidence
  //=========================================================================
  
  /**
   * Returns Pr (e) where e is defined by the most recent invocation of
   * assertEvidence ().  assertEvidence () must have been called prior to
   * calling this method.  The method performs no inference.  It merely
   * retrieves the results of inference performed when assertEvidence () was
   * invoked.  This method runs in constant time.
   * 
   * @return the probability of evidence.
   */
  
  public double probOfEvidence () throws Exception {
    if (!fUpwardPassCompleted) {
      throw new Exception ("assertEvidence () must be called!");
    }
    int root = rootNode ();
    return fTwoPassesCompleted ? computedValue (root) : fNodeToValue[root];
  }
  
  //=========================================================================
  // Retrieving indicator results.
  //=========================================================================
  
  /**
   * For each value of the given network variable, returns the partial
   * derivative of the ac with respect to the indicator for this value,
   * according to the most recent invocation of assertEvidence ().  The
   * result is in the form of a double[] that maps network variable value to
   * partial derivative.  This method runs in time that is linear in the
   * domain size of the given network variable.
   * <p>
   * assertEvidence () must have been called prior to calling this method,
   * and the most recent invocation of assertEvidence () must have set the
   * secondPass flag.  This method performs no inference.  It merely
   * retrieves the results of inference performed when assertEvidence () was
   * invoked.  This method runs in time that is linear in the domain size of
   * the given network variable.
   *  
   * @param v the given network variable.
   * @return the posterior marginal.
   */
  
  public double[] varPartials (Variable v) throws Exception {
    if (!fTwoPassesCompleted) {
      throw new Exception (
        "assertEvidence () must be called with second pass flag set!");
    }
    double[] ans = new double[v.domainNames ().size ()];
    int[] inds = fSrcVarToSrcValToIndicator.get (v);
    for (int u = 0; u < ans.length; u++) {
      int l = inds[u];
      ans[u] =
        (l < 0) ?
        fNodeToDerivative[fVarToNegLitNode[-l]] :
        fNodeToDerivative[fVarToPosLitNode[l]];
    }
    return ans;
  }
  
  /**
   * Invokes {@link #varPartials(Variable) varPartials(Variable)} for each
   * given variable and returns the results.
   * 
   * @param vs the given set of network variables.
   * @return a map from each of the given network variables to its marginal.
   */
  
  public HashMap<Variable,double[]> varPartials (Set<Variable> vs)
   throws Exception {
    HashMap<Variable,double[]> ans = new HashMap<Variable,double[]> ();
    for (Variable v : vs) {ans.put (v, varPartials (v));}
    return ans;
  }

  /**
   * Returns the marginal Pr (X, e) for the given network variable X, where
   * e is defined by the most recent invocation of assertEvidence ().  The
   * result is in the form of a double[] that maps network variable value to
   * probability.  This method runs in time that is linear in the domain size
   * of the given network variable.
   * <p>
   * assertEvidence () must have been called prior to calling this method,
   * and the most recent invocation of assertEvidence () must have set the
   * secondPass flag.  This method performs no inference.  It merely
   * retrieves the results of inference performed when assertEvidence () was
   * invoked.  This method runs in time that is linear in the domain size of
   * the given network variable.
   *  
   * @param v the given network variable.
   * @return the marginal.
   */
  
  public double[] varMarginals (Variable v) throws Exception {
    if (!fTwoPassesCompleted) {
      throw new Exception (
        "assertEvidence () must be called with marginals flag set!");
    }
    double[] ans = new double[v.domainNames ().size ()];
    int[] inds = fSrcVarToSrcValToIndicator.get (v);
    for (int u = 0; u < ans.length; u++) {
      int l = inds[u];
      ans[u] =
        (l < 0) ?
        fAcVarToMostRecentNegWeight[-l] *
        fNodeToDerivative[fVarToNegLitNode[-l]] :
        fAcVarToMostRecentPosWeight[l] *
        fNodeToDerivative[fVarToPosLitNode[l]];
    }
    return ans;
  }
  
  /**
   * Invokes {@link #varMarginals(Variable) varMarginals(Variable)} for each
   * given variable and returns the results.
   *  
   * @param vs the given set of network variables.
   * @return a map from each of the given network variables to its marginal.
   */
  
  public HashMap<Variable,double[]> varMarginals (Set<Variable> vs)
   throws Exception {
    HashMap<Variable,double[]> ans = new HashMap<Variable,double[]> ();
    for (Variable v : vs) {ans.put (v, varMarginals (v));}
    return ans;
  }

  /**
   * Returns the posterior marginal Pr (X | e) for the given network variable
   * X, where e is defined by the most recent invocation of
   * assertEvidence ().  The result is in the form of a double[] that maps
   * network variable value to probability.  This method runs in time that is
   * linear in the domain size of the given variable.
   * <p>
   * assertEvidence () must have been called prior to calling this method,
   * and the most recent invocation of assertEvidence () must have set the
   * secondPass flag.  This method performs no inference.  It merely
   * retrieves the results of inference performed when assertEvidence () was
   * invoked.  This method runs in time that is linear in the domain size of
   * the given network variable.
   *  
   * @param v the given network variable.
   * @return the posterior marginal.
   */
  
  public double[] varPosteriors (Variable v) throws Exception {
    double pe = probOfEvidence ();
    double[] ans = varMarginals (v);
    for (int i = 0; i < ans.length; i++) {ans[i] /= pe;}
    return ans;
  }
  
  /**
   * Invokes {@link #varPosteriors(Variable) varPosteriors(Variable)} for
   * each given variable and returns the results.
   *  
   * @param vs the given set of network variables.
   * @return a map from each of the given network variables to its marginal.
   */
  
  public HashMap<Variable,double[]> varPosteriors (Set<Variable> vs)
   throws Exception {
    HashMap<Variable,double[]> ans = new HashMap<Variable,double[]> ();
    for (Variable v : vs) {ans.put (v, varPosteriors (v));}
    return ans;
  }

  //=========================================================================
  // Retrieving parameter results.
  //=========================================================================
  
  /**
   * For each position of the given network potential, returns the partial
   * derivative of the ac with respect to the parameter for this position,
   * according to the most recent invocation of assertEvidence ().  The
   * result is in the form of a double[] that maps position to partial
   * derivative.  This method runs in time that is linear in the number of
   * positions of the given potential.
   * <p>  
   * assertEvidence () must have been called prior to calling this method,
   * and the most recent invocation of assertEvidence () must have set the
   * secondPass flag.  This method performs no inference.  It merely
   * retrieves the results of inference performed when assertEvidence () was
   * invoked.  Warning: certain encodings that make use of local structure
   * suppress the generation of a parameter variable for some positions or
   * are capable of producing a parameter variable that represents multiple
   * positions.  Obtaining derivatives for these positions is not possible,
   * and Double.NaN will be returned instead.
   */
  
  public double[] potPartials (Potential pot) throws Exception {
    int[] parms = fSrcPotToSrcPosToParameter.get (pot);
    double[] ans = new double[parms.length];
    for (int pos = 0; pos < ans.length; pos++) {
      int l = parms[pos];
      ans[pos] =
        l == 0  ? Double.NaN :
        l < 0 ? fNodeToDerivative[fVarToNegLitNode[-l]] :
        fNodeToDerivative[fVarToPosLitNode[l]];
    }
    return ans;
  }
  
  /**
   * Invokes {@link #potPartials (Potential) potPartials(Potential)} for each
   * given potential and returns the results.
   * 
   * @param ps the given potentials.
   * @return the derivatives.
   */
  
  public HashMap<Potential,double[]> potPartials (
   Set<Potential> ps) throws Exception {
    HashMap<Potential,double[]> ans = new HashMap<Potential,double[]> ();
    for (Potential p : ps) {ans.put (p, potPartials (p));}
    return ans;
  }
  
  /**
   * For the given potential T, returns P (T, e) where e is defined by the
   * most recent invocation of assertEvidence ().  The result is in the form
   * of a double[] that maps position to marginal.  This method runs in time
   * that is linear in the number of positions of the given potential.
   * <p>  
   * assertEvidence () must have been called prior to calling this method,
   * and the most recent invocation of assertEvidence () must have set the
   * secondPass flag.  This method performs no inference.  It merely
   * retrieves the results of inference performed when assertEvidence () was
   * invoked.  Warning: certain encodings that make use of local structure
   * suppress the generation of a parameter variable for some positions or
   * are capable of producing a parameter variable that represents multiple
   * positions.  Obtaining derivatives for these positions is not possible,
   * and Double.NaN will be returned instead.
   *  
   * @param p the given potential.
   * @return the margtinal.
   */
  
  public double[] potMarginals (Potential p) throws Exception {
    int[] parms = fSrcPotToSrcPosToParameter.get (p);
    double[] ans = potPartials (p);
    for (int pos = 0; pos < ans.length; pos++) {
      int l = parms[pos];
      if (!Double.isNaN (ans[pos])) {
        ans[pos] *=
          l < 0 ?
          fAcVarToMostRecentNegWeight[-l] : fAcVarToMostRecentPosWeight[l];
      }
    }
    return ans;
  }
  
  /**
   * Invokes {@link #potMarginals (Potential) potMarginals(Potential)} for
   * each given potential and returns the results.
   * 
   * @param ps the given potentials.
   * @return the marginals.
   */
  
  public HashMap<Potential,double[]> potMarginals (Set<Potential> ps)
   throws Exception {
    HashMap<Potential,double[]> ans = new HashMap<Potential,double[]> ();
    for (Potential p : ps) {ans.put (p, potMarginals (p));}
    return ans;
  }

  /**
   * For the given potential T, returns P (T | e) where e is defined by the
   * most recent invocation of assertEvidence ().  The result is in the form
   * of a double[] that maps position to posterior marginal.  This method
   * runs in time that is linear in the number of positions of the given
   * potential.
   * <p>  
   * assertEvidence () must have been called prior to calling this method,
   * and the most recent invocation of assertEvidence () must have set the
   * secondPass flag.  This method performs no inference.  It merely
   * retrieves the results of inference performed when assertEvidence () was
   * invoked.  Warning: certain encodings that make use of local structure
   * suppress the generation of a parameter variable for some positions or
   * are capable of producing a parameter variable that represents multiple
   * positions.  Obtaining derivatives for these positions is not possible,
   * and Double.NaN will be returned instead.
   * 
   * @param v the given variable.
   * @return the posterior marginal.
   */
  
  public double[] potPosteriors (Potential p)
   throws Exception {
    double[] ans = potMarginals (p);
    double pe = probOfEvidence ();
    for (int pos = 0; pos < ans.length; pos++) {
      if (!Double.isNaN (ans[pos])) {ans[pos] /= pe;}
    }
    return ans;
  }
  
  /**
   * Invokes {@link #potPosteriors (Potential) potPosteriors(Potential)} for
   * each given potentials and returns the results.
   * 
   * @param vs the given variables.
   * @return the posterior marginals.
   */
  
  public HashMap<Potential,double[]> potPosteriors (
   Set<Potential> ps) throws Exception {
    HashMap<Potential,double[]> ans = new HashMap<Potential,double[]> ();
    for (Potential p : ps) {ans.put (p, potPosteriors (p));}
    return ans;
  }

}
