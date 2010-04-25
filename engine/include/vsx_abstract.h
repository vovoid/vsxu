#ifndef VSX_ABSTRACT_H
#define VSX_ABSTRACT_H

// this is an abstract class that you can use to send any type of data pointed to with a
// void pointer.
// Note: You must associate an ID with an official datatype.
// 

// Parameters of this type will be handled as a default channel.

class vsx_abstract {
public:
  void* data;
  unsigned long id;
  unsigned long timestamp;
};




#endif
