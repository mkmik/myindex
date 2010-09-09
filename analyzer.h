#ifndef _ANALYZER_H_
#define _ANALYZER_H_

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "stemming/english_stem.h"

#include <iterator>
#include <functional>
#include <ext/functional>

#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

// helpers

static boost::char_separator<wchar_t> wsep(L" \t\n\r |¦~!%^#$+-=*?_&[](){}<>/\\\"`',.;:0123456789@®€£©·±½¼¾°′≤“’‘‛–—”‚…´¨÷×¿«»§←→ª„¥¬¶≠⁺⁻΄√₁₂₃₄⁷₆²³⇒∥″∈¡≥¢₀∞≈↓⊙˜¤↔ﬀ•∀");
static boost::char_separator<wchar_t> wkept(L"");

static boost::char_separator<char> asep(" \t\n\r|~!%^#$+-=*?_&[](){}/\\\"`',.;:");
static boost::char_separator<char> akept("@0123456789");

template<typename Char>
struct separator {
  boost::char_separator<Char> sep();
};


template<>
struct separator<wchar_t> {
  boost::char_separator<wchar_t> sep() { return wsep; }
  boost::char_separator<wchar_t> kept() { return wkept; }
};

template<>
struct separator<char> {
  boost::char_separator<char> sep() { return asep; }
  boost::char_separator<char> kept() { return akept; }
};

// processors

template<typename String>
struct to_lower_f : public unary_function<String, String> {
  String operator()(const String& __x) const {
    return boost::to_lower_copy(__x);
  }
};

template<typename String,
	 typename Char = typename String::value_type>
struct stem_f : public unary_function<String, String> {
  String operator()(const String& __x) const {
    String tmp = __x;
    stemming::english_stem<Char> stemmer;
    stemmer(tmp);
    return tmp;
  }
};


// analyzer

template<typename String,
	 typename Function,
	 typename Char = typename String::value_type>
struct analyzer {

  typedef boost::tokenizer <
    boost::char_separator<Char>,
    typename String::const_iterator,
    String
    > Tokenizer; 

  typedef boost::transform_iterator<Function, typename Tokenizer::const_iterator > iterator;
  typedef iterator const_iterator;

protected:
  String input;
  Function func;
  Tokenizer tok;

public:

  analyzer(const String &input, const Function &func) : input(input), 
							func(func), 
							tok(input, separator<Char>().sep()) { 
  }

  iterator begin() {    
    return make_transform_iterator(tok.begin(), func);
  }

  iterator end() {    
    return make_transform_iterator(tok.end(), func);
  }
};


#endif
