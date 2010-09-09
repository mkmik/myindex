#include <iostream>
#include <locale>
#include <string>
#include <fstream>
#include <vector>

#include <boost/foreach.hpp>

#include <cstdlib>
#include "stemming/unicode.h"

#include "uncompress.h"
#include "byte_compress.h"
#include "bitreader.h"
#include "bitwriter.h"

#include "gamma_coder.h"
#include "gamma_decoder.h"

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/function_output_iterator.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/typeof/typeof.hpp>
#include <functional>
#include <ext/functional>

#include "searcher.h"
#include "node.h"

#include <bitset>

using namespace boost;
using namespace std;
using namespace stemming;


struct pack_node : public unary_function<Node, int> {
  int operator()(const Node& node) const {
    unsigned int freq = (unsigned int)node.freq - 1;
    int bits = 0;
    while(freq) {
      freq >>= 1;
      bits++;
    }
    //    cout << "bits: " << bits << endl;
    return 123 << (bits+3) | (node.freq - 1) << 3 | (0xff >> (8-3));
  }
};


int main() {
  cout.setf ( ios::showbase ); 
  //locale::global(locale("en_US.UTF-8"));

  Searcher searcher("index-big.db");
  Hits hits = searcher.getHits("a");
  
  cout << "hits: " << hits.size() << endl;
  /*
  BOOST_FOREACH(Node &n, hits) {
    cout << n << endl;
  }
  */

  pack_node func;
  list<int> u;
  copy(make_transform_iterator(hits.begin(), func),
       make_transform_iterator(hits.end(), func),
       back_insert_iterator<list<int> >(u));


  typedef vector<char> bytes_t;

  bytes_t bytes;

  back_insert_iterator<typeof(bytes)> ins(bytes);
  gamma_coder<back_insert_iterator<typeof(bytes)> > gammac(ins);
  
  cout << "test gamma code" << endl;
  #define INPUT_SIZE 10

  int last = 0;
  BOOST_FOREACH(Node &n, hits) {
    int delta = n.doc - last;
    last = n.doc;
    //cout << "compressing: " << dec << delta << endl;
    gammac.push_back(delta);
    gammac.push_back(n.freq);
  }

  gammac.close();
  
  /*
  BOOST_FOREACH(unsigned char n, bytes) {
    cout << "compressed: " << hex << (unsigned int)n << endl;
  }
  */

  cout << dec << "gamma uncompressed size: " << hits.size()*sizeof(Node) << endl;
  cout << dec << "gamma compressed size: " << bytes.size() << endl;
  cout << dec << "gamma compression factor: " << (hits.size()*sizeof(Node))/(double)bytes.size()  << endl;
  

  list<bool> ugo;

  /*
  bitreader<list<char>::iterator> dumper(bytes.begin(), bytes.end());
  
  cout << "DUMPING BITSTREAM" << endl;
  BOOST_FOREACH(bool n, dumper) {
    cout << dec << n;
  }
  cout << "--" << endl;
  */

  bitreader<vector<char>::iterator> br(bytes.begin(), bytes.end());

  //BOOST_FOREACH(bool n, br) {
  //cout << hex << n << endl;
  //}
  
  for(int i=0; i<1; i++) {
    cout << "decoding" << endl;
    vector<int> decoded;
    gamma_decode(br.begin(), br.end(),
		 back_insert_iterator<typeof(decoded)>(decoded)
		 , 1327941
		 );

    BOOST_FOREACH(int n, decoded) {
      cout << "" << dec << n << endl;
    }
    //cout << "decoded " << decoded.size() << " integers" << endl;
  }


  return 0;

  /*
  u.push_back(0x2);
  u.push_back(0x104);
  u.push_back(0x12);
  u.push_back(0x689a06);
  u.push_back(0xffffff);
  u.push_back(0x4);
  u.push_back(0x1);
  u.push_back(0x1);
  u.push_back(0x3);
  u.push_back(0x0);
  u.push_back(0xffffffff);
  u.push_back(0x10);
  */

  list<char> comp;
  copy(byte_compress_iterator<char, list<int>::iterator>(u.begin(), true),
       byte_compress_iterator<char, list<int>::iterator>(u.end()),
       back_insert_iterator<typeof(comp)>(comp)
       );

  //  BOOST_FOREACH(unsigned int ch, u) 
  //cout << "original    : 0x" << hex << ch << endl;
  cout << "len: " << dec << u.size() * sizeof(int) << " bytes" << endl;

  //  BOOST_FOREACH(unsigned char ch, comp) 
  //    cout << "compressed  : 0x" << hex << (unsigned int)ch << endl;
  cout << "len: " << dec << comp.size() * sizeof(char) << " bytes" << endl;

  list<int> uncomp;
  copy(uncompress_iterator<int, list<char>::iterator>(comp.begin(), true),
       uncompress_iterator<int, list<char>::iterator>(comp.end()),
       back_insert_iterator<typeof(uncomp)>(uncomp)
       );
  
  //BOOST_FOREACH(unsigned int num, uncomp) 
  //cout << "uncompressed  : 0x" << hex << num << endl;

  cout << "compression ratio " << dec << (1.0 * u.size() * sizeof(int)) / (comp.size() * sizeof(char)) << " times" << endl;

  cout << "----------------------------" << endl;
  cout << equal(u.begin(), u.end(),
		uncomp.begin()) << endl;

  hits.free();
  return 0;
}


