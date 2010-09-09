#ifndef _SEARCHER_H_
#define _SEARCHER_H_

#include <iostream>
#include <string>
#include <list>
#include <xdepot.h>
#include <math.h>

#include "index.h"

using namespace std;
using namespace qdbm;

#include "node.h"
#include "hits.h"

class Searcher {
  Depot depot;

  IndexMetadata *metadata;
  unsigned short *wd;
  
  bool verbose;
  
protected:

  /** IDT */
  inline double getWqt(int N, int ft) {  
    // Xapian's
    //double wqt = log((N - ft + 0.5) / (ft + 0.5));
    //if(wqt < 0)
    //  return 0.0;
    //return wqt;
    return ft * (log(((double)N) / ((double)(ft+1))) + 1.0); // Lucene's default. 
  }
  
  inline double getWdt(unsigned short fdt, unsigned short Wd) {
    const double k1 = 1.2;
    const double b = 0.75;
    
    double Wa = metadata->averageDocumentLength;
    double L = Wd / Wa;
    if(L < 0.5)
      L = 0.5;
    if(verbose)
      cout << "L: " << L << endl;
    double Kd = k1 * ((1-b) + b * L);
    return ((k1 + 1) * fdt) / (Kd + fdt);
  }

public:
  
  Searcher(string name="index.db");
  ~Searcher();

  Hits getHits(const string &term);
  list<int> *search(list<string> &terms, list<int> &res);
  list<int> *search(list<string> &terms, list<int> *res = NULL);

  unsigned short* getWd();
  int getDocs() { return metadata->docs; }
  int getTerms() { return depot.rnum() - 2; }
  bool isCompressed() { return metadata->compressed; }

  void setVerbose(bool v) { verbose = v; }
};

#endif
