#ifndef CM_OPTIONS_H
#define CM_OPTIONS_H

#pragma GCC diagnostic ignored "-Wunused-function"
#include <string>
using std::string;

extern struct Cm_opt PROG_OPT;

struct Cm_opt{
  int silent;
  int verbose;
  int condensed;
  int discriminative;
  int average_size;
  double threshold;
  char* ac_file;
  char* ac_file2;
  char* lm_file;
  char* lm_file2;
  char* binary_file;
  char* text_output_file;
  char* names_file;
  char* var_pair_file;
};

void getCmOptions(int, char**);

#endif //CM_OPTIONS_H
