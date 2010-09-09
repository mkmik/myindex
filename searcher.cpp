#include "searcher.h"
#include <math.h>
#include <map>
#include <vector>
#include <algorithm>

#include <boost/foreach.hpp>

#include "gamma_decoder.h"

Searcher::Searcher(string name) : depot(name.c_str()), verbose(false) {
  metadata = (IndexMetadata*)depot.get("__metadata__", -1);
  wd = NULL;
}

Searcher::~Searcher() {
  if(wd)
    free(wd);
  free(metadata);
  depot.close();
}

unsigned short* Searcher::getWd() {
  if(!wd)
    wd = (unsigned short*)depot.get("__Wd__", -1);
  return wd;
}

Node* gammaHits(char *val, int size, int& num) {
  bitreader<char*> br(val, val + size);
  
  vector<int> decoded;
  gamma_decode(br.begin(), br.end(),
	       back_insert_iterator<typeof(decoded)>(decoded)
	       );

  // hack
  if(decoded.size() & 1)
    decoded.pop_back();
  num = decoded.size() / 2;
  
  Node* nodes = new Node[num];
  Node* n = nodes;
  int last = 0;
  for(vector<int>::iterator it = decoded.begin(); it != decoded.end(); ++it) {
    n->doc = *it + last;
    last = n->doc;
    ++it;
    n->freq = *it;
    n++;
  }
  
  return nodes;
}

Hits Searcher::getHits(const string& term) {
    int size;
    char *val;
    try {
      val = depot.get(term.c_str(), -1, 0, -1, &size);
    } catch(Depot_error& e){
      //cerr << e << endl;
      return Hits(NULL, 0);
    }
    Node* nodes = (Node*) val;
    int num = size / sizeof(Node);

    if(isCompressed())
      nodes = gammaHits(val, size, num);

    return Hits(nodes, num);
}

list<int> *Searcher::search(list<string> &terms, list<int> *hits) {

  if(!hits) {
    hits = new list<int>;
  }

  return search(terms, *hits);
}

list<int> *Searcher::search(list<string> &terms, list<int> &res) {
  int ndocs = getDocs();
  vector<double> A(ndocs + 1);
  
  list<int> numhits;
  BOOST_FOREACH(string &term, terms) {
    
    Hits hits = getHits(term);
    int ft = hits.size();
    
    if(hits.size() == 0)
      continue;

    Hits::iterator ai;
    for(ai = hits.begin(); ai != hits.end(); ++ai) {
      Node &anode = *ai;
      int doc = anode.doc;

      double wdt = getWdt(anode.freq, getWd()[doc]);
      double wqt = getWqt(ndocs, ft);	
      
      A[doc] += wdt * wqt;
    }

    hits.free();
  }

  for(int i=0; i<ndocs; i++) {
    if(verbose)
      cout << "SCORE " << i << ": " << A[i] << " / " << getWd()[i] << " = " << A[i] << " => ";
    A[i] /= getWd()[i];
    if(verbose)
    cout << A[i] << endl;
  }

  /** very stupid rank sort */

  map<double, list<int> > scored;
  
  // stores the position (index, i.e. a document number) of
  // each positive accumulator value inside the scored map
  BOOST_FOREACH(double &acc, A) {
    if(acc > 0.00)
      scored[acc].push_back(&acc - &A.front());
  }

  // this typedef is required because of the comma
  // inside the template parameters is parsed by the preprocessor
  // as if it were the argument separator of the macro.

  typedef pair<const double, list<int> > mypair;
  BOOST_FOREACH(mypair &lit, scored) {
    copy(lit.second.begin(), lit.second.end(),
	 back_insert_iterator<list<int> >(res));
  }

  return &res;
}
