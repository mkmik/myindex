#include "document.h"
#include <iostream>
#include <fstream>

using namespace boost;
using namespace std;

Document::Document(Depot &depot, int id) : BaseDocument<Document>(id), depot(depot) {
  ostringstream os;
  os.copyfmt(cout);
  os << id;
  name = os.str();
}

void Document::serialize(ostream& o) {
  o << asString() << endl;
}

wstring Document::asWideString() {
  return stemming::convert<string, wstring>(asString());
}

string Document::asString() {
  char *doc = depot.get(name.c_str(), -1);
  string str = doc;
  free(doc);
  return str;
}
