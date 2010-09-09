#ifndef _BITWRITER_H_
#define _BITWRITER_H_

template<typename Output>
struct bitwriter {
  Output output;
  unsigned char byte;
  int pos;

  bitwriter(const Output &o) : output(o), byte(0), pos(0) {
  }

  void putBit(bool bit) {
    byte >>= 1;
    if(bit)
      byte |= 0x80;
    pos++;
    if(pos == 8) {
      *output = byte;
      ++output;
      byte = 0;
      pos = 0;
    }
  }

  void close() {
    byte >>= (8-pos);
    *output = byte;
  };
};

#endif
