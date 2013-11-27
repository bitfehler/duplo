#include <cstdlib>
#include <iostream>
#include <fstream>
#include <list>
#include <getopt.h>

#include "Duplo.h"

using namespace std;

static void usage(ostream& out, const char* name, int status)
{
	out << "Usage: " << name << " [options] <file>..." << endl;
	out << "  or:  " << name << " [options] -i <file>" << endl;
	out << endl;
	out << "Find duplicate source code in input files." << endl;
	out << endl;
	out << "Options:" << endl;
	out << "  -h, --help               Print this help, then exit" << endl;
	out << "  -v, --version            Print version number, then exit" << endl;
	out << "  -q, --quiet              Do not print progress information, only results" << endl;
	out << "  -x, --xml                Generate XML output" << endl;
	out << "  -l, --min-lines N        Set minimum number of lines that count as block to N (default: " << MIN_BLOCK_SIZE << ")" << endl;
	out << "  -l, --min-chars N        Set minimum number of characters that count as line to N (default: " << MIN_CHARS << ")" << endl;
	out << "  -p, --ignore-preprocess  Ignore preprocessor directives" << endl;
	out << "  -n, --ignore-same-name   Do not compare files with the same name" << endl;
	out << "  -i, --input <file>       Read input files from <file>, one file per line" << endl;
	out << "  -o, --output <file>      Write final results to <file> (default: stdout)" << endl;
	out << endl;
	out << "More information might be available here: https://github.com/bitfehler/duplo" << endl;
	exit(status);
}

static struct option long_options[] = {
	{ "help", no_argument, 0, 'h' },
	{ "version", no_argument, 0, 'v' },
	{ "quiet", no_argument, 0, 'q' },
	{ "min-lines", required_argument, 0, 'l' },
	{ "min-chars", required_argument, 0, 'c' },
	{ "ignore-preprocessor", no_argument, 0, 'p' },
	{ "ignore-same-name", no_argument, 0, 'n' },
	{ "xml", no_argument, 0, 'x' },
	{ "input", required_argument, 0, 'i' },
	{ "ouput", required_argument, 0, 'o' },
	{ 0, 0, 0, 0 }
};

/**
 * Main routine
 *
 * @param argc  number of arguments
 * @param argv  arguments
 */
int main(int argc, char* argv[])
{
	int c = 0;

	bool quiet = false;
	int min_lines = MIN_BLOCK_SIZE;
	int min_chars = MIN_CHARS;
	bool ignore_preprocessor = false;
	bool ignore_same_name = false;
	bool xml_output = false;
	char* input = 0L;
	char* output = 0L;

	while ((c = getopt_long(argc, argv, "hql:c:pnxi:o:", long_options, 0L)) != -1) {
		switch (c) {
		case 'h':
			usage(cout, argv[0], 0);
			break;
		case 'v':
			cout << "duplo v" << VERSION << endl;
			exit(0);
			break;
		case 'q':
			quiet = true;
			break;
		case 'l':
			min_lines = atoi(optarg);
			break;
		case 'c':
			min_chars = atoi(optarg);
			break;
		case 'p':
			ignore_preprocessor = true;
			break;
		case 'n':
			ignore_same_name = true;
			break;
		case 'x':
			xml_output = true;
			break;
		case 'i':
			input = optarg;
			break;
		case 'o':
			output = optarg;
			break;
		case '?':
		default:
			usage(cerr, argv[0], 1);
		}
	}

	vector<string> input_files;

	if (optind == argc) {
		if (!input) {
			cerr << "Error: please either supply input files or use the -i option!" << endl;
			cerr << "Try " << argv[0] << " -h for details." << endl;
			exit(1);
		}
		ifstream ifs(input);
		if (!ifs.good()) {
			cerr << "Error: failed to read input files from " << input << "!" << endl;
			exit(1);
		}
		string line;
		while (getline(ifs, line)) {
			input_files.push_back(line);
		}
	} else {
		cout << optind << " - " << argc << endl;
		if (input) {
			cerr << "Error: can only handle input files or the -i option, but not both!" << endl;
			cerr << "Try " << argv[0] << " -h for details." << endl;
			exit(1);
		}
		for (int i = optind; i < argc; ++i) {
			input_files.push_back(string(argv[i]));
		}
	}

	ostream* out = &cout;
	ofstream outf;
	if (output) {
		outf.open(output);
		if (!outf.good()) {
			cerr << "Error: cannot write to output file " << output << "!" << endl;
			exit(1);
		}
		out = &outf;
	}

	Duplo duplo(input_files, min_lines, min_chars, ignore_preprocessor, ignore_same_name, xml_output, quiet);
	duplo.run(*out);

	return 0;
}
