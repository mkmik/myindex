#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>


#include <xdepot.h>
#include <cstdlib>

#include "collection.h"
#include "index.h"
#include "shared.h"
#include "stemming/unicode.h"

#include <locale>

using namespace std;
using namespace qdbm;
using namespace boost;
using namespace boost::lambda;
using namespace boost::detail;
using namespace stemming;
namespace po = boost::program_options;

const char* DOCSNAME = "docs.db";

int main(int argc, char **argv) {
  //std::ios::sync_with_stdio(false);
  locale::global(locale("en_US.UTF-8"));

  int from = 0;
  int to = 0;
  int limit = 0;
  string docsName;
  string finalName;
  vector<string> inspectTerms;
  bool verbose = false;

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "produce help message")
    ("verbose,v", "be verbose")
    ("input,i", po::value<string>(&docsName)->default_value(DOCSNAME), "documents db")
    ("output,o", po::value<string>(&finalName)->default_value(DBNAME), "index db")
    ("from,f", po::value<int>(&from), "index from document n")
    ("to,t", po::value<int>(&to), "index up to document n")
    ("limit,l", po::value<int>(&limit), "to = from + limit")
    ("term,t", po::value<vector<string> >(&inspectTerms), "inspect term");

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
  po::notify(vm);    

  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }

  if (vm.count("verbose"))
    verbose = true;

  if (vm.count("limit"))
    to = from + limit;

  if (inspectTerms.size() > 0) {
    Depot depot(finalName.c_str());
    
    BOOST_FOREACH(string term, inspectTerms) {
      cout << "inspecting term " << term << endl;
      int size;
      try {
	char *cinv = depot.get(term.c_str(), -1, 0, -1, &size);
	int ft = size / sizeof(Node) ;
	Node *inv = (Node*)cinv;
      
	wcout << L"ft: " << ft << endl;
	copy(inv, inv+ft, ostream_iterator<Node, wchar_t>(wcout, L", "));
	wcout << endl;
	delete(inv);
      } catch(Depot_error &e) {
	cerr << e << endl;
      }
    }
    depot.close();
    return 0;
  }

  string dbName = finalName.substr(0, finalName.size()-3) + ".tmp";


  try {

    //DirectoryCollection dc("data");
    //DBCollection dc(docsName);
    TabCollection dc("docs.tab", "newtext-clean");

    typedef typeof(dc) Collection_type;
    typedef Collection_type::Document_type Document_type;

    list<Document_type> docs = dc.getDocuments(from, to);
    
    wcout << L"loading documents:" << endl;
    default_map<string, list<Node> > terms;
    list<Document_type>::iterator di;
    for (di = docs.begin(); di != docs.end(); ++di) {
      if(verbose || di->getId() % 100 == 0)
	wcout << L"processing doc: " << lexical_cast<int>(di->getName()) << endl;
      di->process(terms);
    }
    
    //  for(si = allTerms.begin(); si != allTerms.end(); ++si)
    //    cout << "     merged set term: " << *si << endl;
    
    
    wcout << L"all terms computed: " << endl;
    
    
    
    // open the database
    Depot depot(dbName.c_str(), Depot::OWRITER | Depot::OCREAT);


    wcout << L"building inverted list: " << endl;
    default_map<string, list<Node> >::iterator si;
    for(si = terms.begin(); si != terms.end(); ++si) {
      string term = si->first;
      list<Node> &nodes = si->second;
      
      list<Node>::iterator ni;
      Node* data = new Node[nodes.size()];
      int i=0;
      for (ni = nodes.begin(); ni != nodes.end(); ++ni)
	data[i++] = *ni;


      if(verbose) {
	wcout << L"storing inverted list:" << endl;
	copy(nodes.begin(), nodes.end(), ostream_iterator<Node, wchar_t>(wcout, L", "));
	wcout << endl;
	copy(data, data+nodes.size(), ostream_iterator<Node, wchar_t>(wcout, L", "));
	wcout << endl;
      }

      depot.put(term.c_str(), -1, (const char*)data, sizeof(*data) * nodes.size());

      delete data;

    }


    // compute Wd
    unsigned short *wd = new unsigned short[docs.size()];

    int i;
    double Wa = 0.0;
    for(i=0, di = docs.begin(); di != docs.end(); ++di, i++) {
      wd[i] = (unsigned short)di->getWd();
      Wa +=  wd[i];
      if(verbose)
	wcout << L"wd " << i << L": " << wd[i] << endl;
    } 

    Wa /= docs.size();
    //cout << "Wa " << Wa << endl;
    
    depot.put("__Wd__", -1, (const char*)wd, sizeof(*wd) * docs.size());

    IndexMetadata metadata;
    metadata.docs = docs.size();
    metadata.averageDocumentLength = Wa;
    depot.put("__metadata__", -1, (const char*)&metadata, sizeof(metadata));    

    depot.close();

    if(verbose)
      wcout << L"renaming " << convert<string, wstring>(dbName) << L" to " << convert<string,wstring>(finalName) << endl;
    
    rename(dbName.c_str(), finalName.c_str());

  } catch(Depot_error& e){
    cerr << e << endl;
    return 1;
  } catch(filesystem_error& e) {
    cerr << e.what() << endl;
    cerr << e.system_error() << endl;
    return 1;
  }

  return 0;
}
