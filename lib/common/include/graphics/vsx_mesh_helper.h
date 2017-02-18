#ifndef VSX_MESH_HELPER_H
#define VSX_MESH_HELPER_H
#include <string/vsx_string_helper.h>
#include <graphics/vsx_mesh.h>

namespace vsx_mesh_helper
{

  template<typename T>
  void calculate_tangent_space_into_vertex_colors(vsx_mesh<T>* mesh)
  {
    mesh->data->vertex_colors.allocate( mesh->data->vertices.size() );
    mesh->data->vertex_colors.memory_clear();

    vsx_quaternion<float>* vec_d = (vsx_quaternion<float>*)mesh->data->vertex_colors.get_pointer();

    for (unsigned long a = 0; a < mesh->data->faces.size(); a++)
    {
      long i1 = mesh->data->faces[a].a;
      long i2 = mesh->data->faces[a].b;
      long i3 = mesh->data->faces[a].c;

      const vsx_vector3<float>& v1 = mesh->data->vertices[i1];
      const vsx_vector3<float>& v2 = mesh->data->vertices[i2];
      const vsx_vector3<float>& v3 = mesh->data->vertices[i3];

      const vsx_tex_coord2f& w1 = mesh->data->vertex_tex_coords[i1];
      const vsx_tex_coord2f& w2 = mesh->data->vertex_tex_coords[i2];
      const vsx_tex_coord2f& w3 = mesh->data->vertex_tex_coords[i3];

      float x1 = v2.x - v1.x;
      float x2 = v3.x - v1.x;
      float y1 = v2.y - v1.y;
      float y2 = v3.y - v1.y;
      float z1 = v2.z - v1.z;
      float z2 = v3.z - v1.z;

      float s1 = w2.s - w1.s;
      float s2 = w3.s - w1.s;
      float t1 = w2.t - w1.t;
      float t2 = w3.t - w1.t;

      float r = 1.0f / (s1 * t2 - s2 * t1);
      vsx_quaternion<float> sdir(
            (t2 * x1 - t1 * x2) * r,
            (t2 * y1 - t1 * y2) * r,
            (t2 * z1 - t1 * z2) * r,
            1.0);

      vec_d[i1] += sdir;
      vec_d[i2] += sdir;
      vec_d[i3] += sdir;
    }

    for (unsigned long a = 0; a < mesh->data->vertices.size(); a++)
    {
        vsx_vector3<float>& n = mesh->data->vertex_normals[a];
        vsx_quaternion<float>& t = vec_d[a];

        // Gram-Schmidt orthogonalize
        //vec_d[a] = (t - n * t.dot_product(&n) );
        vec_d[a] = (t - n * t.dot_product(&n) );
        vec_d[a].normalize();

        // Calculate handedness
        //tangent[a].w = (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
    }
  }

  template<typename T>
  vsx_string<>* mesh_to_obj(vsx_mesh<T>* mesh)
  {
    vsx_string<>* result_p = new vsx_string<>;
    vsx_string<>& result = *result_p;

    result += "# VSXu OBJ Generator\n";
    result += "# www.vsxu.com\n";
    result += "o mesh\n";

    for (size_t i = 0; i < mesh->data->vertices.size(); i++)
    {
      vsx_vector3<T>& v = mesh->data->vertices[i];
      result +=
        "v "+
        vsx_string_helper::f2s( v.x ) +" "+
        vsx_string_helper::f2s( v.y ) +" "+
        vsx_string_helper::f2s( v.z ) +"\n"
      ;
    }

    for (size_t i = 0; i < mesh->data->vertex_tex_coords.size(); i++)
    {
      vsx_tex_coord2f& vt = mesh->data->vertex_tex_coords[i];
      result +=
        "vt "+
        vsx_string_helper::f2s( vt.s ) +" "+
        vsx_string_helper::f2s( vt.t ) +"\n"
      ;
    }

    for (size_t i = 0; i < mesh->data->vertex_normals.size(); i++)
    {
      vsx_vector3<T>& vn = mesh->data->vertex_normals[i];
      result +=
        "vn "+
        vsx_string_helper::f2s( vn.x ) +" "+
        vsx_string_helper::f2s( vn.y ) +" "+
        vsx_string_helper::f2s( vn.z ) +"\n"
      ;
    }

    for (size_t i = 0; i < mesh->data->faces.size(); i++)
    {
      vsx_face3& f = mesh->data->faces[i];

      result +=
        "f "+
        vsx_string_helper::i2s( f.a+1 ) +"/"+vsx_string_helper::i2s( f.a+1 )+"/"+vsx_string_helper::i2s( f.a+1 )+" "+
        vsx_string_helper::i2s( f.b+1 ) +"/"+vsx_string_helper::i2s( f.b+1 )+"/"+vsx_string_helper::i2s( f.b+1 )+" "+
        vsx_string_helper::i2s( f.c+1 ) +"/"+vsx_string_helper::i2s( f.c+1 )+"/"+vsx_string_helper::i2s( f.c+1 )+"\n"
      ;
    }
    return result_p;
  }

}

#endif
