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


// UGLY HACKS GALORE:
typedef struct {
    vsx_vector3<> pt[3];
} tri;

typedef struct {
    int npoly;
    tri *poly;
} obj;

/* Six equidistant points lying on the unit sphere */
#define XPLUS vsx_vector3<>( 1,  0,  0  ) /*  X */
#define XMIN  vsx_vector3<>( -1,  0,  0 ) /* -X */
#define YPLUS vsx_vector3<>(  0,  1,  0 ) /*  Y */
#define YMIN  vsx_vector3<>(  0, -1,  0 ) /* -Y */
#define ZPLUS vsx_vector3<>(  0,  0,  1 ) /*  Z */
#define ZMIN  vsx_vector3<>(  0,  0, -1 ) /* -Z */

tri octahedron[] = {
    { { XPLUS, ZPLUS, YPLUS } },
    { { YPLUS, ZPLUS, XMIN  } },
    { { XMIN , ZPLUS, YMIN  } },
    { { YMIN , ZPLUS, XPLUS } },
    { { XPLUS, YPLUS, ZMIN  } },
    { { YPLUS, XMIN , ZMIN  } },
    { { XMIN , YMIN , ZMIN  } },
    { { YMIN , XPLUS, ZMIN  } }
};

#undef XPLUS
#undef XMIN
#undef YPLUS
#undef YMIN
#undef ZPLUS
#undef ZMIN

/* A unit octahedron */
obj oct = {
    sizeof(octahedron) / sizeof(octahedron[0]),
    &octahedron[0]
};

// thanks to Jon Leech (code from 1989!) for the base for this module.
class module_mesh_sphere_octahedron : public vsx_module
{
public:

  // in
  vsx_module_param_float* subdivision_level;
  vsx_module_param_float* max_normalization_level;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  int l_param_updates;
  int current_subdivision_level;
  int current_max_normalization_level;


  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;solid;mesh_sphere_octahedron";

    info->description =
      "A sphere based on an octahedron\n"
      "subdivided [subdivision_level]\n"
      "number of times. You can set\n"
      "[max_normalization_leve] to stop\n"
      "normalizing the vertices making\n"
      "a more bulky surface."
    ;

    info->in_param_spec =
      "subdivision_level:float?min=1&nc=1,"
      "max_normalization_level:float?nc=1"
    ;

    info->out_param_spec =
      "mesh:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;

    subdivision_level = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"subdivision_level");
    subdivision_level->set(1.0f);

    max_normalization_level = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"max_normalization_level");
    max_normalization_level->set(0.0f);


    current_subdivision_level = 0;

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
  }

  bool init()
  {
    mesh = new vsx_mesh<>;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

  void run()
  {
    int new_subdivision_level = (int)subdivision_level->get();
    int new_max_normalization_level = (int)max_normalization_level->get();


    if
    (
      current_subdivision_level == new_subdivision_level
      &&
      current_max_normalization_level == new_max_normalization_level
    )
    return;
    current_subdivision_level = new_subdivision_level;
    current_max_normalization_level = new_max_normalization_level;

    mesh->data->reset();

    obj *old, *nnew;
    int  i;
    int  level;		/* Current subdivision level */
    int  maxlevel = new_subdivision_level;	/* Maximum subdivision level */
    int maxnormlevel = (int)max_normalization_level->get();


    old = &oct;

    /* Subdivide each starting triangle (maxlevel - 1) times */
    for (level = 1; level < maxlevel; level++)
    {
      /* Allocate a new object */
      nnew = (obj *)malloc(sizeof(obj));

      nnew->npoly = old->npoly * 4;

      /* Allocate 4* the number of points in the current approximation */
      nnew->poly  = (tri *)malloc(nnew->npoly * sizeof(tri));

      /* Subdivide each triangle in the old approximation and normalize
       *  the new points thus generated to lie on the surface of the unit
       *  sphere.
       * Each input triangle with vertices labelled [0,1,2] as shown
       *  below will be turned into four new triangles:
       *
       *			Make new points
       *			    a = (0+2)/2
       *			    b = (0+1)/2
       *			    c = (1+2)/2
       *	  1
       *	 /\		Normalize a, b, c
       *	/  \
       *    b/____\ c		Construct new triangles
       *    /\    /\		    [0,b,a]
       *   /	\  /  \		    [b,1,c]
       *  /____\/____\	    [a,b,c]
       * 0	  a	2	    [a,c,2]
       */
      for (i = 0; i < old->npoly; i++)
      {
        tri *oldt = &old->poly[i];
        tri *newt = &nnew->poly[i*4];
        vsx_vector3<> a, b, c;

        a.midpoint(oldt->pt[0], oldt->pt[2]);
        b.midpoint(oldt->pt[0], oldt->pt[1]);
        c.midpoint(oldt->pt[1], oldt->pt[2]);

        if (level < maxnormlevel)
        {
          a.normalize();
          b.normalize();
          c.normalize();
        }

        newt->pt[0] = oldt->pt[0];
        newt->pt[1] = b;
        newt->pt[2] = a;
        newt++;

        newt->pt[0] = b;
        newt->pt[1] = oldt->pt[1];
        newt->pt[2] = c;
        newt++;

        newt->pt[0] = a;
        newt->pt[1] = b;
        newt->pt[2] = c;
        newt++;

        newt->pt[0] = a;
        newt->pt[1] = c;
        newt->pt[2] = oldt->pt[2];
      }

      if (level > 1) {
          free(old->poly);
          free(old);
      }

      /* Continue subdividing new triangles */
      old = nnew;
    }

    for (i = 0; i < old->npoly; i++)
    {
      mesh->data->vertices.push_back(old->poly[i].pt[0]);
      mesh->data->vertices.push_back(old->poly[i].pt[1]);
      mesh->data->vertices.push_back(old->poly[i].pt[2]);
      vsx_vector3<> n;
      n = old->poly[i].pt[0];
      n.normalize();
      mesh->data->vertex_normals.push_back(n);
      n = old->poly[i].pt[1];
      n.normalize();
      mesh->data->vertex_normals.push_back(n);
      n = old->poly[i].pt[2];
      n.normalize();
      mesh->data->vertex_normals.push_back(n);

      vsx_face3 a;
      a.a = i*3+1;
      a.b = i*3;
      a.c = i*3+2;
      mesh->data->faces.push_back(a);
    }

    if (maxlevel > 1)
    free(old);

    mesh->timestamp++;
    result->set_p(mesh);
  }
};


