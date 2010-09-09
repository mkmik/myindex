#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_

#include <string>
#include <set>
#include <list>
using namespace std;

#include "boost/filesystem.hpp"  
using namespace boost::filesystem;

#include <xdepot.h>
using namespace qdbm;

#include "node.h"
#include "shared.h"

#include <cstdlib>
#include <algorithm>
#include <math.h>
#include <boost/foreach.hpp>

#include "analyzer.h"


template<typename This>
class BaseDocument {
protected:
  string name;
  int id;
  int words;
  double wd;

  BaseDocument(int id) : id(id), words(0) {}

public:
  wstring asWideString();
  string asString();  


  /**
   * get utf-8 string from DB, convert to wstring, 
   * tokenize (unicode aware),
   * convert to lowercase,
   * (TODO: stem),
   * count occurrences of terms (as wstrings), 
   * for each term add a node in the inverted list (as utf-8 string again)
   */
  void process(default_map<string, list<Node> > &terms) {
    try {
      default_map<wstring, unsigned short> count(100);
      //default_map<wstring, int> count;
      
      wstring wsdoc = (static_cast<This*>(this))->asWideString();
      
      DefaultAnalyzer anal(wsdoc);
      
      BOOST_FOREACH(wstring term, anal)
	count[term]++;
      
      double wdt_acc = 0.0;
      default_map<wstring, unsigned short>::iterator it;
      for(it = count.begin(); it != count.end(); ++it) {
	int fdt = it->second;
	
	string uterm = stemming::convert<wstring, string>(it->first);
	if(uterm.size() == 1) continue;
	terms[uterm].push_back(Node(id, fdt));
	
	double wdt = 1.0 + log(fdt);
	wdt_acc += wdt * wdt;
      }
      
      wd = sqrt(wdt_acc);
    
    } catch(boost::archive::iterators::dataflow_exception &e) {
      cerr << "utf8 exception in doc " << id << endl;
    }
  }
};

class Document : public BaseDocument<Document> {

protected:

  Depot &depot;
public:
  Document(Depot &depot, int id);
  Document(int id);

  const string& getName() { return name; }
  const int getId() { return id; }
  const int getWords() { return words; }
  const double getWd() { return wd; }

  wstring asWideString();
  string asString();

  void serialize(ostream& o);
  //  void process(default_map<string, list<Node> > &terms);

};

inline ostream& operator<<(ostream& o, Document& doc) {
  doc.serialize(o);
  return o;
}

#endif
