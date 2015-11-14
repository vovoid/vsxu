#ifndef VSX_TEXTURE_TRANSFORM_HELPER_H
#define VSX_TEXTURE_TRANSFORM_HELPER_H

#include <vsx_gl_state.h>
#include <texture/vsx_texture.h>
#include "vsx_texture_transform_base.h"
#include "vsx_texture_transform_rotate.h"
#include "vsx_texture_transform_translate.h"
#include "vsx_texture_transform_scale.h"

namespace vsx_texture_transform_helper
{
  inline void begin_transform(vsx_texture* texture)
  {
    if (!texture->get_transform())
      return;

    vsx_gl_state::get_instance()->matrix_mode( VSX_GL_TEXTURE_MATRIX );
    vsx_gl_state::get_instance()->matrix_push();

    texture->get_transform()->transform();
  }

  inline void end_transform(vsx_texture* texture)
  {
    if (!texture->get_transform())
      return;

    vsx_gl_state::get_instance()->matrix_mode( VSX_GL_TEXTURE_MATRIX );
    vsx_gl_state::get_instance()->matrix_pop();
  }
}

#endif
