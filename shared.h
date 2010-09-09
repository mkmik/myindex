#ifndef _SHARED_H_
#define _SHARED_H_

#include "analyzer.h"

#include <map>
#include <tr1/unordered_map>

// constants

static const char DBNAME[] = "index.db";


// analyzer

#define ANALYZER_CHAIN compose1(stem_f<String>(), to_lower_f<String>())

template<typename String>
struct basic_DefaultAnalyzer : public analyzer<String, typeof(ANALYZER_CHAIN)> {
  basic_DefaultAnalyzer(const String &s) : analyzer<String, typeof(ANALYZER_CHAIN)> (s, ANALYZER_CHAIN) {}
  
};

typedef basic_DefaultAnalyzer<string> ADefaultAnalyzer;
typedef basic_DefaultAnalyzer<wstring> DefaultAnalyzer;


// maps

//typedef std::tr1::unordered_map default_map;
#define default_map std::tr1::unordered_map
//#define default_map std::map

#endif
