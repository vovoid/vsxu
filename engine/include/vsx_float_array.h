#ifndef VSX_FLOAT_ARRAY_H
#define VSX_FLOAT_ARRAY_H

class vsx_float_array {
public:
  vsx_array<float>* data;
  unsigned long timestamp;
  //vsx_float_array& operator=(vsx_float_array& t) {
    //for (int i = 0; i < t.data.size(); ++i) {
      //data.push_back(t.data[i]);
    //}
  //}
  vsx_float_array() {
    data = 0;
  }  
  //~vsx_float_array() {
    //delete data;
  //}  
};

class vsx_float3_array {
public:
  vsx_array<vsx_vector>* data;
  unsigned long timestamp;
  //vsx_float_array& operator=(vsx_float_array& t) {
    //for (int i = 0; i < t.data.size(); ++i) {
      //data.push_back(t.data[i]);
    //}
  //}
  vsx_float3_array() {
    data = 0;
  }  
  //~vsx_float_array() {
    //delete data;
  //}  
};  

class vsx_quaternion_array {
public:
  vsx_array<vsx_quaternion>* data;
  unsigned long timestamp;
  //vsx_float_array& operator=(vsx_float_array& t) {
    //for (int i = 0; i < t.data.size(); ++i) {
      //data.push_back(t.data[i]);
    //}
  //}
  vsx_quaternion_array() {
    data = 0;
  }
  //~vsx_float_array() {
    //delete data;
  //}
};

#endif
