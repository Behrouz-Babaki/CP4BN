#include "textWriter.h"
#include <vector>
#include <fstream>

using std::vector;
using std::ofstream;
using std::endl;

TextWriter::TextWriter () {}

TextWriter::TextWriter (const vector<size_t>& number_of_values, double threshold, char* file_name) : _number_of_values (number_of_values), _threshold (threshold) {
  this->_number_of_variables = _number_of_values.size();
  _output.open (file_name, ofstream::out);
  this->writeHeader();
}

TextWriter::~TextWriter() {
  _output.close();
}

void TextWriter::getNumVals (vector<size_t>& numVals) {
  numVals = this->_number_of_values;
}

size_t TextWriter::getNumVars (void) {
  return this->_number_of_variables;
}

void TextWriter::writeHeader (void) {
  _output << _threshold << endl;
  _output << _number_of_variables << endl;

  for (auto v: _number_of_values)
    _output << v << endl;
}

void TextWriter::write (const vector<size_t>& attrValues, double min, double max){
  for (auto av: attrValues)
    _output << av;

  _output << endl;
  _output << min << endl;
  _output << max << endl;
}

void TextWriter::writeSparse (const vector<size_t>& attrValues, double min, double max) {
  for (size_t attr_counter = 0, size = attrValues.size(); attr_counter < size; attr_counter++)
    if (attrValues[attr_counter] == 1)
      _output << attr_counter << " ";
  _output << endl;
  _output << min << endl;
  _output << max << endl;
}
