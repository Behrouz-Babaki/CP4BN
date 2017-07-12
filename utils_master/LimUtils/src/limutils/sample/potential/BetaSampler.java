package limutils.sample.potential;

import java.io.File;

import org.apache.commons.math3.distribution.BetaDistribution;

import edu.ucla.belief.BeliefNetwork;
import edu.ucla.belief.CPTShell;
import edu.ucla.belief.Table;
import edu.ucla.belief.io.NetworkIO;

public class BetaSampler {

	public static void main(String[] args) {
		if (args.length != 5) {
			System.out.println("Usage: BetaSampler.java [input net] [output net] [alpha] [beta] [limit]");
			System.exit(1);
		}
		
		String input_file_name = args[0];
		String output_file_name = args[1];
		double alpha = Double.parseDouble(args[2]);
		double beta = Double.parseDouble(args[3]);
		double limit = Double.parseDouble(args[4]);
		
		BetaDistribution beta_dist  = new BetaDistribution(alpha, beta);
		BeliefNetwork bn = null;

		try {
			bn = NetworkIO.readHuginNet(new File(input_file_name));
			for (Object tbl : bn.tables()) {
				CPTShell cpt_shell = (CPTShell) tbl;
				Table table = cpt_shell.getCPT();
				int num_vals = table.getCPLength() / 2;
				for (int counter = 0; counter < num_vals; counter++) {
					double prob = beta_dist.sample();
					if (prob < limit)
						prob = 0;
					else if (prob > 1 - limit)
						prob = 1;
					table.setCP(counter * 2, prob);
					table.setCP(counter * 2 + 1, 1 - prob);
				}
			}
			NetworkIO.writeNetwork(bn, new File(output_file_name));
		} catch (Exception e) {
			e.printStackTrace();
			System.exit(1);
		}
	}

}
