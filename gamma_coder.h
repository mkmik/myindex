#ifndef _GAMMA_CODER_H_
#define _GAMMA_CODER_H_

#include <math.h>

#include "bitwriter.h"

template<typename Output>
struct gamma_coder {
  Output output;

  bitwriter<Output> bw;
  
  gamma_coder(const Output &o) : output(o), bw(o) {  
  }

  void push_back(unsigned int num) {
    int l = log2(num);
    for (int a=0; a < l; a++) {       
	bw.putBit(false); //put 0s to indicate how much bits that will follow
    }
    bw.putBit(true);//mark the end of the 0s
    for (int a=0; a < l; a++) { //Write the bits as plain binary
      if (num & (1 << (l-a-1)))
	bw.putBit(true);
      else
	bw.putBit(false);
    }      
  }
  
  void close() {
    bw.close();
  }
};

#endif
