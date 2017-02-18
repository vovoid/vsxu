
#include <container/vsx_nw_vector.h>


/**
 * The HTNode interface should be implemented by
 * object that are node of the tree that want to be
 * displayed in the TreeMap.
 * <P>
 * If you have already a tree structure, just implements
 * this interface in node of the tree.
 *
 * @author Christophe Bouthier [bouthier@loria.fr]
 * @version 1.0
 */
class HTNode {

public:
  vsx_module_specification* module_info;
  int value;
  vsx_string<>name;
  vsx_color<> color;
  vsx_vector3<> size;
  std::vector<HTNode*> children;

  HTNode() {
    module_info = 0x0;
    value = 0;
    color.r = 0.8;
    color.g = 0.8 ;
    color.b = 1;
    color.a = 1;
    size.x = 0.13;
    size.y = 0.025;
  }

  virtual ~HTNode() {
    for (std::vector<HTNode*>::iterator it = children.begin(); it != children.end(); ++it) {
      delete *it;
    }
  }


  // this function will build a tree structure out of a string
  // string syntax: texture;generators;simple;jaw's_own_texgen
  //
  HTNode* add(vsx_string<>add_name, vsx_module_specification* m_info) {
    vsx_nw_vector<vsx_string<> > add_c;
    vsx_string<> deli = ";";
    vsx_string_helper::explode(add_name,deli,add_c,-1);
    if (!module_info) {
      module_info = m_info;
    }

    vsx_string<>comp_class = module_info->component_class;

    if (add_c.size() > 1) {
      // we're not the last part of the string
      // we start by looking in our children to see if the supernode already exists
      HTNode* search_result = 0;
      vsx_string<>this_name = add_c[0];
      for (unsigned long i = 0; i < children.size(); i++) {
        if (children[i]->name == this_name) {
          search_result = children[i];
          i = children.size()+1;
        }
      }
      add_c.remove_index(0);
      vsx_string<> new_add_name;
      vsx_string<> semicolon(";");
      if (add_c.size())
        new_add_name = vsx_string_helper::implode(add_c, semicolon);

      if (search_result) {
        // ask the child to do some creation
        search_result->module_info = module_info;
        search_result->add(new_add_name,m_info);
      } else
      {
        // the child we need to send further instructions to doesn't exist, so create it
        HTNode* b = new HTNode();
        children.push_back(b);
        b->name = this_name;
        b->value = 0;
        b->module_info = module_info;

        if (mc_colors.find(comp_class) != mc_colors.end()) {
          b->color = mc_colors[comp_class];
        } else
        if (mc_colors.find("default") != mc_colors.end()) {
          b->color = mc_colors["default"];
        } else
        {
          b->color.r = 0.8;
          b->color.g = 0.8;
          b->color.b = 0.8;
        }
        return b->add(new_add_name,m_info);
      }
    } else {
        size.x *= 1.05;
        size.y *= 1.05;
        if (size.x > 0.2) size.x = 0.2;
        if (size.y > 0.1) size.y = 0.1;
      // we've only been told to create this, so do that and nothing more.
      HTNode* b = new HTNode();
      if (module_info->component_class == "resource") {
        vsx_nw_vector <vsx_string<> > parts;
        vsx_string<>deli = ".";
        vsx_string_helper::explode(add_name,deli, parts);
        if (parts.size() > 1) {
          if (mc_r_colors.find(parts[parts.size()-1]) != mc_r_colors.end()) {
            b->color = mc_r_colors[parts[parts.size()-1]];
          } else {
            b->color.r = 0.5;
            b->color.g = 0.5;
            b->color.b = 0.5;
          }
        } else {
          if (mc_colors.find("resources_") != mc_colors.end()) {
            b->color = mc_colors["resources_"];
          } else {
            b->color.r = 0.5;
            b->color.g = 0.5;
            b->color.b = 0.5;
          }
        }
      } else
      if (mc_colors.find(comp_class+"_") != mc_colors.end()) {
        b->color = mc_colors[comp_class+"_"];
      } else
      if (mc_colors.find("default_") != mc_colors.end()) {
        b->color = mc_colors["default_"];
      } else
      {
        b->color.r = 1;
        b->color.g = 1;
        b->color.b = 1;
      }
      b->name = vsx_string_helper::str_replace<char>("\\ "," ",add_name);
      b->value = 1;
      b->module_info = module_info;
      children.push_back(b);
      return b;
    }
    return 0;
  }

  /**
   * Checks if this node is a leaf or not.
   * A node could have no children and still not
   * be a leaf.
   *
   * @return    <CODE>true</CODE> if this node is a leaf;
   *            <CODE>false</CODE> otherwise
   */
  virtual bool isLeaf() {
    if (value) return true; else return false;
    //  if (!children.size()) return true; else return false;
  }


  /**
   * Returns the name of this node.
   * Used to display a label in the hyperbolic tree.
   *
   * @return    the name of this node
   */
  vsx_string<>getName() {
    return name;
  }
};
