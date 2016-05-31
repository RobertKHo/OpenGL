// $Id: interp.cpp,v 1.15 2014-07-22 20:03:19-07 - - $
//Robert Ho rokho@ucsc.edu
//Daniel Urrutia deurruti@ucsc.edu
#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

map<string,interpreter::interpreterfn> interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"border"   , &interpreter::do_border   },
   {"moveby"   , &interpreter::do_move   },
};

map<string,interpreter::factoryfn> interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"diamond"  , &interpreter::make_diamond  },
   {"triangle" , &interpreter::make_triangle },
   {"right_triangle"   , &interpreter::make_right_triangle   },
   {"isosceles", &interpreter::make_isosceles },
   {"equilateral", &interpreter::make_equilateral },
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

void interpreter::do_move (param begin, param end){
   DEBUGF ('f', range (begin, end));
   string::size_type sh;
   float pixel = stof(*begin++,&sh);
   window::set_float(pixel);

} 

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.insert ({name, make_shape (++begin, end)});
}

void interpreter::do_border (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string color = *begin++; 
   string::size_type sh;
   int tness = stof(*begin,&sh);
   window::set_border(tness,color);
   //cout << "Color: " << color << "Thickness" << *begin; 
   
}


void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 3 and end - begin != 4){
      throw runtime_error("syntax error");
   }
   string colour = begin[0];
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
    rgbcolor color {colour};
    object obj(itor->second,where,color);
    window::push_back(obj);
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   cout << type << endl;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string f = *begin++;
   auto find = fontcode.find(f); 
   cout << f << endl;
   string words = ""; 
   while (begin != end){
     words.append(*begin + " "); 
     ++begin; 
   }
    return make_shared<text> (find->second, words);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string::size_type sw; 
   string::size_type sh;
   float width = stof(*begin++,&sw);

   float height = stof(*begin,&sh);

   return make_shared<ellipse> (width, height);
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string::size_type sw;
   float diameter = stof(*begin++, &sw);
   cout << diameter << endl;
   return make_shared<circle> (diameter);
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list verts; 

   string::size_type sw; 
   string::size_type sh;

   while(begin != end){
      float xpos = stof(*begin++,&sw);
      if(begin != end){ 
      float ypos  = stof(*begin++,&sh);
      vertex pair({xpos, ypos});
      verts.push_back(pair); 
      } 
   } 
   vertex_list avg; 
   float xavg = 0; 
   float yavg = 0;
   int count = 0;
   auto vert_b = verts.begin();
   auto vert_e = verts.end();
   while( vert_b != vert_e) {
      xavg = xavg + vert_b->xpos; 
      yavg = yavg + vert_b->ypos; 
      count++;  
      ++vert_b;        
   }  
   vertex newpos({xavg/count,yavg/count}) ;
   vert_b = verts.begin();
   vert_e =   verts.end();
   while(vert_b != vert_e){ 
      float newx = vert_b->xpos - newpos.xpos;
      float newy = vert_b->ypos - newpos.ypos;
      vertex norm({newx,newy});
      avg.push_back(norm);
      ++vert_b;
   }   

   return make_shared<polygon> (avg);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string::size_type sw; 
   string::size_type sh;
   float width = stof(*begin++,&sw);
   float height = stof(*begin,&sh);
   return make_shared<rectangle> (width, height);
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
    string::size_type sw;
   float width = stof(*begin++,&sw);
   return make_shared<square> (width);
}

shape_ptr interpreter::make_diamond (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string::size_type sw; 
   string::size_type sh;
   float width = stof(*begin++,&sw);
   float height = stof(*begin,&sh);
   return make_shared<diamond>(width,height);
}

shape_ptr interpreter::make_right_triangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string::size_type sw; 
   string::size_type sh;
   float width = stof(*begin++,&sw);
   float height = stof(*begin,&sh);
   return make_shared<right_triangle>(width,height);

}

shape_ptr interpreter::make_triangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list verts; 

   string::size_type sw; 
   string::size_type sh;

   while(begin != end){
      float xpos = stof(*begin++,&sw);
      if(begin != end){ 
      float ypos  = stof(*begin++,&sh);
      vertex pair({xpos, ypos});
      verts.push_back(pair); 
      } 
   } 
   vertex_list avg; 
   float xavg = 0; 
   float yavg = 0;
   int count = 0;
   auto vert_b = verts.begin();
   auto vert_e = verts.end();
   while( vert_b != vert_e) {
      xavg = xavg + vert_b->xpos; 
      yavg = yavg + vert_b->ypos; 
      count++;  
      ++vert_b;        
   }  
   vertex newpos({xavg/count,yavg/count}) ;
   vert_b = verts.begin();
   vert_e =   verts.end();
   while(vert_b != vert_e){ 
      float newx = vert_b->xpos - newpos.xpos;
      float newy = vert_b->ypos - newpos.ypos;
      vertex norm({newx,newy});
      avg.push_back(norm);
      ++vert_b;
   }   

   return make_shared<triangle> (avg);
}

shape_ptr interpreter::make_isosceles(param begin, param end){
   DEBUGF ('f', range (begin, end));
   string::size_type sw; 
   string::size_type sh;
   float width = stof(*begin++,&sw);
   float height = stof(*begin,&sh);
   return make_shared<isosceles>(width, height);
}

shape_ptr interpreter::make_equilateral(param begin, param end){
   DEBUGF ('f', range (begin, end));
   string::size_type sw; 
   float width = stof(*begin++,&sw);;
   return make_shared<equilateral>(width);
}

