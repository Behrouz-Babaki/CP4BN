#ifndef ATTRSETWRITER_H
#define ATTRSETWRITER_H

#include <vector>
#include <fstream>

using std::vector;
using std::ofstream;

class SetWriter {

 public:
  SetWriter ();
  SetWriter (const vector<size_t>&, double, char*);
  void getNumVals (vector<size_t>&);
  size_t getNumVars ();
  void getByteVec (const vector<size_t>&, vector<unsigned char>&);
  void write (const vector<size_t>&, double, double);
  ~SetWriter();

 private:
  void boolVecRep (const vector<size_t>&, 
	      const vector<size_t>&, 
	      vector<bool>&);
  unsigned char numBits (size_t);
  unsigned char getByte(vector<bool>::const_iterator, 
			vector<bool>::const_iterator);
  void writeHeader ();

  vector<size_t> _number_of_values;
  double _threshold;
  size_t _number_of_variables;
  ofstream _output;

};

#endif // ATTRSETWRITER_H
