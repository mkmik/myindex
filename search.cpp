#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>

#include <xdepot.h>
#include <cstdlib>

#include "collection.h"
#include "searcher.h"

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include "shared.h"
#include "analyzer.h"

using namespace std;
using namespace qdbm;
namespace po = boost::program_options;

const char* NAME = "textual";

int main(int argc, char** argv) {
  // Declare the supported options.
  int maxHits;
  vector<string > query;
  bool defaultQuery = false; // will become true there is no query from the command line
  int times;
  string dbName;
  int fetchDoc = -1;

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "produce help message")
    ("verbose,v", "verbose")
    ("show,s", "show document content")
    ("document,d", po::value<int>(&fetchDoc), "show one single document by id")
    ("info,i", "show index informations")
    ("file,f", po::value<string>(&dbName)->default_value(DBNAME), "input index filename")
    ("times,t", po::value<int>(&times)->default_value(1), "stress test query n times")
    ("max,m", po::value<int>(&maxHits)->default_value(10), "set max hits")
    ("query,q", po::value<vector<string> >(&query), "query")
    ("dump", "dump inverted lists. If no query terms are specified it dumps all terms");
    ;

  po::positional_options_description pos;
  pos.add("query", -1);

  
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).positional(pos).run(), vm);
  po::notify(vm);    
  
  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }

  if(query.size() == 0) {
    defaultQuery = true;
    query.push_back("textual");
  }

  // prepare list of query terms
  list<string> queryTerms;
  BOOST_FOREACH(string queryWord, query) {
    ADefaultAnalyzer anal(queryWord);
    BOOST_FOREACH(string it, anal) {
      cout << "term: " << it << endl;
      queryTerms.push_back(it);
    }  
  }

  // prepare searcher
  try {
    DBCollection documents("docs.db");
    typedef typeof documents CurrentCollection;
    if(fetchDoc >= 0) {
      CurrentCollection::Document_type doc = documents.getDocument(fetchDoc);
      cout << doc;
      return 0;
    }

    Searcher searcher(dbName);

    if (vm.count("info")) {
      cout << "number of documents: " << searcher.getDocs() << endl;
      cout << "number of terms: " << searcher.getTerms() << endl;
      cout << "compressed: " << searcher.isCompressed() << endl;
      return 1;
    }

    if (vm.count("dump")) {
      cout << "dumping index terms" << endl;
      list<string> dumpedTerms = queryTerms;
      if(!defaultQuery) {
	BOOST_FOREACH(string term, dumpedTerms) {
	  Hits hits = searcher.getHits(term);
	  cout << term << ":" << endl;
	  cout << hits << endl;
	}
      } else {
	cout << "not yet implemented" << endl;
      }
      return 1;
    }

    for(int i=0; i < times; i++) {
     

      if(vm.count("verbose"))
	searcher.setVerbose(true);

      // results will be here

      list<int> res;
      // run query
      searcher.search(queryTerms, res);

      // show results
      if(times == 1) {
	int numHits = maxHits;
	BOOST_FOREACH(int doc, res) {
	  cout << "doc: " << doc << endl;
	  if(vm.count("show")) {
	    CurrentCollection::Document_type sdoc = documents.getDocument(doc);
	    cout << sdoc;
	  }
	    
	  if(--numHits <= 0) {
	    cout << (res.size() - maxHits)  << " more results hidden...." << endl;
	    break;
	  }
	}
      }
      
    }
  } catch(Depot_error& e){
    cerr << e << endl;
    return 1;
  }
  cout << "exiting" << endl;
}
