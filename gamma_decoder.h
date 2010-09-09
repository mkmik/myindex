#ifndef GAMMA_DECODER_H_
#define GAMMA_DECODER_H_

#include "bitreader.h"

template<typename Input>
struct bitreader_wrapper {
  Input beg;
  Input en;

  bitreader_wrapper(const Input& beg, const Input& en) : beg(beg), en(en) {}

  bool getBit() {
    bool val = *beg;
    ++beg;
    return val;
  }

  bool hasLeft() {
    return beg != en;
  }
};


template<typename Input, typename Output>
void gamma_decode(const Input& beg, const Input& en, Output o) {
  int numberBits = 0;
  bitreader_wrapper<Input> br(beg, en);

  while(br.hasLeft()) {
    numberBits = 0;

    while(!(br.getBit() || !br.hasLeft())) {
      numberBits++; //keep on reading until we fetch a one...
    }
    int current = 0;
    for (int a=0; a < numberBits; a++) {//Read numberBits bits
      current <<= 1;
      current |= br.getBit();
    }
    //write it as a 32 bit number
    
    current = current | ( 1 << numberBits ) ;              //last bit isn't encoded!
    *o = current;
    ++o;
  }
}


template<typename Input, typename Output>
void gamma_decode(const Input& beg, const Input& en, Output o, int n) {
  int numberBits = 0;
  bitreader_wrapper<Input> br(beg, en);

  while(n--) {
    numberBits = 0;

    while(br.getBit()) {
      numberBits++; //keep on reading until we fetch a one...
    }
    int current = 0;
    for (int a=0; a < numberBits; a++) {//Read numberBits bits      
      current = (current << 1 ) | br.getBit();
    }
    //write it as a 32 bit number
    
    current = current | ( 1 << numberBits ) ;              //last bit isn't encoded!
    *o = current;
    ++o;
  }
}


#endif
