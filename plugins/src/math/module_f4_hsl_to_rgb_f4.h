/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


class module_f4_hsl_to_rgb_f4 : public vsx_module
{
  // in
  vsx_module_param_float4* hsl;

  // out
  vsx_module_param_float4* result4;

  // internal
  vsx_color<> color;

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

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "maths;color;converters;f4_hsl_to_f4_rgb";

    info->description =
      "This module converts color provided in\n"
      "HSL colorspace into RGB along with alpha.\n"
      "It takes a float4,\n"
      "  [H]ue (color - red/green etc.),\n"
      "  [S]aturation (vibrancy)\n"
      "  [L]ightness (1 = white)\n"
      "  [A]lpha\n"
      "and converts to an RedGreenBlueAlpha float4"
    ;

    info->out_param_spec =
      "result_float4:float4";

    info->in_param_spec =
      "hsl:float4";

    info->component_class =
      "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    //--------------------------------------------------------------------------------------------------

    hsl = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"hsl");

    //--------------------------------------------------------------------------------------------------

    result4 = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"result_float4");
    result4->set(0,0);
    result4->set(0,1);
    result4->set(0,2);
    result4->set(0,3);
  }



  void run()
  {
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
