#ifndef VSX_TEXTURE_TRANSFORM_HELPER_H
#define VSX_TEXTURE_TRANSFORM_HELPER_H

#include <vsx_gl_state.h>
#include <texture/vsx_texture.h>
#include <vsx_texture_transform.h>

namespace vsx_texture_transform_helper
{
  void begin_transform(vsx_texture* texture)
  {
    if (!texture->get_transform())
      return;

    vsx_gl_state::get_instance()->matrix_mode( VSX_GL_TEXTURE_MATRIX );
    vsx_gl_state::get_instance()->matrix_push();

    texture->get_transform()->transform();
  }

  void end_transform(vsx_texture* texture)
  {
    if (!texture->get_transform())
      return;

    vsx_gl_state::get_instance()->matrix_mode( VSX_GL_TEXTURE_MATRIX );
    vsx_gl_state::get_instance()->matrix_pop();
  }
}

#endif
