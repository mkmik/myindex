#ifndef _INDEX_H_
#define _INDEX_H_

struct IndexMetadata {
  int docs;
  double averageDocumentLength;
  bool compressed;

  IndexMetadata() : docs(0), averageDocumentLength(0.0), compressed(false) {}
};

#endif
