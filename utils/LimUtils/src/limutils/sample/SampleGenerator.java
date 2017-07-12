package limutils.sample;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;

import edu.ucla.belief.BeliefNetwork;
import edu.ucla.belief.StateNotFoundException;
import edu.ucla.belief.io.NetworkIO;
import edu.ucla.belief.io.hugin.HuginNode;
import edu.ucla.belief.learn.LearningData;
import edu.ucla.belief.learn.Simulator;

public class SampleGenerator {

	private BeliefNetwork BN;
	private Simulator Sim;
	private LimNamesReader namesReader;

	public SampleGenerator(String netFileName, String namesFileName) {
		File file = new File(netFileName);
		this.BN = null;
		try {
			BN = NetworkIO.readHuginNet(file);
		} catch (Exception e) {
			System.out.println("error reading network");
			e.printStackTrace();
			System.exit(1);
		}
		this.Sim = new Simulator(BN);
		this.namesReader = new LimNamesReader(namesFileName);

	}

	public ArrayList<ArrayList<Integer>> sample(int numCases) {
		ArrayList<ArrayList<Integer>> samples = new ArrayList<ArrayList<Integer>>();
		LearningData LData = null;
		try {
			LData = Sim.simulate(numCases, 0);
		} catch (StateNotFoundException e) {
			System.out.println("state not found");
			e.printStackTrace();
			System.exit(1);
		}

		ArrayList<String> var_names = namesReader.get_var_names();
		int num_vars = var_names.size();
		ArrayList<HashMap<String, Integer>> val_maps = namesReader.get_val_maps();
		
		int current_record = LData.getCurrentRecord();
		while (current_record < LData.numRecords()) {
			ArrayList<Integer> currentSample = new ArrayList<Integer>(num_vars);
			for (int var_counter = 0; var_counter < num_vars; var_counter++)
				currentSample.add(-1);
			for (Object var : LData.variables()) {
				HuginNode node = (HuginNode) (var);
				String current_var_name = node.getID();
				String current_val_name = LData.get(node).toString();
				int varIndex = var_names.indexOf(current_var_name);
				int valIndex = val_maps.get(varIndex).get(current_val_name);
				currentSample.set(varIndex, valIndex);
			}
			samples.add(currentSample);
			current_record++;
			LData.setCurrentRecord(current_record);
		}
		return samples;
	}
	
	public void writeToDb(int numCases, String dbFileName){
		ArrayList<ArrayList<Integer>> samples = this.sample(numCases);
		int num_vars = samples.get(0).size();
		PrintWriter writer = null;
		try {
			writer = new PrintWriter(new BufferedWriter(new FileWriter(dbFileName,true)));
			for (int caseCounter = 0; caseCounter < numCases; caseCounter++) {
				ArrayList<Integer> current_vals = samples.get(caseCounter);
				for (int var_counter = 0; var_counter < num_vars; var_counter++)
					if (current_vals.get(var_counter).intValue() == 1)
						writer.print(var_counter + " ");
				writer.println();
			}
			writer.close();
		} catch (IOException e) {
			System.out.println("error writing to file " + dbFileName);
			e.printStackTrace();
			System.exit(1);
		} 
	}

	@SuppressWarnings("unused")
	static private void writeCaseFile(String outFileName, LearningData LData) {
		File outFile = new File(outFileName);
		try {
			LData.writeData(outFile);
		} catch (IOException e) {
			System.out.println("error writing case file");
			e.printStackTrace();
		}
	}

}
