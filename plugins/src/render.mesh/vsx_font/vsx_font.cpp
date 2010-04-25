#include "vsx_gl_global.h"
#include "vsx_math_3d.h"
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_font.h"

vsx_font_info* vsx_font::init(vsx_string font) {
  //vsx_font_info* font_info;
  my_font_info =  new vsx_font_info;

  if (font[0] != '-') {
    my_font_info->type = 0;
    my_font_info->texture = new vsx_texture();
    //std::cout << "registering new font: "<< font << std::endl;
    //font_info->texture->load_png(base_path+"resources\\fonts\\font-"+font+".png",true);
    my_font_info->texture->load_png(base_path+font,true);
    ch = 16.0/255.0f;
    cw = 10.0/255.0f;
    //printf("loading font: %s\n",(base_path+font).c_str());
  }
  return my_font_info;
}  

void vsx_font::reinit(vsx_font_info* f_info,vsx_string font) {
  if (f_info->type == 0) {
//  	printf("reinit %s\n",(base_path+font).c_str());
    f_info->texture->load_png(base_path+font,true);
  }
}  

void vsx_font::reinit_all_active() {
}  

vsx_vector vsx_font::print(vsx_vector p, const vsx_string& str, const vsx_string& font, float size = 1, vsx_string colors) {
  if (my_font_info)
  return print(p,str,size,colors);
  return vsx_vector(0);
}

  vsx_vector vsx_font::print(vsx_vector p, const vsx_string& str, const float size = 1, vsx_string colors) {
    if (!my_font_info) return vsx_vector();
    if (str == "") return p;
    
    bool use_colors = false;
    if (colors.size()) use_colors = true;
    char current_color = 1;
    
    if (my_font_info->type == 0)
    {
      size_s = 0.37*size;    
      
      dx = 0;
      dy = 0;
      //dz = 0;

      glPushMatrix();
      //glLoadIdentity();
      glTranslatef(p.x,p.y,p.z);
      //if (background) {
        //background_color.gl_color();
          /*glBegin(GL_QUADS);
        for (int i = 0; i < str.size(); ++i) {
          if (str[i] == 0x0A) {
            if (mode_2d)
            dy -= size*1.05; else
            dy += size*1.05;
            dx = 0;
          } 
              glVertex3f(dx,dy,0);
              glVertex3f(dx,dy+size,0);
              glVertex3f(dx+size_s,dy+size,0);
              glVertex3f(dx+size_s,dy,0);
          dx += align*size_s;
        }
          glEnd();*/
      //}
      
        //dx = 0;
        //dy = 0;
        
      
        (*(my_font_info->texture)).bind();
        //stc = str.c_str();
        colc = (char*)colors.c_str();

        if (use_colors) 
        syntax_colors[*colc-1].gl_color();
        else
        glColor4f(color.r,color.g,color.b,color.a);
        
        glBegin(GL_QUADS);

        
        if (mode_2d)
        ddy = -size*1.05; else
        ddy = size*1.05;
        ddx = align*size_s;
        for (stc = (char*)str.c_str(); *stc; ++stc) {
          if (use_colors) {
            if (*colc != current_color) {
              current_color = *colc;
              syntax_colors[*colc-1].gl_color();
            }
            ++colc;
          }
          
          if (*stc == 0x0A) {
            dy += ddy;
            dx = 0;
          } else
          {
            sx = (float)(*stc % 16) * cw;
            sy = (float)(*stc / 16) * ch;
            ex = sx+cw;
            ey = -(sy+ch)+1.0f;
            sy = -sy+0.995f;
            //-ey+1.0f

              glTexCoord2f(sx, ey);  glVertex2f(dx       ,  dy);
            	glTexCoord2f(sx, sy);  glVertex2f(dx       ,  dy+size);
            	glTexCoord2f(ex, sy);  glVertex2f(dx+size_s,  dy+size);
            	glTexCoord2f(ex, ey);  glVertex2f(dx+size_s,  dy);
            dx += ddx;
          }
          //++stc;
        }
            glEnd();
        (*(my_font_info->texture))._bind();
        //old_string = str;
        //old_size = size;
        //list_built = true;
      glPopMatrix();
      ep.x = dx+p.x;  // this might need to be fixed
      ep.y = dy+p.y;
      //ep.z = p.z;
      return ep;
    }
    return vsx_vector();
  }

 vsx_vector vsx_font::get_size(const vsx_string& str, float size = 1) {
    int lines = 0;
    int max_char = 0;
    int cur_pos = 0;
    for (int i = 0; i < str.size(); ++i) {
      if (lines == 0) lines = 1;
      ++cur_pos;
      if (str[i] == 0x0A) {
        cur_pos = 0;
        ++lines;
      } else
      if (cur_pos > max_char) max_char = cur_pos;
    }
    return vsx_vector(((float)max_char)*0.37*size,size*1.05*(float)lines);
  }  
  
  vsx_vector vsx_font::print_center(vsx_vector p, const vsx_string& str, float size = 1) {
    p.x -= (align*0.37*size*(double)str.size())*0.5;
    float x1, 
    			//y1, 
    			//z1, 
    			x2 
    			//y2, 
    			//z2
    			;
    p.x -= (x2*size-x1*size)*0.5*0.08;
    return print(p,str,size);
  }

  vsx_vector vsx_font::print_right(vsx_vector p, const vsx_string& str, float size = 1) {
    p.x -= align*0.37*size*(double)str.size();
    return print(p,str,size);
  }
  


