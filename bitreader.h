#ifndef _BITREADER_H_
#define _BITREADER_H_

#include <boost/typeof/typeof.hpp>
#include <boost/iterator/iterator_adaptor.hpp>

/*
 * This iterator adapts an iterator
 * which contains variable length byte-encoded integers.
 * It returns a fully decoded integer for each position.
 */

template <class T, class Base>
struct bitreader_iterator;

// helper to avoid duplicating complex definition of bitreader_iterator's base class
template <class T, class Base>
struct bitreader_iterator_base {
  typedef boost::iterator_adaptor<
  bitreader_iterator<T, Base>,
  Base,
  T,
  boost::forward_traversal_tag,
  T
    > type;
};

template <class T, class Base>
struct bitreader_iterator
  : bitreader_iterator_base<T, Base>::type
{
private:
  typedef typename bitreader_iterator_base<T, Base>::type super_t;

  struct enabler {};  // a private type avoids misuse

  //  typedef typename Base::value_type Value;

public:

  bitreader_iterator() { }
  explicit bitreader_iterator(Base d,
			      bool autoIncrement = false) : super_t(d), forward(d), pos(0) { 
    current = 0; 
    if(autoIncrement)
      parseNext(true);
  }

  template <class OtherValue>
  bitreader_iterator(
	       bitreader_iterator<OtherValue, Base> const& other
	       , typename boost::enable_if<
	       boost::is_convertible<OtherValue*,T*>
	       , enabler
	       >::type = enabler()
		 )
  : bitreader_iterator::iterator_adaptor_(other.base()) { }



private:
  T current;
  Base forward;
  int pos;

  friend class boost::iterator_core_access;
  void increment() {
    parseNext();
  }

  void parseNext(bool autoIncremented = false) {    
    current = (*forward >> pos++) & 0x1;
    //current = (acc << pos++ ) & 0x80;

    if(pos >= 8)
      smartIncrement(autoIncremented);
  }

  
  void smartIncrement(bool autoIncremented = false) {
    if(!autoIncremented)
      ++this->base_reference(); 
    ++forward;
    pos = 0;
  }
  

  T dereference() const { 
    //    std::cout << "  reading: " << current << std::endl;
    return current;
  }

};

template<typename Input>
struct bitreader {
  Input beg;
  Input en;

  typedef bitreader_iterator<bool, Input> iterator;
  typedef bitreader_iterator<bool, Input> const_iterator;

  bitreader(const Input &beg, const Input &en) : beg(beg), en(en) {
  }

  iterator begin() {
    return bitreader_iterator<bool, Input>(beg, true);
  }

  iterator end() {
    return bitreader_iterator<bool, Input>(en);
  }
  
};

#endif
