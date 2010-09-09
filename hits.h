#ifndef _HITS_H_
#define _HITS_H_

#include <string>
#include <list>
#include <cstdlib>
#include <iterator>

using namespace std;

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))


template <typename T>
class AbstractHits {
public:
  T *nodes;

  AbstractHits() : nodes(NULL) {}
  AbstractHits(T *n) : nodes(n) {}

  bool operator !() {
    return !nodes;
  }
};

/**
 * This template will be explicitly specialized below.
 * 
 *
 */
template<typename T>
class GenericHits : AbstractHits<T> {

  /**
   * Create a Hits instance from an array of 
   * inverted lists's nodes.
   * This array is normally obtained directly from
   * disk, if possible through memory mapped IO.
   * 
   * Some specialization may need to convert this array
   * into some internal structure, while other may preserve the
   * array unchanged.
   */
  GenericHits(Node *nds, int size);
};

/* 
 * Choose one Hits implementation as default
 * "list<Node>" is more stable.
 *
 */

//typedef GenericHits<list<Node> > Hits;
typedef GenericHits<Node> Hits;

/** 
 * List-based hits result is simpler to write and debug
 * but it's slighlty slower
 */
template<>
class GenericHits<list<Node> > : public AbstractHits<list<Node> > {
public:
  typedef list<Node>::iterator iterator;
  typedef list<Node>::iterator const_iterator;

  GenericHits() : AbstractHits<list<Node> >() {}

  GenericHits(list<Node> *nds) : AbstractHits<list<Node> >(nds) {}

  GenericHits(Node *nds, int size) {
    nodes = new list<Node>(nds, nds + size);
  }

  int size() {
    return nodes->size();
  }

  iterator begin() const {
    return nodes->begin();
  }

  iterator end() const {
    return nodes->end();
  }

  static GenericHits<list<Node> > intersect(GenericHits &hits, 
					    GenericHits &acc) {
    list<Node> *inter = new list<Node>;
    back_insert_iterator<list<Node> > ii(*inter);

    set_intersection (hits.begin(), hits.end(), 
		      acc.begin(), acc.end(), ii);

    return GenericHits(inter);
  }

  void free() {
    delete nodes;
    nodes = NULL;
  }
};

////////////


/**
 * Array based hits are faster because the inverted list doesn't have to be
 * transformed after being fetched from disk. It also permits memory mapped access
 * if available from the underlying storage.
 *
 * However correct memory management is more problematic with this implementation
 */
template<>
class GenericHits<Node> : public AbstractHits<Node> {
public:
  typedef Node* iterator;
  typedef const Node* const_iterator;

  //  Node* nodes;
  int _size;

  GenericHits(Node* nodes = NULL, int size = 0) : AbstractHits<Node>(nodes), _size(size) {}
  GenericHits(const GenericHits& c) : AbstractHits<Node>(nodes), _size(c._size) {}

  int size() {
    return _size;
  }

  iterator begin() const {
    return nodes;
  }

  iterator end() const {
    return nodes + _size;
  }

  inline void free() {
    ::free(nodes);
    nodes = NULL;
    _size = 0;
  }

  static GenericHits intersect(GenericHits &hits, GenericHits &acc) {
      GenericHits inter;
      int min = MIN(hits.size(), acc.size());

      inter.nodes = (Node*)malloc(min*sizeof(Node));

      GenericHits::iterator ii = inter.begin();
      GenericHits::iterator oo;
      oo = set_intersection (hits.begin(), hits.end(), 
			     acc.begin(), acc.end(), ii);
      inter._size = oo - ii;
      return inter;
  }

};

////////////////////

template<typename T>
inline ostream & operator<<(ostream &s, const GenericHits<T>& val) {
  std::copy(val.begin(), val.end(),
	    ostream_iterator<T>(s, "\t"));
  return s;
}

#endif
