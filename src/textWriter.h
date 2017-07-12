#ifndef TEXTWRITER_H
#define TEXTWRITER_H

#include <vector>
#include <fstream>

using std::vector;
using std::ofstream;

class TextWriter {

 public:
  TextWriter ();
  TextWriter (const vector<size_t>&, double, char*);
  void getNumVals (vector<size_t>&);
  size_t getNumVars ();
  void write (const vector<size_t>&, double, double);
  void writeSparse (const vector<size_t>&, double, double);
  ~TextWriter();

 private:
  void writeHeader ();
  vector<size_t> _number_of_values;
  double _threshold;
  size_t _number_of_variables;
  ofstream _output;

};

#endif // TEXTWRITER_H
