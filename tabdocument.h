#ifndef _TABDOCUMENT_H_
#define _TABDOCUMENT_H_

#include <string>
#include <set>
#include <list>
#include <iostream>
#include <fstream>
using namespace std;

#include "boost/filesystem.hpp"  
using namespace boost::filesystem;

#include "document.h"
#include "node.h"
#include "shared.h"

struct PositionTable {
  ifstream table;
  ifstream data;

  PositionTable(string tableName, string dataName) : table(tableName.c_str()),
						     data(dataName.c_str()) {}

  int size() {
    table.seekg(0, ios_base::end);
    return table.tellg() / sizeof(int);
  }

  void getExtent(int id, int& start, int &end) {
    int tmp;
    table.seekg(id * sizeof(int));
    table.read((char*)&tmp, sizeof(int));
    start = tmp + 1;
    table.read((char*)&tmp, sizeof(int));
    end = tmp;
  }

  string getEntry(int id) {
    int start;
    int end;

    getExtent(id, start, end);
    int len = end - start;

    data.seekg(start);
    char *buffer = new char[len];
    data.read(buffer, len);
    string res(buffer, buffer + len);
    delete[] buffer;
    return res;
  }
};

class TabDocument : public BaseDocument<TabDocument> {

  PositionTable &posTable;
public:
  TabDocument(PositionTable &posTable, int id);
  TabDocument(int id);

  const string& getName() { return name; }
  const int getId() { return id; }
  const int getWords() { return words; }
  const double getWd() { return wd; }

  wstring asWideString();
  string asString();

  void serialize(ostream& o);
  //  void process(default_map<string, list<Node> > &terms);

};

inline ostream& operator<<(ostream& o, TabDocument& doc) {
  doc.serialize(o);
  return o;
}


#endif
