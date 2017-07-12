package limutils.sample;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Scanner;

public class LimNamesReader {

	private  final ArrayList<String> var_names = new ArrayList<String>();
	private  final ArrayList<HashMap<String,Integer>> val_maps = new ArrayList<HashMap<String,Integer>>();

	public LimNamesReader(String fileName) {
		Scanner sc = null;
		try {
			sc = new Scanner (new FileReader(fileName));
			int num_vars = sc.nextInt();
			for (int var_counter = 0; var_counter < num_vars; var_counter++) {
				String var_name = sc.next();
				var_names.add(var_counter, var_name);
				int num_vals = sc.nextInt();
				HashMap<String, Integer> val_map = new HashMap<String, Integer>();
				for (int val_counter = 0; val_counter < num_vals; val_counter++){
					String val_name = sc.next();
					val_map.put(val_name, val_counter);
				}
				val_maps.add(val_map);
			}
				
		} catch (FileNotFoundException e) {
			System.out.println("error reading " + fileName);
			e.printStackTrace();
			System.exit(1);
		}

	}
	
	public  ArrayList<String> get_var_names(){
		return var_names;
	}
	
	public  ArrayList<HashMap<String,Integer>> get_val_maps() {
		return val_maps;
	}
}
