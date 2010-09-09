#ifndef _COLLECTION_H_
#define _COLLECTION_H_

#include <list>
#include <string>
#include "document.h"
#include "tabdocument.h"

#include <xdepot.h>
using namespace qdbm;

using namespace std;

class Collection {
public:
  list<Document> getDocuments();
  Document getDocument(int id);
};

class DBCollection : public Collection {
  Depot depot;
public:
  typedef Document Document_type;

  DBCollection(const string& name);

  Document_type getDocument(int id);
  list<Document_type> getDocuments(int from = 0, int to = 0);
};

class TabCollection : public Collection {
  PositionTable posTable;
public:

  typedef TabDocument Document_type;
  TabCollection(const string& tabName, const string& dataName);

  Document_type getDocument(int id);
  list<Document_type> getDocuments(int from = 0, int to = 0);

};

#endif
