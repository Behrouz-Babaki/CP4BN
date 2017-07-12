#include "cm_options.h"
#include <stdlib.h>
#include <argp.h>

#include <string>
#include <sstream>
#include <iostream>
using std::string;
using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;

struct Cm_opt PROG_OPT;
const char *argp_program_version =
  "likely_finder 0.3";
const char *argp_program_bug_address =
  "<Behrouz.Babaki@cs.kuleuven.be>";
     
/* Program documentation. */
static char doc[] =
  "likely_finder -- a program for finding partial/complete likely states";
     
/* A description of the arguments we accept. */
static char args_doc[] = "";
     
/* The options we understand. */
static struct argp_option options[] = {
  {"verbose",  'v', 0,     0,  "Produce verbose output" },
  {"quiet",    'q', 0,     0,  "Don't produce any output" },
  {"silent",   's', 0,      OPTION_ALIAS },
  {"acfile",   'a', "FILE", 0,
   "Read the arithmetic circuit from FILE"},
  {"lmfile",   'l', "FILE", 0,
   "Read the literal map from FILE"},
  {"output",   'o', "FILE", 0,
   "Output to FILE instead of standard output" },
  {"threshold", 't', "THETA", 0,
   "Use THETA as probability threshold"},
  {"binary", 'b', "BINARY", 0,
   "Write threshold and likely sets to BINARY"},
  {"text", 'x', "TEXT", 0,
   "Write threshold and sparse likely sets to TEXT"},
  {"names", 'n', "NAMES", 0,
   "Write number, names, and values of network variables to NAMES"},
  {"maximal", 'm', 0,   0, "Extract maximal itemsets"},
  {"closed", 'c', 0,   0, "Extract closed itemsets"},
  {"average-size", 'z', 0, 0, "Dispalys the average size of itemsets"},
  {"discriminative" , 'd', 0, 0, "Extract discriminative itemsets"},
  {"acfile2", 'f', "FILE", 0, "Read the second arithmetic cricuit from FILE"},
  {"lmfile2", 'g' , "FILE", 0, "Read the second literal map from FILE"},
  {"pairfile", 'p', "FILE", 0, "Read the variable pairings from FILE"},
  { 0 }
};
     
/* Used by main to communicate with parse_opt. */
struct arguments
{
  int silent, verbose;
  char *output_file;
  char *ac_file;
  char *ac_file2;
  char *lm_file;
  char *lm_file2;
  char* threshold;
  char* binary_file;
  char* text_output_file;
  char* names_file;
  char* var_pair_file;
  bool threshold_set;
  int condensed, discriminative;
  int average_size;
};


/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
   *          know is a pointer to our arguments structure. */
  struct arguments *arguments = static_cast<struct arguments*> (state->input);
  switch (key)
    {
    case 'q': case 's':
      arguments->silent = 1;
      break;
    case 'v':
      arguments->verbose = 1;
      break;
    case 'o':
      arguments->output_file = arg;
      break;
    case 'a':
      arguments->ac_file = arg;
      break;
    case 'f':
      arguments->ac_file2 = arg;
      break;
    case 'l':
      arguments->lm_file = arg;
      break;
    case 'g':
      arguments->lm_file2 = arg;
      break;
    case 'b':
      arguments->binary_file = arg;
      break;
    case 'x':
      arguments->text_output_file = arg;
      break;
    case 'n':
      arguments->names_file = arg;
      break;
    case 'p':
      arguments->var_pair_file = arg;
      break;
    case 't':
      arguments->threshold = arg;
      arguments->threshold_set = true;
      break;
    case 'm':
      arguments->condensed = 1;
      break;
    case 'c':
      arguments->condensed = 2;
      break;
    case 'd':
      arguments->discriminative = 1;
      break;
    case 'z':
      arguments->average_size = 1;
      break;
     
    // case ARGP_KEY_ARG:
    //   if (state->arg_num >= 3)
    // 	/* Too many arguments. */
    // 	argp_usage (state);
    //   break;
     
    // case ARGP_KEY_END:
    //   if (state->arg_num < 0)
    // 	/* Not enough arguments. */
    // 	argp_usage (state);
    //   break;

    // case  ARGP_KEY_NO_ARGS:
    //   argp_usage (state);
    //   break;
     
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

void getCmOptions(int argc, char** argv) {
  
  /// PROG_OPT default values
  PROG_OPT.threshold = 0;

  struct arguments _arguments;

  /* arguments default values. */
  _arguments.threshold = 0;
  _arguments.silent = 0;
  _arguments.verbose = 0;
  _arguments.threshold_set = false;
  _arguments.ac_file = NULL;
  _arguments.ac_file2 = NULL;
  _arguments.lm_file = NULL;
  _arguments.lm_file2 = NULL;
  _arguments.binary_file = NULL;
  _arguments.text_output_file = NULL;
  _arguments.names_file = NULL;
  _arguments.var_pair_file = NULL;
  _arguments.condensed = 0;
  _arguments.discriminative = 0;
  _arguments.average_size = 0;

  argp_parse (&argp, argc, argv, 0, 0, &_arguments);

  if (_arguments.threshold_set){
    string threshold_str(_arguments.threshold);
    stringstream ss(threshold_str);
    double threshold_double;
    ss >> threshold_double;
    
    if (!ss.fail() && threshold_double >= 0 && threshold_double <=1)
      PROG_OPT.threshold = threshold_double;
    else{
      cerr << "error: invalid threshold value." << endl;
      exit(1);
    }
  }

  if (_arguments.ac_file == NULL || _arguments.lm_file == NULL) {
    cerr << "error: input files for arithmetic circuit and literal map should be specified." << endl;
    exit(1);
  }

  if (_arguments.discriminative) {
    if (_arguments.ac_file2 == NULL || _arguments.lm_file2 == NULL) {
      cerr << "error: In discriminative setting, for each network two input files for arithmetic circuit and literal map should be specified." << endl;
      exit(1);
    }

    if (_arguments.var_pair_file == NULL) {
      cerr << "error: In discriminative setting, you should specify how variables of the two networks are paired." << endl;
      exit(1);
    }
  }
  
  PROG_OPT.ac_file = _arguments.ac_file;
  PROG_OPT.ac_file2 = _arguments.ac_file2;
  PROG_OPT.lm_file = _arguments.lm_file;
  PROG_OPT.lm_file2 = _arguments.lm_file2;
  PROG_OPT.binary_file = _arguments.binary_file;
  PROG_OPT.text_output_file = _arguments.text_output_file;
  PROG_OPT.names_file = _arguments.names_file;
  PROG_OPT.var_pair_file = _arguments.var_pair_file;
  PROG_OPT.silent = _arguments.silent;
  PROG_OPT.verbose = _arguments.verbose;
  PROG_OPT.condensed = _arguments.condensed;
  PROG_OPT.discriminative = _arguments.discriminative;
  PROG_OPT.average_size = _arguments.average_size;
  
}

     
     
     
