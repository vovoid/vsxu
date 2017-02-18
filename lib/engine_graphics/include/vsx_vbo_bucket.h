#pragma once

#include <vsx_gl_global.h>
#include <container/vsx_ma_vector.h>
#include <math/vector/vsx_vector2.h>
#include <math/vector/vsx_vector3.h>
#include <math/vector/vsx_vector4.h>
#include <graphics/face/vsx_face1.h>
#include <graphics/face/vsx_face2.h>
#include <graphics/face/vsx_face3.h>
#include <graphics/face/vsx_face4.h>
#include <color/vsx_color.h>
#include <graphics/vsx_texcoord.h>


/*
  VSXu VBO Bucket helper class

  This class handles changing underlying data (size etc) automatically.

  Because of underlying use of vsx_array, it won't reallocate memory unnecesarily.

 1. Triangles

   vsx_vbo_bucket my_triangle_bucket;
   (fill out vertices)
   (fill out faces)

 2. Points, Point sprites

   vsx_vbo_bucket<GLuint, 1, GL_POINTS> my_point_bucket;

*/

template
<
  // Face storage type. Must match sizeof in A
  typename Tf = vsx_face3,

  // What OpenGL native draw type to use
  GLuint T_type = GL_TRIANGLES,

  // VBO upload strategy - GL_STATIC_DRAW || GL_DYNAMIC_DRAW || GL_STREAM_DRAW
  GLuint T_vbo_usage = GL_STATIC_DRAW_ARB,

  // Vertex data type
  typename Tv = vsx_vector3<>,

  // TexCoord data type
  typename Tuv = vsx_tex_coord2f
>
class vsx_vbo_bucket
{
public:
  // 1. compute data into these buckets
  vsx_ma_vector< Tv >     vertices;
  vsx_ma_vector< vsx_vector3<> >     vertex_normals;
  vsx_ma_vector< vsx_color<> >      vertex_colors;
  vsx_ma_vector< Tuv >  vertex_tex_coords;
  vsx_ma_vector< Tf >              faces;

  // 2. reset buffers
  inline void reset_used()
  {
    vertices.reset_used();
    vertex_normals.reset_used();
    vertex_colors.reset_used();
    vertex_tex_coords.reset_used();
    faces.reset_used();
  }


  // 2. call any of the invalidation functions after changing the data
  inline void invalidate_vertices()
  {
    i_invalidate_vertices();
  }

  inline void invalidate_normals()
  {
    i_invalidate_normals();
  }

  inline void invalidate_texcoords()
  {
    i_invalidate_texcoords();
  }

  inline void invalidate_colors()
  {
    i_invalidate_colors();
  }

  inline void invalidate()
  {
    if (vertices.size())
      i_invalidate_vertices();
    if (vertex_normals.size())
      i_invalidate_normals();
    if (vertex_tex_coords.size())
      i_invalidate_texcoords();
    if (vertex_colors.size())
      i_invalidate_colors();
  }

  // 3. fire off the upload to the GPU
  inline void update()
  {
    update_inner();
  }

  // 4. call the draw command
  // n=0 means draw all elements
  inline void output( size_t n = 0 )
  {
    // sanity checks
    if ( !faces.get_used() ) return;

    // maintain the vbo type
    if ( !maintain_vbo_type(T_vbo_usage) ) return;

    // enable arrays
    if ( !enable_client_arrays() ) return;

    // draw
    perform_draw(n);

    // disable arrays
    disable_client_arrays();
  }

  void unload()
  {
    destroy_vbo();
  }

  vsx_vbo_bucket()
  {
    m_flags = 0;

    // offset values
    offset_normals = 0;
    offset_vertices = 0;
    offset_texcoords = 0;
    offset_vertex_colors = 0;
    // vbo handles
    vbo_id_vertex_normals_texcoords = 0;
    vbo_id_draw_indices = 0;
    // current - state - used to see if anything has changed
    current_vbo_draw_type = 0;
    current_num_vertices = 0;
    current_num_colors = 0;
    current_num_faces = 0;

    // vbo handles
    vbo_id_vertex_normals_texcoords = 0;
    vbo_id_draw_indices = 0;

    current_vbo_draw_type = 0;

    invalidation_flags = 0;
  }

  ~vsx_vbo_bucket()
  {
    destroy_vbo();
  }


private:

  #if PLATFORM_BITS == 32
    #define VBO_VBOB_TYPE int
  #else
    #define VBO_VBOB_TYPE int64_t
  #endif

  // vbo index offsets
  VBO_VBOB_TYPE offset_normals;
  VBO_VBOB_TYPE offset_vertices;
  VBO_VBOB_TYPE offset_texcoords;
  VBO_VBOB_TYPE offset_vertex_colors;

  // flag data
  #define VSX_VBOB_VERTICES 1
  #define VSX_VBOB_NORMALS 2
  #define VSX_VBOB_COLORS 4
  #define VSX_VBOB_TEXCOORDS 8

  // invalidation flags
  unsigned char invalidation_flags;

  // internal flags what buffers are enabled
  unsigned char m_flags;

  // vbo handles
  GLuint vbo_id_vertex_normals_texcoords;
  GLuint vbo_id_draw_indices;

  // current - state - used to see if anything has changed
  GLuint current_vbo_draw_type;
  size_t current_num_vertices;
  size_t current_num_colors;
  size_t current_num_faces;

  inline void i_invalidate_vertices()
  {
    invalidation_flags |= VSX_VBOB_VERTICES;
  }

  inline void i_invalidate_normals()
  {
    invalidation_flags |= VSX_VBOB_NORMALS;
  }

  inline void i_invalidate_texcoords()
  {
    invalidation_flags |= VSX_VBOB_TEXCOORDS;
  }

  inline void i_invalidate_colors()
  {
    invalidation_flags |= VSX_VBOB_COLORS;
  }

  bool init_vbo(GLuint draw_type = GL_DYNAMIC_DRAW_ARB)
  {
    if (vbo_id_vertex_normals_texcoords) {
      return true;
    }
    current_vbo_draw_type = draw_type;
    offset_normals = 0;
    offset_vertices = 0;
    offset_texcoords = 0;
    offset_vertex_colors = 0;
    GLintptr offset = 0;

    //-----------------------------------------------------------------------
    // generate the buffers
    if (vbo_id_vertex_normals_texcoords == 0)
    {
      glGenBuffersARB
      (
        1,
        &vbo_id_vertex_normals_texcoords
      );
    }
    // bind the vertex, normals buffer for use
    glBindBufferARB
    (
      GL_ARRAY_BUFFER_ARB,
      vbo_id_vertex_normals_texcoords
    );

    //-----------------------------------------------------------------------
    // allocate the buffer
    glBufferDataARB(
      GL_ARRAY_BUFFER_ARB,
      vertex_normals.get_sizeof()
      +
      vertices.get_sizeof()
      +
      vertex_tex_coords.get_sizeof()
      +
      vertex_colors.get_sizeof()
      ,
      0,
      draw_type//GL_STATIC_DRAW_ARB // only static draw
    );

    //-----------------------------------------------------------------------
    // inject the different arrays
    // 1: vertex normals ----------------------------------------------------
    if ( vertex_normals.size() )
    {
      offset_normals = offset;
      glBufferSubDataARB
      (
        GL_ARRAY_BUFFER_ARB,
        offset,
        vertex_normals.get_sizeof(),
        vertex_normals.get_pointer()
      );
      offset += vertex_normals.get_sizeof();
    }

    // 2: texture coordinates -----------------------------------------------
    if ( vertex_tex_coords.size() )
    {
      offset_texcoords = offset;
      glBufferSubDataARB
      (
        GL_ARRAY_BUFFER_ARB,
        offset,
        vertex_tex_coords.get_sizeof(),
        vertex_tex_coords.get_pointer()
      );
      offset += vertex_tex_coords.get_sizeof();
    }

    // 3: optional: vertex color coordinates -----------------------------------------------
    if ( vertex_colors.size() )
    {
      offset_vertex_colors = offset;
      glBufferSubDataARB
      (
        GL_ARRAY_BUFFER_ARB,
        offset,
        vertex_colors.get_sizeof(),
        vertex_colors.get_pointer()
      );
      offset += vertex_colors.get_sizeof();
    }
    current_num_colors = vertex_colors.size();

    // 4: vertices ----------------------------------------------------------
    offset_vertices = offset;
    glBufferSubDataARB
    (
      GL_ARRAY_BUFFER_ARB,
      offset,
      vertices.get_sizeof(),
      vertices.get_pointer()
    );
    offset += vertices.get_sizeof();
    current_num_vertices = vertices.size();

    //-----------------------------------------------------------------------

    // unbind the array buffer
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

    // 5: Elements ----------------------------------------------------------
    if (vbo_id_draw_indices == 0)
    {
      glGenBuffersARB(1, &vbo_id_draw_indices);
    }
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vbo_id_draw_indices);
    // upload data to video card
    glBufferDataARB(
      GL_ELEMENT_ARRAY_BUFFER_ARB,
      faces.get_sizeof(),
      faces.get_pointer(),
      T_vbo_usage
    );

    current_num_faces = faces.size();
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    return true;
  }

  inline void destroy_vbo()
  {
    req(vbo_id_vertex_normals_texcoords);

    glDeleteBuffersARB(1, &vbo_id_draw_indices);
    glDeleteBuffersARB(1, &vbo_id_vertex_normals_texcoords);

    vbo_id_vertex_normals_texcoords = 0;
    vbo_id_draw_indices = 0;
  }


  inline bool check_if_need_to_reinit_vbo(GLuint draw_type)
  {
    if (!vbo_id_vertex_normals_texcoords)
      return true;

    if (current_num_vertices != vertices.size() )
      return true;

    if (current_num_colors != vertex_colors.size() )
      return true;

    if (current_num_faces != faces.size() )
      return true;

    if (current_vbo_draw_type != draw_type)
      return true;

    return false;
  }

  inline bool maintain_vbo_type(GLuint draw_type = GL_DYNAMIC_DRAW_ARB)
  {
    if (!check_if_need_to_reinit_vbo(draw_type)) return true;
    destroy_vbo();
    return init_vbo(draw_type);
  }

  inline bool enable_client_arrays()
  {
    // reset presence values
    m_flags = 0;

    // bind the vertex, normals buffer for use
    glBindBufferARB
    (
      GL_ARRAY_BUFFER_ARB,
      vbo_id_vertex_normals_texcoords
    );

    // vertex colors
    if ( vertex_colors.get_used() )
    {
      glColorPointer(4,GL_FLOAT,0,(GLvoid*)offset_vertex_colors);
      m_flags |= VSX_VBOB_COLORS;
    }

    // vertex normals
    if ( vertex_normals.get_used() )
    {
      glNormalPointer(GL_FLOAT,0,(GLvoid*)offset_normals);
      m_flags |= VSX_VBOB_NORMALS;
    }

    // tex coords
    if ( vertex_tex_coords.get_used() )
    {
      glTexCoordPointer((GLint)Tuv::arity(),GL_FLOAT,0,(GLvoid*)offset_texcoords);
      m_flags |= VSX_VBOB_TEXCOORDS;
    }

    // enable vertices
    glVertexPointer((GLint)(sizeof(Tv)/sizeof(float)),GL_FLOAT,0,(GLvoid*)offset_vertices);

    // bind the index array buffer buffer for use
    glBindBufferARB
    (
      GL_ELEMENT_ARRAY_BUFFER_ARB,
      vbo_id_draw_indices
    );

    glEnableClientState(GL_VERTEX_ARRAY);

    if (m_flags & VSX_VBOB_COLORS)
      glEnableClientState(GL_COLOR_ARRAY);

    if (m_flags & VSX_VBOB_NORMALS)
      glEnableClientState(GL_NORMAL_ARRAY);

    if (m_flags & VSX_VBOB_TEXCOORDS)
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // all went well
    return true;
  }

  inline void disable_client_arrays()
  {
    glDisableClientState(GL_VERTEX_ARRAY);

    if (m_flags & VSX_VBOB_COLORS)
    glDisableClientState(GL_COLOR_ARRAY);

    if (m_flags & VSX_VBOB_NORMALS)
    glDisableClientState(GL_NORMAL_ARRAY);

    if (m_flags & VSX_VBOB_TEXCOORDS)
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // unbind the VBO buffers
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  }

  inline void perform_draw( size_t num = 0 )
  {
    if (num == 0)
      num = faces.get_used() * Tf::arity();
    glDrawElements
    (
      T_type,
      (GLsizei)num,
      GL_UNSIGNED_INT,
      0
    );
  }

  inline void update_inner()
  {
    if (!faces.get_used())
    {
      return;
    }

    // handle invalidation instead
    if (!invalidation_flags) return;


    if (check_if_need_to_reinit_vbo(current_vbo_draw_type))
    {
      return;
    }


    // bind the vertex, normals buffer for use
    glBindBufferARB
    (
      GL_ARRAY_BUFFER_ARB,
      vbo_id_vertex_normals_texcoords
    );

    // if buffer type is "DYNAMIC_DRAW", upload new data
    if (
        invalidation_flags & VSX_VBOB_NORMALS
        &&
        vertex_normals.get_used()
        )
    {
      glBufferSubDataARB
      (
        GL_ARRAY_BUFFER_ARB,
        0,
        vertex_normals.get_sizeof(),
        vertex_normals.get_pointer()
      );
    }

    if (
        invalidation_flags & VSX_VBOB_TEXCOORDS
        &&
        vertex_tex_coords.get_used()
        )
    {
      glBufferSubDataARB
      (
        GL_ARRAY_BUFFER_ARB,
        offset_texcoords,
        vertex_tex_coords.get_sizeof(),
        vertex_tex_coords.get_pointer()
      );
    }

    if (
        invalidation_flags & VSX_VBOB_COLORS
        &&
        vertex_colors.get_used()
        )
    {
      glBufferSubDataARB
      (
        GL_ARRAY_BUFFER_ARB,
        offset_vertex_colors,
        vertex_colors.get_sizeof(),
        vertex_colors.get_pointer()
      );
    }

    if ( invalidation_flags & VSX_VBOB_VERTICES )
    {
      glBufferSubDataARB
      (
        GL_ARRAY_BUFFER_ARB,
        offset_vertices,
        vertices.get_sizeof(),
        vertices.get_pointer()
      );
    }

    // unbind the VBO buffers
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

    invalidation_flags = 0;
  }
}; // end VBO Bucket

