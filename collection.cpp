#include <iostream>
using namespace std;

#include "collection.h"
#include <boost/lexical_cast.hpp>

#include "boost/filesystem.hpp"  
using namespace boost::filesystem;
using namespace boost;

DBCollection::DBCollection(const string& name) : depot(name.c_str()) {
}

DBCollection::Document_type DBCollection::getDocument(int id) {
  return Document(depot, id);
}

list<DBCollection::Document_type> DBCollection::getDocuments(int from, int to) {
  bool unlimited = (from == 0 && to == 0);

  list<DBCollection::Document_type> l;

  Datum dat = depot.firstkey();
  int ff = 0;
  int ll = to;
  try {
    do {
      if(ff++ >= from)
	l.push_back(Document(depot, lexical_cast<int>(dat.ptr())));
    } while((dat = depot.nextkey()) && (unlimited || --ll >= 0));
  } catch(Depot_error& e) {
  }

  return l;
}


///

TabCollection::TabCollection(const string& tabName, const string& dataName) : posTable(tabName, dataName) {
}

TabCollection::Document_type TabCollection::getDocument(int id) {
  return Document_type(posTable, id);
}

list<TabCollection::Document_type> TabCollection::getDocuments(int from, int to) {
  bool unlimited = (from == 0 && to == 0);

  list<TabCollection::Document_type> l;

  to = posTable.size();

  int ff = 0;
  int ll = to;
 
  do {
    if(ff++ >= from)      
      l.push_back(TabDocument(posTable, ff));
  } while((ff < to) && (unlimited || --ll >= 0));

  return l;
}

