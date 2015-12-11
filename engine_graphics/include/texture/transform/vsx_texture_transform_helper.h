#pragma once

#include <vsx_gl_state.h>
#include <texture/vsx_texture.h>
#include "vsx_texture_transform_base.h"
#include "vsx_texture_transform_rotate.h"
#include "vsx_texture_transform_translate.h"
#include "vsx_texture_transform_scale.h"

namespace vsx_texture_transform_helper
{

  template <class T = vsx_texture_gl>
  inline void begin_transform(vsx_texture<T>* texture)
  {
    if (!texture->get_transform())
      return;

    vsx_gl_state::get_instance()->matrix_mode( VSX_GL_TEXTURE_MATRIX );
    vsx_gl_state::get_instance()->matrix_push();

    texture->get_transform()->transform();
  }

  template <class T = vsx_texture_gl>
  inline void end_transform(vsx_texture<T>* texture)
  {
    if (!texture->get_transform())
      return;

    vsx_gl_state::get_instance()->matrix_mode( VSX_GL_TEXTURE_MATRIX );
    vsx_gl_state::get_instance()->matrix_pop();
  }
}
