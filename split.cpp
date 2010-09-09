#include <iostream>
#include <locale>
#include <boost/lexical_cast.hpp>
#include <string>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>

using namespace boost;
using namespace std;
namespace po = boost::program_options;

const int LINES = 20;
const string OUTPUT = "docs.tab";


int main(int argc, char **argv) {
  string input;
  string output;
  int size;

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "produce help message")
    ("verbose,v", "be verbose")
    ("input,i", po::value<string>(&input), "source text")
    ("output,o", po::value<string>(&output)->default_value(OUTPUT), "binary document position database")
    ("documents,n", po::value<int>(&size)->default_value(1000), "number of documents");

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
  po::notify(vm);    

  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }
    
  ifstream fin(input.c_str(), ios_base::in);
  ofstream fout(output.c_str(), ios_base::out | ios_base::binary | ios::trunc);

  if(!fin.is_open()) {
    if(input.size() == 0)
      cerr << "please specify the input file, see --help" << endl;
    else
      cerr << "error opening input file" << endl;
    return 1;
  }

  if(!fout.is_open()) {
    cerr << "error opening output file" << endl;
    return 1;
  }

  fin.seekg(0, ios_base::end);
  int initialSkip = fin.tellg() / size;
  fin.seekg(0);

  if(initialSkip < 2) {
    cerr << "document size would be too small: " << initialSkip << endl;
    return 1;
  }

  int pos = 0;
  string dummy;
  while(!fin.eof()) {
    //cout << pos << endl;
    fout.write((char*)&pos, sizeof(pos));
    fin.seekg(pos + initialSkip);
    fin >> dummy;
    pos = fin.tellg();
  }
  
  
}
