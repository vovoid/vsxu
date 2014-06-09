#ifndef GRAVITY_STRIP_H_
#define GRAVITY_STRIP_H_

#include "gravity_lines.h"


class gravity_strip : public gravity_lines
{
public:
  float length;
  bool thin_edges;

  void init_strip();
  void render();
  void generate_mesh(
      vsx_mesh<>       &mesh,
      vsx_face3*      &fs_d,
      vsx_vector3<>*    &vs_d,
      vsx_vector3<>*    &ns_d,
      vsx_tex_coord2f* &ts_d,
      vsx_matrix<float>*    modelview_matrix,
      vsx_vector3<>*    upvector,
      int &generated_vertices,
      int &generated_faces
  );
};


#endif /*GRAVITY_STRIP_H_*/
