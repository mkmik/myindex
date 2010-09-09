#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>

#include <xdepot.h>
#include "node.h"
#include "index.h"

#include "gamma_coder.h"

using namespace qdbm;
using namespace std;
using namespace boost;
namespace po = boost::program_options;

#define DBNAME "index.db"
#define OUTPUT "compressed.db"

struct index_compressor {
  Depot input;
  Depot output;

  int usize;
  int size;
  int count;

  index_compressor(string inputName, string outputName) : input(inputName.c_str()),
							  output(outputName.c_str(), Depot::OWRITER | Depot::OCREAT),
							  usize(0), size(0), count(0) {
  }

  ~index_compressor() {
    cout << "uncompressed size: " << usize << endl;
    cout << "uncompressed average: " << usize/count << endl;
    cout << "compressed size: " << size << endl;
    cout << "compressed average: " << size/count << endl;
    
    input.close();
    output.close();
  }

  void compress() {
    try {
      int from = 0;
      int to = 0;
      bool unlimited = (from == 0 && to == 0);
      int ff = 0;
      int ll = to;
      
      Datum dat = input.firstkey();
      
      do {
	if(ff++ >= from)
	  process(dat.ptr());
      } while((dat = input.nextkey()) && (unlimited || --ll >= 0));
    } catch(Depot_error& e) {
    }
  }
  
  void process(const char *term) {
    int csize;
    Node *cinv = (Node*)input.get(term, -1, 0, -1, &csize);

    if(term[0] == '_') {
      // copy as is

      // set compressed bit in metadata
      if(strcmp("__metadata__", term) == 0) {
	IndexMetadata *md = (IndexMetadata*) cinv;
	md->compressed = true;
      }

      output.put(term, -1, (const char*)cinv, csize);
      return;
    }

    //cout << term << endl;
    vector<char> bytes;
    back_insert_iterator<typeof(bytes)> ins(bytes);
    gamma_coder<back_insert_iterator<typeof(bytes)> > gammac(ins);

    int last = 0;
    BOOST_FOREACH(Node &n, make_pair(cinv, cinv + csize/sizeof(Node))) {
      int delta = n.doc - last;
      last = n.doc;
      //cout << "compressing: " << dec << delta << endl;
      gammac.push_back(delta);
      gammac.push_back(n.freq);
    }

    gammac.close();

    char *data = new char[bytes.size()+1];
    char *d = data;
    BOOST_FOREACH(char x, bytes)
      *d++ = x;
    output.put(term, -1, data, bytes.size());
    //output.put(term, -1, "test", -1);
    delete[] data;
    //cout << csize/sizeof(Node) << endl;

    count++;
    usize += csize;
    size += bytes.size();

    free(cinv);
  }
  
};


int main(int argc, char **argv) {
  string indexName;
  string finalName;

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "produce help message")
    ("verbose,v", "be verbose")
    ("input,i", po::value<string>(&indexName)->default_value(DBNAME), "uncompressed index")
    ("output,o", po::value<string>(&finalName)->default_value(OUTPUT), "compressed index");

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
  po::notify(vm);    

  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }


  try {
    index_compressor comp(indexName, finalName);
    comp.compress();
  } catch(Depot_error& e) {
    cerr << e << endl;
  }


  cout << "compressed" << endl;
}
