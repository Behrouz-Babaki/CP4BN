package lim.enumerator;

import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.nio.file.Paths;
import java.util.ArrayList;

import edu.ucla.belief.ace.Evidence;
import edu.ucla.belief.ace.OnlineEngine;
import edu.ucla.belief.ace.Variable;

public class LIEnumerator {

	static int[] domainCnt;
	static int[] valIndices;
	static int numVars;

	public static void main(String[] args) {
		if (!args[0].endsWith(".net.ac") || !args[1].endsWith(".net.lmap")) {
			System.out.println("Usage: LIEnumerator [ac file] [lmap file]");
			System.exit(1);
		}
		DataOutputStream outFile = null;
		PrintWriter varValOutFile = null;
		OnlineEngine inferenceEngine = null;

		try {

			String fileName = Paths.get(args[0]).toAbsolutePath().toString();
			String attrFileName = fileName.replace(".net.ac", ".net.attr");
			String varValFileName = fileName.replace(".net.ac", ".net.varval");
			outFile = new DataOutputStream(new FileOutputStream(attrFileName));
			varValOutFile = new PrintWriter(new FileWriter(varValFileName));

			inferenceEngine = new OnlineEngine(args[0], args[1]);
			Evidence evidence = new Evidence(inferenceEngine);
			ArrayList<Variable> varArray = new ArrayList<Variable>(
					inferenceEngine.variables());
			numVars = varArray.size();

			varValOutFile.println(numVars);
			for (Variable currentVar : varArray) {
				varValOutFile.println(currentVar.name());
				int currentValNum = currentVar.domainNames().size();
				varValOutFile.println(currentValNum);
				for (String currentVal : currentVar.domainNames())
					varValOutFile.println(currentVal);
			}
			varValOutFile.close();

			domainCnt = new int[numVars];
			valIndices = new int[numVars];
			for (int counter = 0; counter < numVars; counter++) {
				Variable currentVar = varArray.get(counter);
				domainCnt[counter] = currentVar.domainNames().size();
				valIndices[counter] = 0;
			}

			ArrayList<Integer> changedVarIds = new ArrayList<Integer>();
			for (int counter = 0; counter < numVars; counter++)
				changedVarIds.add(counter);
			do {
				for (int varId : changedVarIds) {
					evidence.varRetract(varArray.get(varId));
					if (valIndices[varId] < domainCnt[varId])
						evidence.varCommit(varArray.get(varId),
								valIndices[varId]);
				}
				inferenceEngine.assertEvidence(evidence, false);
				double prob = inferenceEngine.probOfEvidence();
				outFile.writeDouble(prob);
				// for (int counter = 0; counter < numVars; counter++)
				// System.out.print(valIndices[counter] + " ");
				// System.out.println(" --> " + prob);

			} while (getNextIndex(changedVarIds));
			outFile.close();

		} catch (FileNotFoundException e) {
			e.printStackTrace();
			System.exit(1);
		} catch (Exception e) {
			e.printStackTrace();
			System.exit(1);
		}
	}

	static boolean getNextIndex(ArrayList<Integer> changedVars) {
		changedVars.clear();
		boolean notYet = true;
		for (int counter = numVars - 1; notYet && counter >= 0; counter--) {
			if (valIndices[counter] < domainCnt[counter]) {
				valIndices[counter]++;
				changedVars.add(counter);
				notYet = false;
			} else if (counter != 0) {
				valIndices[counter] = 0;
				changedVars.add(counter);
			} else
				return false;
		}
		return true;
	}
}
