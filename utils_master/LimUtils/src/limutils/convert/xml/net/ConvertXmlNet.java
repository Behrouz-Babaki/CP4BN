package limutils.convert.xml.net;

import java.io.File;

import edu.ucla.belief.BeliefNetwork;
import edu.ucla.belief.io.NetworkIO;
import edu.ucla.belief.io.xmlbif.RunReadBIF;

public class ConvertXmlNet {

	public static void main(String[] args) {
		if (args.length != 2){
			System.out.println("Usage: ConvertXmlNet [xmlbif file] [net file]");
			System.exit(1);
		}
			
		String inFileName = args[0];
		String outFileName = args[1];
		File xmlFile = new File(inFileName);
		File netFile = new File(outFileName);

		RunReadBIF BifReader = new RunReadBIF(xmlFile, null);
		BeliefNetwork BN = null;

		try {
			BN = BifReader.beliefNetwork();
			if (BN == null) {
				System.out.println("unable to read network");
				System.exit(1);
			} else
				NetworkIO.saveFileAs(BN, netFile);
		} catch (Exception e) {
			System.out.println("error opening files ...");
			e.printStackTrace();
		}
	}
}
