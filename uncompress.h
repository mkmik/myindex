#ifndef _UNCOMPRESS_H_
#define _UNCOMPRESS_H_

#include <boost/typeof/typeof.hpp>
#include <boost/iterator/iterator_adaptor.hpp>

/*
 * This iterator adapts an iterator
 * which contains variable length byte-encoded integers.
 * It returns a fully decoded integer for each position.
 */

template <class T, class Base>
struct uncompress_iterator;

// helper to avoid duplicating complex definition of uncompress_iterator's base class
template <class T, class Base>
struct uncompress_iterator_base {
  typedef boost::iterator_adaptor<
  uncompress_iterator<T, Base>,
  Base,
  T,
  boost::forward_traversal_tag,
  T
    > type;
};

template <class T, class Base>
struct uncompress_iterator
  : uncompress_iterator_base<T, Base>::type
{
private:
  typedef typename uncompress_iterator_base<T, Base>::type super_t;

  struct enabler {};  // a private type avoids misuse


public:

  uncompress_iterator() { }
  explicit uncompress_iterator(Base d,
			       bool autoIncrement = false) : super_t(d), forward(d) { 
    current = 0; 
    if(autoIncrement)
      parseNext(true);
  }

  template <class OtherValue>
  uncompress_iterator(
	       uncompress_iterator<OtherValue, Base> const& other
	       , typename boost::enable_if<
	       boost::is_convertible<OtherValue*,T*>
	       , enabler
	       >::type = enabler()
		 )
  : uncompress_iterator::iterator_adaptor_(other.base()) { }



private:
  int current;
  Base forward;

  friend class boost::iterator_core_access;
  void increment() {
    parseNext();
  }

  void parseNext(bool autoIncremented = false) {
    unsigned char ch = (unsigned char)*forward;
    int acc = 0;
    while(ch & 0x80) { // escaped
      acc += ch & ~(0x80);
      acc <<= 7;

      smartIncrement(!autoIncremented);

      ch = (unsigned char)*forward;
    }
    acc += ch;
    smartIncrement(autoIncremented);
    current = acc;
  }

  
  void smartIncrement(bool autoIncremented = false) {
    if(!autoIncremented)
      ++this->base_reference(); 
    ++forward;

  }
  

  int dereference() const { 
    int x = current;
    return x;
  }

};

#endif
