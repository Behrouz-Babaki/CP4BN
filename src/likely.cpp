#pragma GCC diagnostic ignored "-Wunused-local-typedefs"

#include <vector>
#include <string>
#include <gecode/float.hh>
#include <gecode/int.hh>
#include <gecode/search.hh>
#include "lim.h"
#include "lim_condensed.h"
#include "lim_discriminative.h"
#include "cm_options.h"
#include "lmapReader.h"
#include "acReader.h"
#include "attrSetWriter.h"
#include "textWriter.h"

#ifdef USE_GIST
#include <gecode/gist.hh>
#endif //USE_GIST

using namespace Gecode;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

extern struct Cm_opt PROG_OPT;

void printStats(int, const Gecode::Search::Statistics&);

int main (int argc, char** argv) {

  getCmOptions(argc, argv);

  try {

    double threshold = PROG_OPT.threshold;
    
    LmapReader MapReader (PROG_OPT.lm_file);
    
    if (PROG_OPT.names_file != NULL) {
      string namesFile(PROG_OPT.names_file);
      MapReader.writeVarValFile(namesFile);
    }

    vector<size_t> varValNums;
    MapReader.getNVarVals(varValNums);

    //TODO There should be a better method for copying
    SetWriter* BinaryWriter = NULL;
    bool output_binary = false;
    if (PROG_OPT.binary_file != NULL) {
      output_binary = true;
      BinaryWriter = new SetWriter (varValNums, threshold, PROG_OPT.binary_file);
    }

    //TODO There should be a better method for copying
    TextWriter* textWriter = NULL;
    bool output_text = false;
    if (PROG_OPT.text_output_file != NULL) {
      output_text = true;
      textWriter = new TextWriter (varValNums, threshold, PROG_OPT.text_output_file);
    }


    AcReader NetReader(PROG_OPT.ac_file, MapReader);

    double total_length = 0;
    Lim* s;
    if (PROG_OPT.condensed > 0) 
      s = new Lim_Condensed (NetReader, threshold, PROG_OPT.condensed);
    else if (PROG_OPT.discriminative) {
      LmapReader MapReader2(PROG_OPT.lm_file2);
      AcReader NetReader2(PROG_OPT.ac_file2, MapReader2);
      PairReader PReader(PROG_OPT.var_pair_file, MapReader, MapReader2);
      PReader.test();
      s = new Lim_Discriminative (NetReader, NetReader2, PReader, threshold);
    }
    else
      s = new Lim (NetReader, threshold);
#ifdef USE_GIST
    Gist::Print<Lim_Condensed> p("Print solution");
    Gist::Options o;
    o.inspect.click(&p);
    o.c_d = 1; // can be very memory consumptive
    Gist::dfs(s, o);
#else
      vector<size_t> attrValues;
      double minProb, maxProb;
      int nrSols=0;
      Search::Options o;
      o.c_d = 1; // can be very memory consumptive
      DFS<Lim>* d = new DFS<Lim>(s,o);
      while (Lim* current_solution = d->next()){
	if (PROG_OPT.verbose)
	  current_solution->print(cout);
	current_solution->getSetnProb(attrValues, minProb, maxProb);
	if (output_binary)
	  BinaryWriter->write(attrValues, minProb, maxProb);
	if (output_text)
	  textWriter->writeSparse(attrValues, minProb, maxProb);
	if (PROG_OPT.condensed && PROG_OPT.average_size)
	  total_length += (dynamic_cast<Lim_Condensed*>(current_solution))->getItemsetSize();
	delete current_solution;
	nrSols++;
      }
      delete s;
      delete BinaryWriter;
      if (!PROG_OPT.silent)
	printStats(nrSols, d->statistics());
      if (PROG_OPT.average_size) {
	cout << "AVG_LENGTH " << total_length / double(nrSols) << endl;
      }
#endif
    return 0;

  } catch (Exception e) {
    cerr << "Something went wrong ...\n" << e.what() << endl;
    return 1;
  } // try
}

void printStats(int nrSols, const Gecode::Search::Statistics &stats){
  cout <<"Search statistics:"<<std::endl;
  cout <<"PROPS "<<stats.propagate<<std::endl;
  cout <<"SOLS  "<<nrSols<<std::endl;
  cout <<"FAIL  "<<stats.fail<<std::endl;
  cout <<"NODE  "<<stats.node<<std::endl;
  cout <<"DEPTH "<<stats.depth<<std::endl;
}
