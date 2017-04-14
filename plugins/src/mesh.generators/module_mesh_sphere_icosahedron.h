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

#include <graphics/vsx_mesh_helper.h>

// Phi - the square root of 5 plus 1 divided by 2
#define sqr5 2.2361f
#define phi (1.0f + sqr5) * 0.5f

// Golden ratio - the ratio of edgelength to radius
#define ratio (float)sqrt( 10.0f + (2.0f * sqr5)) / (4.0f * phi)
#define ia (42.0f / ratio) * 0.5f
#define ib (42.0f / ratio) / (2.0f * phi)


#define v0  vsx_vector3<>(    0,   (ib),  -(ia)  )
#define v1  vsx_vector3<>(  (ib),   (ia),     0  )
#define v2  vsx_vector3<>( -(ib),   (ia),     0  )
#define v3  vsx_vector3<>(    0,   (ib),   (ia)  )
#define v4  vsx_vector3<>(    0,  -(ib),   (ia)  )
#define v5  vsx_vector3<>( -(ia),     0,   (ib)  )
#define v6  vsx_vector3<>(     0,  -(ib),  -(ia)  )
#define v7  vsx_vector3<>(    ia,     0,  -(ib)  )
#define v8  vsx_vector3<>(    ia,     0,     ib  )
#define v9  vsx_vector3<>( -(ia),     0,  -(ib)  )
#define v10 vsx_vector3<>(    ib,   -(ia),    0  )
#define v11 vsx_vector3<>( -(ib),   -(ia),    0  )


// thanks to Jon Leech (code from 1989!) for the base for this module.
class module_mesh_sphere_icosahedron : public vsx_module
{
public:
  // in
  vsx_module_param_float* subdivision_level;
  vsx_module_param_float* max_normalization_level;
  vsx_module_param_float* initial_ico_scale;
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
      "mesh;solid;mesh_sphere_icosahedron";

    info->description =
      "A sphere based on an icosahedron\n"
      "subdivided [subdivision_level]\n"
      "number of times. You can set\n"
      "[max_normalization_leve] to stop\n"
      "normalizing the vertices making\n"
      "a more bulky surface."
    ;

    info->in_param_spec =
      "subdivision_level:float?min=1&nc=1,"
      "max_normalization_level:float?nc=1,"
      "initial_ico_scale:float?nc=1"
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

    initial_ico_scale = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"initial_ico_scale");
    initial_ico_scale->set(1.0f);

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
    if (!param_updates) return;
    param_updates = 0;
    int new_subdivision_level = (int)subdivision_level->get();
    int new_max_normalization_level = (int)max_normalization_level->get();

    current_subdivision_level = new_subdivision_level;
    current_max_normalization_level = new_max_normalization_level;

    mesh->data->reset();

    tri icosahedron[] =
    {
      { { v0, v1, v2 } },
      { { v3, v2, v1 } },
      { { v3, v4, v5 } },
      { { v3, v8, v4 } },
      { { v0, v6, v7 } },
      { { v0, v9, v6 } },
      { { v4, v10, v11 } },
      { { v6, v11, v10 } },
      { { v2, v5, v9 } },
      { { v11, v9, v5 } },
      { { v1, v7, v8 } },
      { { v10, v8, v7 } },
      { { v3, v5, v2 } },
      { { v3, v1, v8 } },
      { { v0, v2, v9 } },
      { { v0, v7, v1 } },
      { { v6, v9, v11 } },
      { { v6, v10, v7 } },
      { { v4, v11, v5 } },
      { { v4, v8, v10 } }
    };


    /* A unit icosahedron */
    obj ico = {
        sizeof(icosahedron) / sizeof(icosahedron[0]),
        &icosahedron[0]
    };

    obj *old, *nnew;
    int  i;
    int  level;		/* Current subdivision level */
    int  maxlevel = new_subdivision_level;	/* Maximum subdivision level */
    int maxnormlevel = (int)max_normalization_level->get();

    float iscale = initial_ico_scale->get();
    for (i = 0; i < ico.npoly; i++)
    {
      ico.poly[i].pt[0] *= iscale;
      ico.poly[i].pt[1] *= iscale;
      ico.poly[i].pt[2] *= iscale;
    }

    old = &ico;


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
        vsx_vector3<> xa, xb, xc;

        xa.midpoint(oldt->pt[0], oldt->pt[2]);
        xb.midpoint(oldt->pt[0], oldt->pt[1]);
        xc.midpoint(oldt->pt[1], oldt->pt[2]);

        if (level < maxnormlevel)
        {
          xa.normalize();
          xb.normalize();
          xc.normalize();
        }

        newt->pt[0] = oldt->pt[0];
        newt->pt[1] = xb;
        newt->pt[2] = xa;
        newt++;

        newt->pt[0] = xb;
        newt->pt[1] = oldt->pt[1];
        newt->pt[2] = xc;
        newt++;

        newt->pt[0] = xa;
        newt->pt[1] = xb;
        newt->pt[2] = xc;
        newt++;

        newt->pt[0] = xa;
        newt->pt[1] = xc;
        newt->pt[2] = oldt->pt[2];
      }

      if (level > 1) {
          free(old->poly);
          free(old);
      }

      /* Continue subdividing new triangles */
      old = nnew;
    }

    size_t vertex_index = 0;
    for (i = 0; i < old->npoly; i++)
    {
      vsx_vector3<> find = old->poly[i].pt[0];
      // try to find this vertex somewhere before
      int index_a = -1;
      for (size_t j = 0; j < mesh->data->vertices.size(); j++)
      {
        if (mesh->data->vertices[j] == find)
        {
          index_a = (int)j;
          break;
        }
      }
      if (index_a == -1)
      {
        mesh->data->vertices.push_back(old->poly[i].pt[0]);

        vsx_vector3<> n(0,0,0);
        mesh->data->vertex_normals.push_back(n);

        index_a = (int)vertex_index;
        vertex_index++;
      }


      find = old->poly[i].pt[1];
      // try to find this vertex somewhere before
      int index_b = -1;
      for (size_t j = 0; j < mesh->data->vertices.size(); j++)
      {
        if (mesh->data->vertices[j] == find)
        {
          index_b = (int)j;
          break;
        }
      }
      if (index_b == -1)
      {
        mesh->data->vertices.push_back(old->poly[i].pt[1]);

        vsx_vector3<> n(0,0,0);
        mesh->data->vertex_normals.push_back(n);

        index_b = (int)vertex_index;
        vertex_index++;
      }


      find = old->poly[i].pt[2];
      // try to find this vertex somewhere before
      int index_c = -1;
      for (size_t j = 0; j < mesh->data->vertices.size(); j++)
      {
        if (mesh->data->vertices[j] == find)
        {
          index_c = (int)j;
          break;
        }
      }
      if (index_c == -1)
      {
        mesh->data->vertices.push_back(old->poly[i].pt[2]);

        vsx_vector3<> n(0,0,0);
        mesh->data->vertex_normals.push_back(n);

        index_c = (int)vertex_index;
        vertex_index++;
      }


      vsx_face3 face;
      face.a = index_b;
      face.b = index_a;
      face.c = index_c;

      vsx_vector3<> an;
      an.assign_face_normal
      (
            &mesh->data->vertices[index_b],
            &mesh->data->vertices[index_a],
            &mesh->data->vertices[index_c]
      );
      mesh->data->vertex_normals[index_a] += an;
      mesh->data->vertex_normals[index_b] += an;
      mesh->data->vertex_normals[index_c] += an;

      mesh->data->faces.push_back(face);
    }

    // normalize all normals
    for (size_t i = 0; i < mesh->data->vertex_normals.size(); i++)
    {
      mesh->data->vertex_normals[i].normalize();
    }

    if (maxlevel > 1)
    {
      free(old->poly);
      free(old);
    }

    mesh->timestamp++;

//    vsx_string<>* r = vsx_mesh_helper::mesh_to_obj(mesh);
//    vsx_printf(L"%s\n", r->c_str());

    result->set_p(mesh);
  }
};


#undef sqr5
#undef phi

#undef ratio
#undef ia
#undef ib


#undef v0
#undef v1
#undef v2
#undef v3
#undef v4
#undef v5
#undef v6
#undef v7
#undef v8
#undef v9
#undef v10
#undef v11
