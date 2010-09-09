#include "tabdocument.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <math.h>
#include <boost/foreach.hpp>

#include "analyzer.h"

using namespace boost;
using namespace std;

TabDocument::TabDocument(PositionTable &tabfile, int id) : BaseDocument<TabDocument>(id), posTable(tabfile) {
  ostringstream os;
  os.copyfmt(cout);
  os << id;
  name = os.str();
}

void TabDocument::serialize(ostream& o) {
  o << asString() << endl;
}

wstring TabDocument::asWideString() {
  return stemming::convert<string, wstring>(asString());
}

string TabDocument::asString() {
  return posTable.getEntry(id);
}

