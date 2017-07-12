package limutils.associationrules;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Scanner;

public class ExtractRules {

	@SuppressWarnings("unused")
	public static void main(String[] args) {
		if (args.length < 3) {
			System.out
					.println("Usage: ExtractRules.java [itemset_text_file] ['frequent'|'closed']"
							+ " [min_conf] [output_file_name]? [names_file]?");
			System.exit(1);
		}

		String itemset_file_name = args[0];
		String input_type_str = args[1];
		double min_conf = Double.parseDouble(args[2]);
		int input_type = -1;

		if (input_type_str.compareTo("frequent") == 0)
			input_type = 0;
		else if (input_type_str.compareTo("closed") == 0)
			input_type = 1;
		else {
			System.out
					.println("The second argument must be either 'frequent' or 'closed'.");
			System.exit(1);
		}

		boolean output_to_file = false;
		String output_file_name = null;
		boolean use_names = false;
		String names_file_name = null;

		if (args.length > 3) {
			output_to_file = true;
			output_file_name = args[3];
		}

		if (args.length > 4) {
			use_names = true;
			names_file_name = args[4];
		}

		Scanner sc = null;
		try {
			sc = new Scanner(new File(itemset_file_name));

			String line;
			line = sc.nextLine();
			double threshold = Double.parseDouble(line);
			line = sc.nextLine();
			int num_vars = Integer.parseInt(line);

			ArrayList<Integer> var_vals = new ArrayList<Integer>();
			for (int var_counter = 0; var_counter < num_vars; var_counter++) {
				int var_val;
				line = sc.nextLine();
				var_val = Integer.parseInt(line);
				var_vals.add(var_val);
			}

			Itemsets iss = new Itemsets("OUTTXT");

			while (sc.hasNextLine()) {
				line = sc.nextLine().trim();
				String[] _items = line.split(" ");
				ArrayList<Integer> items = new ArrayList<Integer>();
				for (String _item : _items)
					if (!_item.isEmpty())
						items.add(Integer.parseInt(_item));

				int num_items = items.size();
				int[] itemset = new int[num_items];
				for (int item_counter = 0; item_counter < num_items; item_counter++)
					itemset[item_counter] = items.get(item_counter).intValue();

				Itemset is = new Itemset(itemset);

				line = sc.nextLine();
				double minProb = Double.parseDouble(line);
				line = sc.nextLine();
				double maxProb = Double.parseDouble(line);

				is.setProbability(maxProb);
				iss.addItemset(is, is.size());
			}
			sc.close();

			AssocRules rules = new AssocRules("rules");
			AlgoAgrawalFaster94 algo = null;

			if (input_type == 0)
				algo = new AlgoAgrawalFaster94();
			else if (input_type == 1)
				algo = new AlgoClosedRules();

			rules = algo.runAlgorithm(iss, null, min_conf);
			System.out.println(rules.getRulesCount());

			if (output_to_file)
				write_rules(rules, output_file_name, names_file_name);
			
		} catch (IOException e) {
			System.out.println("error opening file");
			System.exit(1);
			e.printStackTrace();
		}
	}

	private static void write_rules(AssocRules rules, String output_file_name,
			String names_file_name) {

		PrintWriter writer = null;
		try {
			writer = new PrintWriter(new File(output_file_name));
			ArrayList<String> var_names = new ArrayList<String>();
			ArrayList<String> val_names = new ArrayList<String>();

			boolean use_names = false;
			if (names_file_name != null) {
				use_names = true;
				get_names(names_file_name, var_names, val_names);
			}

			int num_rules = rules.getRulesCount();
			for (int rule_counter = 0; rule_counter < num_rules; rule_counter++) {
				AssocRule rule = rules.getRule(rule_counter);
				int[] itemset1 = rule.getItemset1();
				int[] itemset2 = rule.getItemset2();
				print_itemset(writer, itemset1, use_names, var_names, val_names);
				writer.print(" => ");
				print_itemset(writer, itemset2, use_names, var_names, val_names);
				writer.println(" : " + rule.getConfidence());
			}
			writer.close();
		} catch (FileNotFoundException e) {
			System.out.println("error opening file " + output_file_name);
			e.printStackTrace();
			System.exit(1);
		}

	}

	private static void get_names(String names_file_name,
			ArrayList<String> var_names, ArrayList<String> val_names) {
		var_names.clear();
		val_names.clear();

		try {
			Scanner sc = new Scanner(new File(names_file_name));
			String line = sc.nextLine();
			int num_vars = Integer.parseInt(line);
			for (int var_counter = 0; var_counter < num_vars; var_counter++) {
				line = sc.nextLine().trim();
				var_names.add(line);
				line = sc.nextLine();
				int num_vals = Integer.parseInt(line);
				for (int val_counter = 0; val_counter < num_vals; val_counter++) {
					line = sc.nextLine().trim();
					if (val_counter == 1)
						val_names.add(line);
				}
			}
			sc.close();

		} catch (FileNotFoundException e) {
			System.out.println("error opening file " + names_file_name);
			e.printStackTrace();
		}
	}

	private static void print_itemset(PrintWriter writer, int[] itemset,
			boolean use_names, ArrayList<String> var_names,
			ArrayList<String> val_names) {
		writer.print("{");
		int num_items = itemset.length;
		for (int item_counter = 0; item_counter < num_items; item_counter++) {
			int item = itemset[item_counter];
			if (use_names) {
				writer.print(var_names.get(item));
				writer.print(" = ");
				writer.print(val_names.get(item));
			} else
				writer.print(item);

			if (item_counter < num_items - 1)
				writer.print(", ");
		}
		writer.print("}");
	}
}
