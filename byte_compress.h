#ifndef _BYTE_COMPRESS_H_
#define _BYTE_COMPRESS_H_

#include <boost/typeof/typeof.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <list>
#include <iostream>

/**
 * Implement variable byte encoding.
 * Short integers will occupy less bytes than long integers
 * Integers are encoded in big endian, i.e most significant byte first,
 * this allows simpler decoding
 */

template <class T, class Base>
struct byte_compress_iterator;

// helper to avoid duplicating complex definition of byte_compress_iterator's base class
template <class T, class Base>
struct byte_compress_iterator_base {
  typedef boost::iterator_adaptor<
    byte_compress_iterator<T, Base>,
    Base,
    T,
    boost::forward_traversal_tag,
    T
    > type;
};

template <class T, class Base>
struct byte_compress_iterator
  : byte_compress_iterator_base<T, Base>::type
{
private:
  typedef typename byte_compress_iterator_base<T, Base>::type super_t;

  struct enabler {};  // a private type avoids misuse


public:

  byte_compress_iterator() { }
  explicit byte_compress_iterator(Base d,
			     bool autoIncrement = false) : super_t(d), holder(0), forward(d) { 
    
    if(autoIncrement)
      move(true);

  }

  template <class OtherValue>
  byte_compress_iterator(
	       byte_compress_iterator<OtherValue, Base> const& other
	       , typename boost::enable_if<
	       boost::is_convertible<OtherValue*,T*>
	       , enabler
	       >::type = enabler()
		 )
  : byte_compress_iterator::iterator_adaptor_(other.base()) { }



private:
  std::vector<T> holder;
  T last;

  Base forward;

  friend class boost::iterator_core_access;


  void increment() {
    move();
  }

  void move(bool autoIncremented = false) {
    using namespace std;

    if(holder.size() == 0) {
      decode();
      ++forward;
      if(!autoIncremented)
	++this->base_reference();
    }

    last = holder.back();
    holder.pop_back();
    
  }

  void decode() {
    unsigned int n = (unsigned int)*forward;
    bool first = true;
    if(n==0)
      holder.push_back(0);
    while(n) {
      T escape = first ? 0 : 0x80;
      holder.push_back(n & 0x7f | escape);

      n >>= 7;
      first = false;
    }
  }
  

  T dereference() const {
    return last;
  }

};

#endif
