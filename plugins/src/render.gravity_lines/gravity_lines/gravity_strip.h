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
      vsx_mesh       &mesh,
      vsx_face*      &fs_d,
      vsx_vector*    &vs_d,
      vsx_vector*    &ns_d,
      vsx_tex_coord* &ts_d,
      vsx_matrix*    modelview_matrix,
      vsx_vector*    upvector,
      int &generated_vertices,
      int &generated_faces
  );
};


#endif /*GRAVITY_STRIP_H_*/
