#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "main.h"
#include "math_binary_ops.h"
#include "vsx_math_3d.h"
#include "vsx_quaternion.h"




//TODO: make an enumeration toggler

#ifndef MIN_MAX_STATIC
  #define MIN_MAX_STATIC
float max (float x, float a)
{
   x -= a;
   x += (float)fabs (x);
   x *= 0.5f;
   x += a;
   return (x);
}

float min (float x, float b)
{
   x = b - x;
   x += (float)fabs (x);
   x *= 0.5f;
   x = b - x;
   return (x);
}

#endif

float clip (float x, float a, float b)
{
   float x1 = (float)fabs (x-a);
   float x2 = (float)fabs (x-b);
   x = x1 + (a+b);
   x -= x2;
   x *= 0.5f;
   return (x);
}


class module_4float_to_float4 : public vsx_module {
  // in
  vsx_module_param_float* floata;
  vsx_module_param_float* floatb;
  vsx_module_param_float* floatc;
  vsx_module_param_float* floatd;
  // out
  vsx_module_param_float4* result4;
  // internal
public:
  virtual void module_info(vsx_module_info* info);
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  virtual void run();
};

class module_float_to_float3 : public vsx_module {
  // in
  vsx_module_param_float* param1;

  // out
  vsx_module_param_float3* result3;
  // internal
public:
  virtual void module_info(vsx_module_info* info);
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  virtual void run();
};


class module_vector_add : public vsx_module {
  // in
  vsx_module_param_float3* param1;
  vsx_module_param_float3* param2;

  // out
  vsx_module_param_float3* result3;
  // internal
public:
  virtual void module_info(vsx_module_info* info);
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  virtual void run();
};



class module_vector_add_float : public vsx_module {
  // in
  vsx_module_param_float3* param1;
  vsx_module_param_float* param2;

  // out
  vsx_module_param_float3* result3;
  // internal
public:
  virtual void module_info(vsx_module_info* info);
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  virtual void run();
};






class vsx_float3_accumulator : public vsx_module {
  // in
  vsx_module_param_float3* float3_in;
  vsx_module_param_int* reset;
  // out
  vsx_module_param_float3* result_float3;
  // internal
  float value[3];
public:

  virtual void module_info(vsx_module_info* info);
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  virtual void run();
};

class vsx_float4_accumulator : public vsx_module {
  // in
  vsx_module_param_float4* float4_in;
  vsx_module_param_int* reset;
  // out
  vsx_module_param_float4* result_float4;
  // internal
  float value[4];
public:

  virtual void module_info(vsx_module_info* info);
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  virtual void run();
};


class vsx_float_abs : public vsx_module {
  // in
  vsx_module_param_float* float_in;
  // out
  vsx_module_param_float* result_float;
  // internal
public:
  virtual void module_info(vsx_module_info* info);
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  virtual void run();
};

class vsx_float_sin : public vsx_module {
  // in
  vsx_module_param_float* float_in;
  // out
  vsx_module_param_float* result_float;
  // internal
public:
  virtual void module_info(vsx_module_info* info);
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  virtual void run();
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

class vsx_bool_and : public vsx_module {
  // in
  vsx_module_param_float* float_a;
  vsx_module_param_float* float_b;
  // out
  vsx_module_param_float* result_float;
  // internal
public:
  virtual void module_info(vsx_module_info* info);
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  virtual void run();
};

class vsx_bool_nand : public vsx_module {
  // in
  vsx_module_param_float* float_a;
  vsx_module_param_float* float_b;
  // out
  vsx_module_param_float* result_float;
  // internal
public:
  virtual void module_info(vsx_module_info* info);
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  virtual void run();
};

class vsx_bool_or : public vsx_module {
  // in
  vsx_module_param_float* float_a;
  vsx_module_param_float* float_b;
  // out
  vsx_module_param_float* result_float;
  // internal
public:
  virtual void module_info(vsx_module_info* info);
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  virtual void run();
};
class vsx_bool_nor : public vsx_module {
  // in
  vsx_module_param_float* float_a;
  vsx_module_param_float* float_b;
  // out
  vsx_module_param_float* result_float;
  // internal
public:
  virtual void module_info(vsx_module_info* info);
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  virtual void run();
};
class vsx_bool_xor : public vsx_module {
  // in
  vsx_module_param_float* float_a;
  vsx_module_param_float* float_b;
  // out
  vsx_module_param_float* result_float;
  // internal
public:
  virtual void module_info(vsx_module_info* info);
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  virtual void run();
};

class vsx_bool_not : public vsx_module {
  // in
  vsx_module_param_float* float_a;
  // out
  vsx_module_param_float* result_float;
  // internal
public:
  virtual void module_info(vsx_module_info* info);
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  virtual void run();
};


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
class vsx_float_array_pick : public vsx_module {
  // in
  vsx_module_param_float_array* float_in;
  vsx_module_param_float* which;
  // out
  vsx_module_param_float* result_float;
  // internal
  vsx_float_array* my_array;
public:
  virtual void module_info(vsx_module_info* info);
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  virtual void run();
};










class module_3float_to_float3 : public vsx_module {
  // in
  vsx_module_param_float* floata;
  vsx_module_param_float* floatb;
  vsx_module_param_float* floatc;
  // out
  vsx_module_param_float3* result3;
  // internal
public:


  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;converters;3float_to_float3";
    info->description = "[floata, floatb, floatc] are copied and \n"
                        "converted into \n"
                        "[result_float3] which is a float3\n"
                        "\n"
                        "This is a simple conversion module.\n"
                        "Whenever possible, try to set values\n"
                        "with sliders, this is only good if you\n"
                        "really need to convert";

    info->out_param_spec = "result_float3:float3";
    info->in_param_spec = "floata:float?help=`Set it to anything you like :)`,"
                          "floatb:float,"
                          "floatc:float";

    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    floata = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floata");
    floatb = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floatb");
    floatc = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floatc");

    result3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_float3");
    result3->set(0.0f, 0);
    result3->set(0.0f, 1);
    result3->set(0.0f, 2);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    result3->set(floata->get(),0);
    result3->set(floatb->get(),1);
    result3->set(floatc->get(),2);
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void module_4float_to_float4::module_info(vsx_module_info* info)
{
  info->identifier = "maths;converters;4float_to_float4";
  info->description = ""
                      "[floata, floatb, floatc, floatd] are copied and \n"
                      "converted into \n"
                      "[result_float4] which is a float3\n"
                      "\n"
                      "This is a simple conversion module.\n"
                      "Whenever possible, try to set values\n"
                      "with sliders, this is only good if you\n"
                      "really need to convert";

  info->out_param_spec = "result_float4:float4";
  info->in_param_spec = "\
floata:float,\
floatb:float,\
floatc:float,\
floatd:float";

  info->component_class = "parameters";
}

void module_4float_to_float4::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  floata = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floata");
  floatb = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floatb");
  floatc = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floatc");
  floatd = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floatd");
  floata->set(1);
  floatb->set(1);
  floatc->set(1);
  floatd->set(1);

  result4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
  result4->set(1,0);
  result4->set(1,1);
  result4->set(1,2);
  result4->set(1,3);
//--------------------------------------------------------------------------------------------------
}

void module_4float_to_float4::run() {
  result4->set(floata->get(),0);
  result4->set(floatb->get(),1);
  result4->set(floatc->get(),2);
  result4->set(floatd->get(),3);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void module_vector_add::module_info(vsx_module_info* info)
{
  info->identifier = "maths;arithmetics;vector;vector_add||maths;arithmetics;float3;float3_add_float3";
  info->description = "";

  info->out_param_spec = "result_float3:float3";
  info->in_param_spec = "\
param1:float3,\
param2:float3";
  info->component_class = "parameters";
}

void module_vector_add::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  param1 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param1");
  param2 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param2");

  result3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_float3");
  result3->set(0,0);
  result3->set(0,1);
  result3->set(0,2);
//--------------------------------------------------------------------------------------------------
}

void module_vector_add::run() {
  result3->set(param1->get(0)+param2->get(0),0);
  result3->set(param1->get(1)+param2->get(1),1);
  result3->set(param1->get(2)+param2->get(2),2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class module_float4_add : public vsx_module {
  // in
  vsx_module_param_float4* param1;
  vsx_module_param_float4* param2;

  // out
  vsx_module_param_float4* result4;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "!maths;arithmetics;vector;float4_add|maths;arithmetics;float4;float4_add";
    info->description = "";

    info->out_param_spec = "result_float4:float4";
    info->in_param_spec = ""
      "param1:float4,"
      "param2:float4";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    param1 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"param1");
    param2 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"param2");

    result4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
    result4->set(0,0);
    result4->set(0,1);
    result4->set(0,2);
    result4->set(0,3);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    result4->set(param1->get(0)+param2->get(0),0);
    result4->set(param1->get(1)+param2->get(1),1);
    result4->set(param1->get(2)+param2->get(2),2);
    result4->set(param1->get(3)+param2->get(3),3);
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void module_vector_add_float::module_info(vsx_module_info* info)
{
  info->identifier = "maths;arithmetics;vector;vector_add_float||maths;arithmetics;float3;float3_add_float";
  info->description = "";

  info->out_param_spec = "result_float3:float3";
  info->in_param_spec = "\
param1:float3,\
param2:float";
  info->component_class = "parameters";
}

void module_vector_add_float::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  param1 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param1");
  param2 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param2");

  result3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_float3");
  result3->set(0,0);
  result3->set(0,1);
  result3->set(0,2);
//--------------------------------------------------------------------------------------------------
}

void module_vector_add_float::run() {
  result3->set(param1->get(0)+param2->get(),0);
  result3->set(param1->get(1)+param2->get(),1);
  result3->set(param1->get(2)+param2->get(),2);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class module_vector_mul_float : public vsx_module {
  // in
  vsx_module_param_float3* param1;
  vsx_module_param_float* param2;

  // out
  vsx_module_param_float3* result3;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;vector;vector_mul_float||maths;arithmetics;float3;float3_mul_float";
    info->description = "multiplies each of the values in\n"
                        "the float3 vector with the float\n"
                        "value in param1";

    info->out_param_spec = "result_float3:float3";
    info->in_param_spec = "param1:float3,"
                          "param2:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    param1 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param1");
    param2 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param2");

    result3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_float3");
    result3->set(0,0);
    result3->set(0,1);
    result3->set(0,2);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    result3->set(param1->get(0)*param2->get(),0);
    result3->set(param1->get(1)*param2->get(),1);
    result3->set(param1->get(2)*param2->get(),2);
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class module_vector_normalize : public vsx_module {
  // in
  vsx_module_param_float3* param1;
  // out
  vsx_module_param_float3* result3;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;vector;vector_normalize";
    info->description = "Normalizes the vector (length = 1.0)\n";

    info->out_param_spec = "result_float3:float3";
    info->in_param_spec = "param1:float3";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    param1 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param1");
    result3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_float3");
    result3->set(0,0);
    result3->set(0,1);
    result3->set(0,2);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    float v = (float)(1.0 / sqrt( param1->get(0)*param1->get(0) + param1->get(1)*param1->get(1) + param1->get(2)*param1->get(2) ));
    result3->set(param1->get(0)*v,0);
    result3->set(param1->get(1)*v,1);
    result3->set(param1->get(2)*v,2);
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class module_vector_cross_product : public vsx_module {
  // in
  vsx_module_param_float3* param1;
  vsx_module_param_float3* param2;
  // out
  vsx_module_param_float3* result3;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;vector;vector_cross_product";
    info->description = "Vector cross product - \n"
                        "Result is perpendicular to the angle formed\n"
                        "by the input vectors.";

    info->out_param_spec = "result_float3:float3";
    info->in_param_spec = "param1:float3, param2:float3";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    param1 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param1");
    param2 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param2");
    result3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_float3");
    result3->set(0,0);
    result3->set(0,1);
    result3->set(0,2);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    //x = (u.y*v.z) - (u.z*v.y);
    //y = (u.z*v.x) - (u.x*v.z);
    //z = (u.x*v.y) - (u.y*v.x);
    result3->set(  param1->get(1) * param2->get(2)  -  param1->get(2) * param2->get(1)  ,0);
    result3->set(  param1->get(2) * param2->get(0)  -  param1->get(0) * param2->get(2)  ,1);
    result3->set(  param1->get(0) * param2->get(1)  -  param1->get(1) * param2->get(0)  ,2);
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
class module_vector_dot_product : public vsx_module {
  // in
  vsx_module_param_float3* param1;
  vsx_module_param_float3* param2;
  // out
  vsx_module_param_float* result;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;vector;vector_dot_product";
    info->description = "Vector dot product - \n"
                        "'Multiply' one vector by another";
    info->in_param_spec = "param1:float3, param2:float3";
    info->out_param_spec = "result_float:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    param1 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param1");
    param2 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param2");
    result = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result->set(0.0f);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    result->set(
        param1->get(0) * param2->get(0)
        +
        param1->get(1) * param2->get(1)
        +
        param1->get(2) * param2->get(2)
    );
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class module_vector_from_points : public vsx_module {
  // in
  vsx_module_param_float3* param1;
  vsx_module_param_float3* param2;
  // out
  vsx_module_param_float3* result3;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;vector;vector_from_points";
    info->description = "Returns a directional vector from \n"
                        "a ---> b";

    info->out_param_spec = "result_float3:float3";
    info->in_param_spec = "param1:float3, param2:float3";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    param1 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param1");
    param2 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"param2");
    result3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_float3");
    result3->set(0,0);
    result3->set(0,1);
    result3->set(0,2);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    result3->set(  param2->get(0) - param1->get(0) ,0);
    result3->set(  param2->get(1) - param1->get(1) ,1);
    result3->set(  param2->get(2) - param1->get(2) ,2);
  }

};


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class module_vector_4float_to_quaternion : public vsx_module {
  // in
  vsx_module_param_float* param1;
  vsx_module_param_float* param2;
  vsx_module_param_float* param3;
  vsx_module_param_float* param4;

  // out
  vsx_module_param_quaternion* result_quat;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;converters;4float_to_quaternion";
    info->description = "takes 4 float values and forms a\n"
                        "quaternion\n";

    info->out_param_spec = "result_quat:quaternion";
    info->in_param_spec = "param1:float,"
                          "param2:float,"
                          "param3:float,"
                          "param4:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    param1 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param1");
    param2 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param2");
    param3 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param3");
    param4 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param4");

    result_quat = (vsx_module_param_quaternion*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"result_quat");
    result_quat->set(0.0f,0);
    result_quat->set(0.0f,1);
    result_quat->set(0.0f,2);
    result_quat->set(1.0f,3);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    result_quat->set(param1->get(),0);
    result_quat->set(param2->get(),1);
    result_quat->set(param3->get(),2);
    result_quat->set(param4->get(),3);
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class module_vector_float4_to_4float : public vsx_module {
  // in
  vsx_module_param_float4* in_float4;

  // out
  vsx_module_param_float* param1;
  vsx_module_param_float* param2;
  vsx_module_param_float* param3;
  vsx_module_param_float* param4;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;converters;float4_to_4float";
    info->description = "takes float4\n"
                        "outputs 4 floats\n";

    info->in_param_spec = "in_float4:float4";
    info->out_param_spec = "param1:float,"
                           "param2:float,"
                           "param3:float,"
                           "param4:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    in_float4 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"in_float4");
    in_float4->set(0.0f,0);
    in_float4->set(0.0f,1);
    in_float4->set(0.0f,2);
    in_float4->set(1.0f,3);

    //--------------------------------------------------------------------------------------------------

    param1 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param1");
    param2 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param2");
    param3 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param3");
    param4 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param4");
    param1->set(0.0f);
    param2->set(0.0f);
    param3->set(0.0f);
    param4->set(0.0f);

  }

  void run() {
    param1->set(in_float4->get(0));
    param2->set(in_float4->get(1));
    param3->set(in_float4->get(2));
    param4->set(in_float4->get(3));
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


class module_vector_quaternion_to_4float : public vsx_module {
  // in
  vsx_module_param_quaternion* in_quat;

  // out
  vsx_module_param_float* param1;
  vsx_module_param_float* param2;
  vsx_module_param_float* param3;
  vsx_module_param_float* param4;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;converters;quaternion_to_4float";
    info->description = "takes quaternion\n"
                        "outputs 4 floats\n";

    info->in_param_spec = "in_quat:quaternion";
    info->out_param_spec = "param1:float,"
                           "param2:float,"
                           "param3:float,"
                           "param4:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    in_quat = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"in_quat");
    in_quat->set(0.0f,0);
    in_quat->set(0.0f,1);
    in_quat->set(0.0f,2);
    in_quat->set(1.0f,3);

    //--------------------------------------------------------------------------------------------------

    param1 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param1");
    param2 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param2");
    param3 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param3");
    param4 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param4");
    param1->set(0.0f);
    param2->set(0.0f);
    param3->set(0.0f);
    param4->set(1.0f);
    
  }

  void run() {
    param1->set(in_quat->get(0));
    param2->set(in_quat->get(1));
    param3->set(in_quat->get(2));
    param4->set(in_quat->get(3));
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class module_quaternion_rotation_accumulator_2d : public vsx_module {
  // in
  vsx_module_param_float* param_x;
  vsx_module_param_float* param_y;

  // out
  vsx_module_param_quaternion* result_quat;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;accumulators;quat_rotation_accum_2d";
    info->description = "It's like the rotation sphere,\n"
                        "you can rotate it step by step\n"
                        "by x or y";
    info->out_param_spec = "result_quat:quaternion";
    info->in_param_spec = "param_x:float,"
                          "param_y:float";
    info->component_class = "parameters";
  }

  vsx_quaternion q1,q_out;

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    param_x = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param_x");
    param_y = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param_y");
    q_out.x = q_out.y = q_out.z = 0.0f;
    q_out.w = 1.0f;
    result_quat = (vsx_module_param_quaternion*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"result_quat");
    result_quat->set(0.0f,0);
    result_quat->set(0.0f,1);
    result_quat->set(0.0f,2);
    result_quat->set(1.0f,3);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    q1.x = 0.0f;
    q1.w = 1.0f;
    q1.y = param_x->get();
    q1.z = 0.0f;
    q1.normalize();
    q_out.mul(q_out, q1);

    q1.x = 0.0f;
    q1.w = 1.0f;
    q1.y = 0.0f;
    q1.z = param_y->get();
    q1.normalize();
    q_out.mul(q_out, q1);

    result_quat->set(q_out.x,0);
    result_quat->set(q_out.y,1);
    result_quat->set(q_out.z,2);
    result_quat->set(q_out.w,3);
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class module_quaternion_slerp_2 : public vsx_module {
  // in
  vsx_module_param_quaternion* quat_a;
  vsx_module_param_quaternion* quat_b;
  vsx_module_param_float* pos;
  // out
  vsx_module_param_quaternion* result_quat;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;interpolation;quat_slerp_2p";
    info->description = "Rotate seamlessly between 2 angles/rotations";
    info->out_param_spec = "result_quat:quaternion";
    info->in_param_spec = "quat_a:quaternion,"
                          "quat_b:quaternion,"
                          "pos:float";
    info->component_class = "parameters";
  }

  vsx_quaternion q1,q2,q_out;

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    quat_a = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION, "quat_a");
    quat_b = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION, "quat_b");
    pos = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "pos");
    quat_a->set(0.0f, 0);
    quat_b->set(0.0f, 0);
    quat_a->set(0.0f, 1);
    quat_b->set(0.0f, 1);
    quat_a->set(0.0f, 2);
    quat_b->set(0.0f, 2);
    quat_a->set(1.0f, 3);
    quat_b->set(1.0f, 3);
    q_out.x = q_out.y = q_out.z = 0.0f;
    q_out.w = 1.0f;
    result_quat = (vsx_module_param_quaternion*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"result_quat");
    result_quat->set(0.0f,0);
    result_quat->set(0.0f,1);
    result_quat->set(0.0f,2);
    result_quat->set(1.0f,3);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    q1.x = quat_a->get(0);
    q1.y = quat_a->get(1);
    q1.z = quat_a->get(2);
    q1.w = quat_a->get(3);
    q1.normalize();
    q2.x = quat_b->get(0);
    q2.y = quat_b->get(1);
    q2.z = quat_b->get(2);
    q2.w = quat_b->get(3);
    q2.normalize();
    float p = pos->get();
    if (p < 0.0f) p = 0.0f;
    if (p > 1.0f) p = 1.0f;
    q_out.slerp(q1, q2, p);

    result_quat->set(q_out.x,0);
    result_quat->set(q_out.y,1);
    result_quat->set(q_out.z,2);
    result_quat->set(q_out.w,3);
  }

};


class module_quaternion_slerp_3 : public vsx_module {
  // in
  vsx_module_param_quaternion* quat_a;
  vsx_module_param_quaternion* quat_b;
  vsx_module_param_quaternion* quat_c;
  vsx_module_param_float* pos;
  // out
  vsx_module_param_quaternion* result_quat;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;interpolation;quat_slerp_3p";
    info->description = "Rotate seamlessly between 3 angles/rotations\nCan be used to make full laps/rotations.";
    info->out_param_spec = "result_quat:quaternion";
    info->in_param_spec = "quat_a:quaternion,"
                          "quat_b:quaternion,"
                          "quat_c:quaternion,"
                          "pos:float";
    info->component_class = "parameters";
  }

  vsx_quaternion q1,q2,q_out;

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    quat_a = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION, "quat_a");
    quat_b = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION, "quat_b");
    quat_c = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION, "quat_c");
    pos = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "pos");
    quat_a->set(0.0f, 0);
    quat_b->set(0.0f, 0);
    quat_c->set(0.0f, 0);
    quat_a->set(0.0f, 1);
    quat_b->set(0.0f, 1);
    quat_c->set(0.0f, 1);
    quat_a->set(0.0f, 2);
    quat_b->set(0.0f, 2);
    quat_c->set(0.0f, 2);
    quat_a->set(1.0f, 3);
    quat_b->set(1.0f, 3);
    quat_c->set(1.0f, 3);
    q_out.x = q_out.y = q_out.z = 0.0f;
    q_out.w = 1.0f;
    result_quat = (vsx_module_param_quaternion*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"result_quat");
    result_quat->set(0.0f,0);
    result_quat->set(0.0f,1);
    result_quat->set(0.0f,2);
    result_quat->set(1.0f,3);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    float p = pos->get();
    if (p < 0.5f)
    {
      q1.x = quat_a->get(0);
      q1.y = quat_a->get(1);
      q1.z = quat_a->get(2);
      q1.w = quat_a->get(3);
      q1.normalize();
      q2.x = quat_b->get(0);
      q2.y = quat_b->get(1);
      q2.z = quat_b->get(2);
      q2.w = quat_b->get(3);
      q2.normalize();
    } else
    {
      q1.x = quat_b->get(0);
      q1.y = quat_b->get(1);
      q1.z = quat_b->get(2);
      q1.w = quat_b->get(3);
      q1.normalize();
      q2.x = quat_c->get(0);
      q2.y = quat_c->get(1);
      q2.z = quat_c->get(2);
      q2.w = quat_c->get(3);
      q2.normalize();
    }


    if (p < 0.0f) p = 0.0f;
    if (p >= 1.0f) {
      p = 1.0f;
    } else
    p = fmod(p,0.5f)*2.0f;

    q_out.slerp(q1, q2, p);

    result_quat->set(q_out.x,0);
    result_quat->set(q_out.y,1);
    result_quat->set(q_out.z,2);
    result_quat->set(q_out.w,3);
  }

};


class module_quaternion_mul : public vsx_module {
  // in
  vsx_module_param_quaternion* quat_a;
  vsx_module_param_quaternion* quat_b;
  // out
  vsx_module_param_quaternion* result_quat;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;quaternion;quat_mul";
    info->description = "Rotate quaternion a by quaternion b";
    info->out_param_spec = "result_quat:quaternion";
    info->in_param_spec = "quat_a:quaternion,"
                          "quat_b:quaternion";
    info->component_class = "parameters";
  }

  vsx_quaternion q1,q2,q_out;

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    quat_a = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION, "quat_a");
    quat_b = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION, "quat_b");
    quat_a->set(0.0f, 0);
    quat_b->set(0.0f, 0);
    quat_a->set(0.0f, 1);
    quat_b->set(0.0f, 1);
    quat_a->set(0.0f, 2);
    quat_b->set(0.0f, 2);

    quat_a->set(1.0f, 3);
    quat_b->set(1.0f, 3);
    q_out.x = q_out.y = q_out.z = 0.0f;
    q_out.w = 1.0f;
    result_quat = (vsx_module_param_quaternion*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"result_quat");
    result_quat->set(0.0f,0);
    result_quat->set(0.0f,1);
    result_quat->set(0.0f,2);
    result_quat->set(1.0f,3);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    q1.x = quat_a->get(0);
    q1.y = quat_a->get(1);
    q1.z = quat_a->get(2);
    q1.w = quat_a->get(3);
    q1.normalize();
    q2.x = quat_b->get(0);
    q2.y = quat_b->get(1);
    q2.z = quat_b->get(2);
    q2.w = quat_b->get(3);
    q2.normalize();
    q_out.mul(q1, q2);

    result_quat->set(q_out.x,0);
    result_quat->set(q_out.y,1);
    result_quat->set(q_out.z,2);
    result_quat->set(q_out.w,3);
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class module_vec4_mul_float : public vsx_module {
  // in
  vsx_module_param_float4* param1;
  vsx_module_param_float* param2;
  // out
  vsx_module_param_float4* result4;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "!maths;arithmetics;vec4;float4_mul_float||maths;arithmetics;float4;float4_mul_float";
    info->description = "multiplies each of the values in\n"
                        "the float4 vector with the float\n"
                        "value in param1";
    info->in_param_spec = "param1:float4,param2:float";
    info->out_param_spec = "result_float4:float4";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    param1 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"param1");
    param1->set(0,0);
    param1->set(0,1);
    param1->set(0,2);
    param1->set(0,3);
    param2 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param2");

    result4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
    result4->set(0,0);
    result4->set(0,1);
    result4->set(0,2);
    result4->set(0,3);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    result4->set(param1->get(0)*param2->get(),0);
    result4->set(param1->get(1)*param2->get(),1);
    result4->set(param1->get(2)*param2->get(),2);
    result4->set(param1->get(3)*param2->get(),3);
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void module_float_to_float3::module_info(vsx_module_info* info)
{
  info->identifier = "maths;converters;float_to_float3";
  info->description = "float to float3 (vector) conversion\n"
                      "param1 is copied into all 3 of the\n"
                      "values in the float3 parameter\n"
                      "\n"
                      "useful for generating a uniform\n"
                      "scaling vector for instance";
  info->in_param_spec = "param1:float";
  info->out_param_spec = "result_float3:float3";
  info->component_class = "parameters";
}

void module_float_to_float3::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  param1 = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"param1");

  result3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_float3");
  result3->set(0,0);
  result3->set(0,1);
  result3->set(0,2);
//--------------------------------------------------------------------------------------------------
}

void module_float_to_float3::run() {
  result3->set(param1->get(),0);
  result3->set(param1->get(),1);
  result3->set(param1->get(),2);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void vsx_float_array_pick::module_info(vsx_module_info* info)
{
  info->identifier = "maths;array;float_array_pick";
  info->description = "picks the value from the array/list\n"
                      "and delivers it as out parameter";
  info->in_param_spec = "float_in:float_array,which:float";
  info->out_param_spec = "result_float:float";
  info->component_class = "parameters";
}

void vsx_float_array_pick::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  float_in = (vsx_module_param_float_array*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"float_in");
  which = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"which");
  which->set(0);

  result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
  result_float->set(0);
//--------------------------------------------------------------------------------------------------
}

void vsx_float_array_pick::run() {
  my_array = float_in->get_addr();
  if (my_array)
  result_float->set((*(my_array->data))[(int)which->get()]);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


class vsx_float_array_average : public vsx_module {
  // in
  vsx_module_param_float_array* float_in;
  vsx_module_param_float* start;
  vsx_module_param_float* end;
  // out
  vsx_module_param_float* result_float;
  // internal
  vsx_float_array* my_array;
public:



  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;array;float_array_average";
    info->description = "Calculates the average of all\n\
values in the array";
    info->in_param_spec = "float_in:float_array,start:float,end:float";
    info->out_param_spec = "result_float:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    float_in = (vsx_module_param_float_array*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"float_in");

    start = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"start");
    start->set(0);
    end = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"end");
    end->set(0);

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    my_array = float_in->get_addr();
    if (my_array) {
      float temp = 0;
      float divisor = 0;
      if (end->get() > start->get()) {
        float x = (float)ceil(start->get());
        float x_e = (float) floor(end->get());
        float x_e_f = end->get();
        if (x_e > (*(my_array->data)).size()-1) {
          x_e = (*(my_array->data)).size()-1;
          x_e_f = x_e;
        }
        divisor += x - start->get();
        temp += (*(my_array->data))[(int)x-1]*(x - start->get());
        for (; x < x_e; ++x) {
          temp += (*(my_array->data))[(int)x];
        }
        divisor += x_e_f - x_e;
        temp += (*(my_array->data))[(int)x_e+1]*(x_e_f - x_e);
        temp /= divisor;
        result_float->set(temp);
      }
    }
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class vsx_float_accumulator : public vsx_module
{
  // in
  vsx_module_param_float* float_in;
  vsx_module_param_int* reset;
  // out
  vsx_module_param_float* result_float;
  // internal
  float value;
public:


  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;accumulators;float_accumulator";
    info->description = "accumulator - float\n"
                        "result += in\n"
                        "\n"
                        "adds the value in float_in\n"
                        "to its current value once\n"
                        "per frame";
    info->out_param_spec = "result_float:float";
    info->in_param_spec = "float_in:float,reset:enum?ok";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_in");
    value = 0;
    reset = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"reset");
    reset->set(-1);
    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0.0f);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    if (reset->get() == 0) {
      reset->set(-1);
      value = 0;
    }
    value += float_in->get();
    result_float->set(value);
  }

};
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class vsx_float_accumulator_limits : public vsx_module
{
  // in
  vsx_module_param_float* float_in;
  vsx_module_param_float* limit_lower;
  vsx_module_param_float* limit_upper;
  vsx_module_param_int* reset;
  // out
  vsx_module_param_float* result_float;
  // internal
  float value;
public:


  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;accumulators;float_accumulator_limits";
    info->description = "accumulator - float with limits\n"
                        "result += in\n"
                        "\n"
                        "adds the value in float_in\n"
                        "to its current value once\n"
                        "per frame";
    info->out_param_spec = "result_float:float";
    info->in_param_spec = "float_in:float,limit_lower:float,limit_upper:float,reset:enum?ok";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_in");
    limit_lower = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"limit_lower");
    limit_upper = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"limit_upper");
    limit_upper->set(1.0f);
    value = 0;
    reset = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"reset");
    reset->set(-1);
    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0.0f);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    if (reset->get() == 0) {
      reset->set(-1);
      value = 0;
    }
    value += float_in->get();
    if (value > limit_upper->get()) value = limit_upper->get();
    if (value < limit_lower->get()) value = limit_lower->get();
    result_float->set(value);
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void vsx_float3_accumulator::module_info(vsx_module_info* info)
{
  info->identifier = "maths;accumulators;float3_accumulator";
  info->description = "accumulator - float3\n"
                      "result += in\n"
                      "\n"
                      "adds the value in float3_in\n"
                      "to its current value once\n"
                      "per frame";
  info->out_param_spec = "result_float3:float3";
  info->in_param_spec = "float3_in:float3,reset:enum?ok";
  info->component_class = "parameters";
}

void vsx_float3_accumulator::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  float3_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"float3_in");
  value[0] = 0;
  value[1] = 0;
  value[2] = 0;
  reset = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"reset");
  reset->set(-1);
  result_float3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_float3");
  result_float3->set(0,0);
  result_float3->set(0,1);
  result_float3->set(0,2);
//--------------------------------------------------------------------------------------------------
}

void vsx_float3_accumulator::run() {
  if (reset->get() == 0) {
    reset->set(-1);
    value[0] = 0;
    value[1] = 0;
    value[2] = 0;
  }
  value[0] += float3_in->get(0);
  value[1] += float3_in->get(1);
  value[2] += float3_in->get(2);
  result_float3->set(value[0],0);
  result_float3->set(value[1],1);
  result_float3->set(value[2],2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void vsx_float4_accumulator::module_info(vsx_module_info* info)
{
  info->identifier = "maths;accumulators;float4_accumulator";
  info->description = "accumulator - float4\n"
                      "result += in\n"
                      "\n"
                      "adds the value in float4_in\n"
                      "to its current value once\n"
                      "per frame";
  info->out_param_spec = "result_float4:float4";
  info->in_param_spec = "float4_in:float4,reset:enum?ok";
  info->component_class = "parameters";
}

void vsx_float4_accumulator::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  float4_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"float4_in");
  value[0] = 0;
  value[1] = 0;
  value[2] = 0;
  value[3] = 0;
  reset = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"reset");
  reset->set(-1);
  result_float4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
  result_float4->set(0,0);
  result_float4->set(0,1);
  result_float4->set(0,2);
  result_float4->set(0,3);
//--------------------------------------------------------------------------------------------------
}

void vsx_float4_accumulator::run() {
  if (reset->get() == 0) {
    reset->set(-1);
    value[0] = 0;
    value[1] = 0;
    value[2] = 0;
    value[3] = 0;
  }
  value[0] += float4_in->get(0);
  value[1] += float4_in->get(1);
  value[2] += float4_in->get(2);
  value[3] += float4_in->get(3);
  result_float4->set(value[0],0);
  result_float4->set(value[1],1);
  result_float4->set(value[2],2);
  result_float4->set(value[3],3);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class vsx_float_dummy : public vsx_module {
  // in
  vsx_module_param_float* float_in;
  // out
  vsx_module_param_float* result_float;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;dummies;float_dummy";
    info->description =
      "[floatin] is copied without modification into\n"
      "[result_float]\n"
      "\n"
      "This module is good for distributing a value you want to\n"
      "set to many components. Or host to code\n"
      "a filter in VSXL. For instance if you\n"
      "have many components in a macro which need\n"
      "the same value but you don't want 10\n"
      "aliases going out, it's a good idea to\n"
      "keep one inside your macro.\n"
      ;
    info->out_param_spec = "result_float:float";
    info->in_param_spec = "float_in:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_in");
    float_in->set(1);

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(1);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    result_float->set(float_in->get());
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class vsx_float_limit : public vsx_module
{
  // in
  vsx_module_param_float* float_in;
  vsx_module_param_float* limit_value;
  vsx_module_param_int* type;
  // out
  vsx_module_param_float* result_float;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;limiters;float_limiter";
    info->description = "If value_in is la\n"
                        "limit_value, result is set to\n"
                        "limit_value, otherwise result\n"
                        "is set to value_in.\n";
    info->out_param_spec = "result_float:float";
    info->in_param_spec = "value_in:float,limit_value:float,type:enum?max|min";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"value_in");
    float_in->set(0.0f);
    limit_value = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"limit_value");
    limit_value->set(1.0f);
    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0.0f);
    type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"type");
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    if (type->get() == 0)
    {
      if (float_in->get() > limit_value->get())
      {
        result_float->set(limit_value->get());
      } else
      result_float->set(float_in->get());
    } else
    {
      if (float_in->get() < limit_value->get())
      {
        result_float->set(limit_value->get());
      } else
      result_float->set(float_in->get());
    }
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class vsx_float_smooth : public vsx_module
{
  // in
  vsx_module_param_float* float_in;
  vsx_module_param_float* speed;
  // out
  vsx_module_param_float* result_float;
  // internal
  double internal_value;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "!maths;interpolation;float_interpolation||maths;interpolation;float_smoother";
    info->description = "Smoothens a value over time\n"
                        " - same algorith as the knobs/sliders";
    info->out_param_spec = "result_float:float";
    info->in_param_spec = "value_in:float,speed:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"value_in");
    float_in->set(0.0f);
    speed = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"speed");
    speed->set(1.0f);
    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0.0f);
    internal_value = 0.0f;
  //--------------------------------------------------------------------------------------------------
  }
  void run() {
    double tt = engine->dtime*speed->get();
    if (tt > 1.0) tt = 1.0;
    double temp = (float)(internal_value*(1.0-tt)+(double)float_in->get() * tt);
    //printf("temp: %f\n",temp);
    internal_value = temp;
    result_float->set((float)internal_value);
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class vsx_float_interpolate : public vsx_module
{
  // in
  vsx_module_param_float* float_in_a;
  vsx_module_param_float* float_in_b;
  vsx_module_param_float* pos;
  // out
  vsx_module_param_float* result_float;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;interpolation;float_interpolate";
    info->description = "linear interpolation for the value\npos ranges from 0.0 to 1.0";
    info->in_param_spec = "float_in_a:float,float_in_b:float,pos:float";
    info->out_param_spec = "result_float:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    float_in_a = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_in_a");
    float_in_b = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_in_b");
    pos = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"pos");
    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0.0f);
  }
  void run() {
    float p = pos->get();
    if (p < 0.0f) p = 0.0f;
    else // optimization
    if (p > 1.0f) p = 1.0f;
    float p1 = 1.0f - p;
    result_float->set( float_in_a->get(0) * p1 + float_in_b->get(0) * p, 0);
  }
};
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class vsx_float3_interpolate : public vsx_module
{
  // in
  vsx_module_param_float3* float3_in_a;
  vsx_module_param_float3* float3_in_b;
  vsx_module_param_float* pos;
  // out
  vsx_module_param_float3* result_float3;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;interpolation;float3_interpolate";
    info->description = "linear interpolation for the value\npos ranges from 0.0 to 1.0";
    info->in_param_spec = "float3_in_a:float3,float3_in_b:float3,pos:float";
    info->out_param_spec = "result_float3:float3";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    float3_in_a = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"float3_in_a");
    float3_in_b = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"float3_in_b");
    pos = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"pos");
    result_float3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"result_float3");
    result_float3->set(0.0f, 0);
    result_float3->set(0.0f, 1);
    result_float3->set(0.0f, 2);
  }
  void run() {
    float p = pos->get();
    if (p < 0.0f) p = 0.0f;
    else // optimization
    if (p > 1.0f) p = 1.0f;
    float p1 = 1.0f - p;
    result_float3->set( float3_in_a->get(0) * p1 + float3_in_b->get(0) * p, 0);
    result_float3->set( float3_in_a->get(1) * p1 + float3_in_b->get(1) * p, 1);
    result_float3->set( float3_in_a->get(2) * p1 + float3_in_b->get(2) * p, 2);
  }

};


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class vsx_float4_interpolate : public vsx_module
{
  // in
  vsx_module_param_float4* float4_in_a;
  vsx_module_param_float4* float4_in_b;
  vsx_module_param_float* pos;
  // out
  vsx_module_param_float4* result_float4;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;interpolation;float4_interpolate||maths;color;color_linear_interpolate";
    info->description = "linear interpolation for the value\npos ranges from 0.0 to 1.0";
    info->in_param_spec = "float4_in_a:float4,float4_in_b:float4,pos:float";
    info->out_param_spec = "result_float4:float4";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    float4_in_a = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"float4_in_a");
    float4_in_b = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"float4_in_b");
    pos = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"pos");
    result_float4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
    result_float4->set(0.0f, 0);
    result_float4->set(0.0f, 1);
    result_float4->set(0.0f, 2);
    result_float4->set(0.0f, 3);
  }
  void run() {
    float p = pos->get();
    if (p < 0.0f) p = 0.0f;
    else // optimization
    if (p > 1.0f) p = 1.0f;
    float p1 = 1.0f - p;
    result_float4->set( float4_in_a->get(0) * p1 + float4_in_b->get(0) * p, 0);
    result_float4->set( float4_in_a->get(1) * p1 + float4_in_b->get(1) * p, 1);
    result_float4->set( float4_in_a->get(2) * p1 + float4_in_b->get(2) * p, 2);
    result_float4->set( float4_in_a->get(3) * p1 + float4_in_b->get(3) * p, 3);
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void vsx_float_abs::module_info(vsx_module_info* info)
{
  info->identifier = "maths;arithmetics;binary;abs";
  info->description = "abs:\n"
                      "result = abs( float_in )\n"
                      "turns negative values into positive\n"
                      "for instance, -0.67 becomes 0.67";
  info->out_param_spec = "result_float:float";
  info->in_param_spec = "float_in:float";
  info->component_class = "parameters";
}

void vsx_float_abs::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_in");

  result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
  result_float->set(0);
//--------------------------------------------------------------------------------------------------
}

void vsx_float_abs::run() {
  result_float->set((float)fabs(float_in->get()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void vsx_float_sin::module_info(vsx_module_info* info)
{
  info->identifier = "maths;arithmetics;binary;sin";
  info->description = "sin:\n"
                      "result = sin( float_in )\n"
                      "returns the sine of float_in given\n"
                      "in radians";
  info->out_param_spec = "result_float:float";
  info->in_param_spec = "float_in:float";
  info->component_class = "parameters";
}

void vsx_float_sin::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_in");

  result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
  result_float->set(0);
//--------------------------------------------------------------------------------------------------
}

void vsx_float_sin::run() {
  result_float->set((float)sin(float_in->get()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class vsx_float_cos : public vsx_module {
  // in
  vsx_module_param_float* float_in;
  // out
  vsx_module_param_float* result_float;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;binary;cos";
    info->description = "cos:\n"
                        "result = cos( float_in )\n"
                        "returns the cosine of float_in given\n"
                        "in radians";
    info->in_param_spec = "float_in:float";
    info->out_param_spec = "result_float:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_in");

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    result_float->set((float)cos(float_in->get()));
  }

};



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class vsx_float_acos : public vsx_module {
  // in
  vsx_module_param_float* float_in;
  // out
  vsx_module_param_float* result_float;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;arithmetics;binary;acos";
    info->description = "acos:\n"
                        "result = acos( float_in )\n"
                        "returns the arcus cosine\n"
                        "of float_in given\n"
                        "in radians";
    info->in_param_spec = "float_in:float";
    info->out_param_spec = "result_float:float";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"float_in");

    result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
    result_float->set(0);
  //--------------------------------------------------------------------------------------------------
  }

  void run() {
    result_float->set((float)acos(float_in->get()));
  }

};
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void vsx_bool_and::module_info(vsx_module_info* info)
{
  info->identifier = "maths;arithmetics;boolean;and";
  info->description = "boolean and:\n"
                      "result = a and b\n"
                      "the logic operation and:\n"
                      "1 and 0 = 0\n"
                      "0 and 1 = 0\n"
                      "0 and 0 = 0\n"
                      "1 and 1 = 1\n"
                      "\n"
                      "Note: 1 is defined as\n"
                      "larger than +/-0.5";
  info->in_param_spec = "a:float,b:float";
  info->out_param_spec = "result_float:float";
  info->component_class = "parameters";
}

void vsx_bool_and::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  float_a = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"a");
  float_b = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"b");
  result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
  result_float->set(0);
//--------------------------------------------------------------------------------------------------
}

void vsx_bool_and::run() {
  result_float->set((float)(round(float_a->get()) && round(float_b->get())));
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void vsx_bool_nand::module_info(vsx_module_info* info)
{
  info->identifier = "maths;arithmetics;boolean;nand";
  info->description = "boolean nand:\n"
                      "result = !(a and b)\n"
                      "the logic operation nand:\n"
                      "1 nand 0 = 1\n"
                      "0 nand 1 = 1\n"
                      "0 nand 0 = 1\n"
                      "1 nand 1 = 0\n"
                      "\n"
                      "Note: 1 is defined as\n"
                      "larger than +/-0.5";
  info->in_param_spec = "a:float,b:float";
  info->out_param_spec = "result_float:float";
  info->component_class = "parameters";
}

void vsx_bool_nand::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  float_a = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"a");
  float_b = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"b");
  result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
  result_float->set(0);
//--------------------------------------------------------------------------------------------------
}

void vsx_bool_nand::run() {
  result_float->set((float)!(round(float_a->get()) && round(float_b->get())));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void vsx_bool_or::module_info(vsx_module_info* info)
{
  info->identifier = "maths;arithmetics;boolean;or";
  info->description = "boolean or:\n"
                      "result = a or b\n"
                      "the logic operation or:\n"
                      "1 or 0 = 1\n"
                      "0 or 1 = 1\n"
                      "0 or 0 = 0\n"
                      "1 or 1 = 1\n"
                      "\n"
                      "Note: 1 is defined as\n"
                      "larger than +/-0.5";
  info->in_param_spec = "a:float,b:float";
  info->out_param_spec = "result_float:float";
  info->component_class = "parameters";
}

void vsx_bool_or::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  float_a = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"a");
  float_b = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"b");
  result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
  result_float->set(0);
//--------------------------------------------------------------------------------------------------
}

void vsx_bool_or::run() {
  result_float->set((float)(round(float_a->get()) || round(float_b->get())));
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void vsx_bool_nor::module_info(vsx_module_info* info)
{
  info->identifier = "maths;arithmetics;boolean;nor";
  info->description = "boolean or:\n"
                      "result = !(a or b)\n"
                      "the logic operation or:\n"
                      "1 nor 0 = 0\n"
                      "0 nor 1 = 0\n"
                      "0 nor 0 = 1\n"
                      "1 nor 1 = 0\n"
                      "\n"
                      "Note: 1 is defined as\n"
                      "larger than +/-0.5";
  info->in_param_spec = "a:float,b:float";
  info->out_param_spec = "result_float:float";
  info->component_class = "parameters";
}

void vsx_bool_nor::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  float_a = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"a");
  float_b = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"b");
  result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
  result_float->set(0);
//--------------------------------------------------------------------------------------------------
}

void vsx_bool_nor::run() {
  result_float->set((float)!(round(float_a->get()) || round(float_b->get())));
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void vsx_bool_xor::module_info(vsx_module_info* info)
{
  info->identifier = "maths;arithmetics;boolean;xor";
  info->description = "";
  info->in_param_spec = "a:float,b:float";
  info->out_param_spec = "result_float:float";
  info->component_class = "parameters";
}

void vsx_bool_xor::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  float_a = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"a");
  float_b = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"b");
  result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
  result_float->set(0);
//--------------------------------------------------------------------------------------------------
}

void vsx_bool_xor::run() {
  result_float->set((float)((bool)round(float_a->get()) ^ (bool)round(float_b->get())));
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void vsx_bool_not::module_info(vsx_module_info* info)
{
  info->identifier = "maths;arithmetics;boolean;not";
  info->description = "boolean not:\n"
                      "result = !a\n"
                      "the logic operation not:\n"
                      "not 1 = 0\n"
                      "not 0 = 1\n"
                      "\n"
                      "Note: 1 is defined as\n"
                      "larger than +/-0.5";
  info->in_param_spec = "a:float";
  info->out_param_spec = "result_float:float";
  info->component_class = "parameters";
}

void vsx_bool_not::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  float_a = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"a");
  result_float = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"result_float");
  result_float->set(0);
//--------------------------------------------------------------------------------------------------
}

void vsx_bool_not::run() {
  result_float->set((float)(!round(float_a->get())));
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class vsx_float3_dummy : public vsx_module {
  // in
  vsx_module_param_float3* float3_in;
  // out
  vsx_module_param_float3* result_float3;
  // internal
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;dummies;float3_dummy";
    info->description = "This is only to gather connections\n"
                        " - inside a macro for instance if \n"
                        "you have many components which need\n"
                        "to read the same value but you don't\n"
                        "want 10 aliases going out.\n[floatin]\n"
                        "is copied to [result_float].";
    info->out_param_spec = "out_float3:float3";
    info->in_param_spec = "float3_in:float3";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    float3_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"float3_in");
    float3_in->set(0,0);
    float3_in->set(0,1);
    float3_in->set(0,2);

    result_float3 = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"out_float3");
    result_float3->set(0,0);
    result_float3->set(0,1);
    result_float3->set(0,2);
  }

  void run() {
    result_float3->set(float3_in->get(0),0);
    result_float3->set(float3_in->get(1),1);
    result_float3->set(float3_in->get(2),2);
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class vsx_float4_dummy : public vsx_module {
  // in
  vsx_module_param_float4* float4_in;
  // out
  vsx_module_param_float4* result_float4;
  // internal
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;dummies;float4_dummy";
    info->description = "This is only to gather connections\n"
                        " - inside a macro for instance if \n"
                        "you have many components which need\n"
                        "to read the same value but you don't\n"
                        "want 10 aliases going out.\n[floatin]\n"
                        "is copied to [result_float].";
    info->out_param_spec = "out_float4:float4";
    info->in_param_spec = "float4_in:float4";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    float4_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"float4_in");
    float4_in->set(0,0);
    float4_in->set(0,1);
    float4_in->set(0,2);
    float4_in->set(0,3);

    result_float4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"out_float4");
    result_float4->set(0,0);
    result_float4->set(0,1);
    result_float4->set(0,2);
    result_float4->set(0,3);
  }

  void run() {
    result_float4->set(float4_in->get(0),0);
    result_float4->set(float4_in->get(1),1);
    result_float4->set(float4_in->get(2),2);
    result_float4->set(float4_in->get(3),3);
  }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class vsx_quaternion_dummy : public vsx_module {
  // in
  vsx_module_param_quaternion* quaternion_in;
  // out
  vsx_module_param_quaternion* result_quaternion;
  // internal
  public:
    void module_info(vsx_module_info* info)
    {
      info->identifier = "maths;dummies;quaternion_dummy";
      info->description = "This is only to gather connections\n"
          " - inside a macro for instance if \n"
          "you have many components which need\n"
          "to read the same value but you don't\n"
          "want 10 aliases going out.\n[quatin]\n"
          "is copied to [result_quat].";
      info->out_param_spec = "out_quat:quaternion";
      info->in_param_spec = "quat_in:quaternion";
      info->component_class = "parameters";
    }

    void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
    {
      loading_done = true;
      quaternion_in = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"quat_in");
      quaternion_in->set(0.0f,0);
      quaternion_in->set(0.0f,1);
      quaternion_in->set(0.0f,2);
      quaternion_in->set(1.0f,3);

      result_quaternion = (vsx_module_param_quaternion*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"out_quat");
      result_quaternion->set(0.0f,0);
      result_quaternion->set(0.0f,1);
      result_quaternion->set(0.0f,2);
      result_quaternion->set(1.0f,3);
    }

    void run() {
      result_quaternion->set(quaternion_in->get(0),0);
      result_quaternion->set(quaternion_in->get(1),1);
      result_quaternion->set(quaternion_in->get(2),2);
      result_quaternion->set(quaternion_in->get(3),3);
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class vsx_float3to3float : public vsx_module {
  // in
  vsx_module_param_float3* float3_in;
  // out
  vsx_module_param_float* a;
  vsx_module_param_float* b;
  vsx_module_param_float* c;
  // internal
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;converters;float3to3float";
    info->description = "This is only to gather connections - inside a macro for instance if you have many components which need to read the same value but you don't want 10 aliases going out.\n[floatin] is copied to [result_float].";
    info->out_param_spec = "a:float,b:float,c:float";
    info->in_param_spec = "float3_in:float3";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    float3_in = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"float3_in");
    a = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"a");
    a->set(0.0f);
    b = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"b");
    b->set(0.0f);
    c = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"c");
    c->set(0.0f);
  }

  void run() {
    a->set(float3_in->get(0));
    b->set(float3_in->get(1));
    c->set(float3_in->get(2));
  }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


class vsx_module_4f_hsv_to_rgb_f4 : public vsx_module {
  vsx_color color;
  // in
  vsx_module_param_float* floata;
  vsx_module_param_float* floatb;
  vsx_module_param_float* floatc;
  vsx_module_param_float* floatd;
  // out
  vsx_module_param_float4* result4;
  // internal
public:

void module_info(vsx_module_info* info)
{
  info->identifier = "maths;color;converters;4f_hsv_to_f4_rgb";

  info->description = "\
This module converts color provided in\n\
HSV colorspace into RGB along with alpha.\n\
It takes 4 floats,\n\
  [H]ue (color - red/green etc.),\n\
  [S]aturation (vibrancy)\n\
  [V]alue (brightness)\n\
  [A]lpha\n\
and converts to an RedGreenBlueAlpha float4";
  info->out_param_spec = "result_float4:float4";
  info->in_param_spec =
"\
h:float,\
s:float,\
v:float,\
a:float\
";

  info->component_class = "parameters";
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  floata = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"h");
  floatb = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"s");
  floatc = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"v");
  floatd = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"a");
  floata->set(0);
  floatb->set(1);
  floatc->set(1);
  floatd->set(1);

  result4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
  result4->set(0,0);
  result4->set(0,1);
  result4->set(0,2);
  result4->set(0,3);
//--------------------------------------------------------------------------------------------------
}

void run() {
  //float a, b, c;
  color.hsv((float)fmod(fabs(floata->get()),1.0),floatb->get(),floatc->get());
  result4->set(color.r,0);
  result4->set(color.g,1);
  result4->set(color.b,2);
  result4->set(floatd->get(),3);
}
};



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


class vsx_module_f4_hsl_to_rgb_f4 : public vsx_module {
  vsx_color color;
  // in
  vsx_module_param_float4* hsl;
  // out
  vsx_module_param_float4* result4;
  // internal

  float Hue_2_RGB(float v1, float v2, float vH)
  {
    vH = (float)fmod(vH + 1.0f,1.0f);
  if ( vH < 0.0f ) vH += 1.0f;
  if( vH > 1.0f ) vH -= 1.0f;
    if ( ( 6.0f * vH ) < 1.0f ) return ( v1 + ( v2 - v1 ) * 6.0f * vH );
    if ( ( 2.0f * vH ) < 1.0f ) return ( v2 );
    if ( ( 3.0f * vH ) < 2.0f ) return ( v1 + ( v2 - v1 ) * ( ( 2.0f / 3.0f ) - vH ) * 6.0f );
    return ( v1 );
  }


public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;color;converters;f4_hsl_to_f4_rgb";
    info->description = "\
This module converts color provided in\n\
HSL colorspace into RGB along with alpha.\n\
It takes a float4,\n\
  [H]ue (color - red/green etc.),\n\
  [S]aturation (vibrancy)\n\
  [L]ightness (1 = white)\n\
  [A]lpha\n\
and converts to an RedGreenBlueAlpha float4";
    info->out_param_spec = "result_float4:float4";
    info->in_param_spec = "hsl:float4";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    hsl = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"hsl");
    result4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
    result4->set(0,0);
    result4->set(0,1);
    result4->set(0,2);
    result4->set(0,3);
  }



  void run() {
    float H = hsl->get(0);
    float S = hsl->get(1);
    float L = hsl->get(2);
    if ( S == 0 )                       //HSL values = 0 � 1
    {
       color.r = L;                      //RGB results = 0 � 255
       color.g = L;
       color.b = L;
    }
    else
    {
       float var_2;
       if ( L < 0.5 ) var_2 = L * ( 1.0f + S );
       else           var_2 = ( L + S ) - ( S * L );

       float var_1 = 2.0f*L - var_2;

       color.r = Hue_2_RGB( var_1, var_2, H + ( 1.0f / 3.0f ) );
       color.g = Hue_2_RGB( var_1, var_2, H );
       color.b = Hue_2_RGB( var_1, var_2, H - ( 1.0f / 3.0f ) );
    }

    result4->set(color.r,0);
    result4->set(color.g,1);
    result4->set(color.b,2);
    result4->set(hsl->get(3),3);
  }
};


//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------


vsx_module* MOD_CM(unsigned long module) {
  switch (module) {
    case 0:  return (vsx_module*)(new module_3float_to_float3);
    case 1:  return (vsx_module*)(new module_4float_to_float4);
    case 2:  return (vsx_module*)(new vsx_module_4f_hsv_to_rgb_f4);
    case 3:  return (vsx_module*)(new vsx_float_dummy);
    case 4:  return (vsx_module*)(new vsx_float3_dummy);
    case 5:  return (vsx_module*)(new vsx_float_array_pick);
    case 6:  return (vsx_module*)(new vsx_arith_add);
    case 7:  return (vsx_module*)(new vsx_arith_sub);
    case 8:  return (vsx_module*)(new vsx_arith_mult);
    case 9:  return (vsx_module*)(new vsx_arith_div);
    case 10: return (vsx_module*)(new vsx_arith_min);
    case 11: return (vsx_module*)(new vsx_arith_max);
    case 12: return (vsx_module*)(new vsx_arith_pow);
    case 13: return (vsx_module*)(new vsx_arith_round);
    case 14: return (vsx_module*)(new vsx_arith_floor);
    case 15: return (vsx_module*)(new vsx_arith_ceil);
    case 16: return (vsx_module*)(new vsx_float_accumulator);
    case 17: return (vsx_module*)(new vsx_float3_accumulator);
    case 18: return (vsx_module*)(new vsx_float4_accumulator);
    case 19: return (vsx_module*)(new module_vector_add);
    case 20: return (vsx_module*)(new module_vector_add_float);
    case 21: return (vsx_module*)(new module_vector_mul_float);
    case 22: return (vsx_module*)(new module_float_to_float3);
    case 23: return (vsx_module*)(new vsx_float_abs);
    case 24: return (vsx_module*)(new vsx_float_sin);
    case 25: return (vsx_module*)(new vsx_float_cos);
    case 26: return (vsx_module*)(new vsx_bool_and);
    case 27: return (vsx_module*)(new vsx_bool_or);
    case 28: return (vsx_module*)(new vsx_bool_nor);
    case 29: return (vsx_module*)(new vsx_bool_xor);
    case 30: return (vsx_module*)(new vsx_bool_not);
    case 31: return (vsx_module*)(new module_vec4_mul_float);
    case 32: return (vsx_module*)(new vsx_bool_nand);
    case 33: return (vsx_module*)(new module_float4_add);
    case 34: return (vsx_module*)(new vsx_float_array_average);
    case 35: return (vsx_module*)(new vsx_arith_mod);
    case 36: return (vsx_module*)(new vsx_module_f4_hsl_to_rgb_f4);
    case 37: return (vsx_module*)(new vsx_float3to3float);
    case 38: return (vsx_module*)(new vsx_float_limit);
    case 39: return (vsx_module*)(new module_vector_4float_to_quaternion);
    case 40: return (vsx_module*)(new vsx_float_smooth);
    case 41: return (vsx_module*)(new module_quaternion_rotation_accumulator_2d);
    case 42: return (vsx_module*)(new module_vector_normalize);
    case 43: return (vsx_module*)(new module_vector_cross_product);
    case 44: return (vsx_module*)(new module_vector_dot_product);
    case 45: return (vsx_module*)(new module_vector_from_points);
    case 46: return (vsx_module*)(new module_quaternion_slerp_2);
    case 47: return (vsx_module*)(new module_quaternion_mul);
    case 48: return (vsx_module*)(new vsx_float_accumulator_limits);
    case 49: return (vsx_module*)(new module_quaternion_slerp_3);
    case 50: return (vsx_module*)(new vsx_float4_interpolate);
    case 51: return (vsx_module*)(new vsx_float_interpolate);
    case 52: return (vsx_module*)(new vsx_float_acos);
    case 53: return (vsx_module*)(new vsx_quaternion_dummy);
    case 54: return (vsx_module*)(new module_vector_quaternion_to_4float);
    case 55: return (vsx_module*)(new vsx_float3_interpolate);
    case 56: return (vsx_module*)(new vsx_float4_dummy);
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (module_3float_to_float3*)m; break;
    case 1: delete (module_4float_to_float4*)m; break;
    case 2: delete (vsx_module_4f_hsv_to_rgb_f4*)m; break;
    case 3: delete (vsx_float_dummy*)m; break;
    case 4: delete (vsx_float3_dummy*)m; break;
    case 5: delete (vsx_float_array_pick*)m; break;
    case 6: delete (vsx_arith_add*)m; break;
    case 7: delete (vsx_arith_sub*)m; break;
    case 8: delete (vsx_arith_mult*)m; break;
    case 9: delete (vsx_arith_div*)m; break;
    case 10: delete (vsx_arith_min*)m; break;
    case 11: delete (vsx_arith_max*)m; break;
    case 12: delete (vsx_arith_pow*)m; break;
    case 13: delete (vsx_arith_round*)m; break;
    case 14: delete (vsx_arith_floor*)m; break;
    case 15: delete (vsx_arith_ceil*)m; break;
    case 16: delete (vsx_float_accumulator*)m; break;
    case 17: delete (vsx_float3_accumulator*)m; break;
    case 18: delete (vsx_float4_accumulator*)m; break;
    case 19: delete (module_vector_add*)m; break;
    case 20: delete (module_vector_add_float*)m; break;
    case 21: delete (module_vector_mul_float*)m; break;
    case 22: delete (module_float_to_float3*)m; break;
    case 23: delete (vsx_float_abs*)m; break;
    case 24: delete (vsx_float_sin*)m; break;
    case 25: delete (vsx_float_cos*)m; break;
    case 26: delete (vsx_bool_and*)m; break;
    case 27: delete (vsx_bool_or*)m; break;
    case 28: delete (vsx_bool_nor*)m; break;
    case 29: delete (vsx_bool_xor*)m; break;
    case 30: delete (vsx_bool_not*)m; break;
    case 31: delete (module_vec4_mul_float*)m; break;
    case 32: delete (vsx_bool_nand*)m; break;
    case 33: delete (module_float4_add*)m; break;
    case 34: delete (vsx_float_array_average*)m; break;
    case 35: delete (vsx_arith_mod*)m; break;
    case 36: delete (vsx_module_f4_hsl_to_rgb_f4*)m; break;
    case 37: delete (vsx_float3to3float*)m; break;
    case 38: delete (vsx_float_limit*)m; break;
    case 39: delete (module_vector_4float_to_quaternion*)m; break;
    case 40: delete (vsx_float_smooth*)m; break;
    case 41: delete (module_quaternion_rotation_accumulator_2d*)m; break;
    case 42: delete (module_vector_normalize*)m; break;
    case 43: delete (module_vector_cross_product*)m; break;
    case 44: delete (module_vector_dot_product*)m; break;
    case 45: delete (module_vector_from_points*)m; break;
    case 46: delete (module_quaternion_slerp_2*)m; break;
    case 47: delete (module_quaternion_mul*)m; break;
    case 48: delete (vsx_float_accumulator_limits*)m; break;
    case 49: delete (module_quaternion_slerp_3*)m; break;
    case 50: delete (vsx_float4_interpolate*)m; break;
    case 51: delete (vsx_float_interpolate*)m; break;
    case 52: delete (vsx_float_acos*)m; break;
    case 53: delete (vsx_quaternion_dummy*)m; break;
    case 54: delete (module_vector_quaternion_to_4float*)m; break;
    case 55: delete (vsx_float3_interpolate*)m; break;
    case 56: delete (vsx_float4_dummy*)m; break;
  }
}

unsigned long MOD_NM() {
  return 57;
}


//module_vector_float4_to_4float