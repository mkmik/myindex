#ifndef _NODE_H_
#define _NODE_H_

#include <string>
#include <iostream>
using namespace std;

class Node {
public:
  int doc;
  unsigned short freq;

  Node() { Node(0,0); }
  Node(int doc, unsigned short freq) : doc(doc), freq(freq) {}
};

inline bool operator<(Node& a, Node& b) {
  return a.doc < b.doc;
}

inline ostream & operator<<(ostream &s, const Node& val) {
  return s << "(" << val.doc << ";" << val.freq << ")";
} 

inline wostream & operator<<(wostream &s, const Node& val) {
  return s << L"(" << val.doc << L";" << val.freq << L")";
} 


class Hit {
 public:
  int doc;
  double score;

  Hit() { Hit(0,0); }
  Hit(int doc, double score) : doc(doc), score(score) {}
};

inline bool operator<(Hit& a, Hit& b) {
  return a.score < b.score;
}

inline ostream & operator<<(ostream &s, const Hit& val) {
  return s << val.doc;
}

#endif
