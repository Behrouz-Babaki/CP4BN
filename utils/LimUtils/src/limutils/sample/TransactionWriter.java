package limutils.sample;


public class TransactionWriter {

	public static void main(String[] args) {
		if (args.length != 4){
			System.out.println("Usage: TransactionWriter [network file] [names file] [db file] [#cases]");
			System.exit(1);
		}
		
		SampleGenerator sGen = new SampleGenerator(args[0], args[1]);
		sGen.writeToDb(Integer.parseInt(args[3]), args[2]);
	}
}
