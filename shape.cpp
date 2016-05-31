// $Id: shape.cpp,v 1.7 2014-05-08 18:32:56-07 - - $
//Robert Ho rokho@ucsc.edu
//Daniel Urrutia deurruti@ucsc.edu
#include <cmath>
#include <typeinfo>
#include <unordered_map>
#include <string>
using namespace std;

#include "shape.h"
#include "util.h"

#include <GL/freeglut.h>

vertex_list emp{};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font, const string& textdata):
      glut_bitmap_font(glut_bitmap_font), textdata(textdata) {
   DEBUGF ('c', this);

}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);
}

triangle::triangle (const vertex_list& vertices): polygon(vertices) {
   DEBUGF ('c', this);
}
rectangle::rectangle (GLfloat width, GLfloat height):
            polygon({}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
   vertex vert({width/2,height/2});
   vertices.push_back(vert);
   vert= {width/2, -height/2};
   vertices.push_back(vert);
   vert ={-width/2,-height/2};
   vertices.push_back(vert);
   vert= {-width/2,height/2}; 
   vertices.push_back(vert);
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond (GLfloat width, GLfloat height):polygon({}){
   vertex vert({0,height/2});
   vertices.push_back(vert);
   vert= {width/2, 0};
   vertices.push_back(vert);
   vert ={0,-height/2};
   vertices.push_back(vert);
   vert= {-width/2,0}; 
   vertices.push_back(vert);
}

right_triangle::right_triangle (GLfloat width, GLfloat height)
                                :triangle({}){

   vertex vert({-width/4 ,3*height/4});
   vertices.push_back(vert);
   vert= {3*width/4, -height/4};
   vertices.push_back(vert);
   vert ={-width/4,-height/4};
   vertices.push_back(vert);


}

isosceles::isosceles(GLfloat width, GLfloat height):triangle({}){
   DEBUGF ('c', this);
   vertex vert({0, height/2});
   vertices.push_back(vert);
   vert= {-width/2, -height/2};
   vertices.push_back(vert);
   vert ={width/2, -height/2};
   vertices.push_back(vert);
}

equilateral::equilateral(GLfloat width):triangle({}){
   DEBUGF ('c', this);
   vertex vert({-width/2, -width/2});
   vertices.push_back(vert);
   vert= {0, width/2};
   vertices.push_back(vert);
   vert ={width/2, -width/2};
   vertices.push_back(vert);
}

void ellipse::draw_label(const vertex& center, int obj_num) const{
   string words = to_string(obj_num);  
   void *font = GLUT_BITMAP_8_BY_13;
   rgbcolor lcolor{"black"};
   glColor3ubv(lcolor.ubvec);
   glRasterPos2f(center.xpos,center.ypos);
   for( auto ch: words) glutBitmapCharacter(font,ch);

}

void polygon::draw_label(const vertex& center, int obj_num) const{
   string words = to_string(obj_num);  
   void *font = GLUT_BITMAP_8_BY_13;
   rgbcolor lcolor{"black"};
   glColor3ubv(lcolor.ubvec);
   glRasterPos2f(center.xpos,center.ypos);
   for( auto ch: words) glutBitmapCharacter(font,ch);


}

void text::draw_label(const vertex& center, int obj_num) const{


}


void text::draw (const vertex& center, const rgbcolor& color, 
                 bool border = false) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   string words = this->textdata; 
   void *font = this->glut_bitmap_font;
   glColor3ubv(color.ubvec);
   glRasterPos2f(center.xpos,center.ypos);
   for( auto ch: words) glutBitmapCharacter(font,ch);

}



void text::draw_border(const vertex& center, int thickness,
                       const string& color)const{

}

void polygon::draw_border(const vertex& center, int thickness,
                          const string& color) const{
   glBegin (GL_POLYGON);
   rgbcolor scolor{color};
   glColor3ubv (scolor.ubvec);
   auto begin = this->vertices.begin();
   auto end = this->vertices.end();
   
   while(begin != end) {
      float sizex = begin->xpos; 
      float sizey = begin->ypos; 
      if(sizex < 0 and sizey > 0){
         sizex -= thickness;
         sizey += thickness;
      }else if(sizey <0 and sizex > 0){
         sizey -= thickness;
         sizex += thickness;
      }else if(sizex < 0 and sizey < 0){
         sizex -= thickness;
         sizey -= thickness;
      }else if(sizex > 0 and sizey >0){
         sizex += thickness;
         sizey += thickness;
      }else if(sizex == 0 and sizey > 0){ 
         sizey += thickness;
      }else if(sizex == 0 and sizey < 0){ 
         sizey -= thickness;
      }else if(sizey == 0 and sizex > 0){ 
         sizex += thickness;
      }else if(sizey == 0 and sizex < 0){ 
         sizex -= thickness;
      }
      //cout << sizex << " " << sizey << endl;
      glVertex2f (sizex + center.xpos, sizey + center.ypos);
      ++begin; 
   }  
   glEnd(); 
}

void ellipse::draw (const vertex& center, const rgbcolor& color,
                    bool border = false) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv(color.ubvec);
   //im editing this we never use the center 
   const float delta = 2 * M_PI/32 ; 
   float width =  this->dimension.xpos  /3;
   float height =  this->dimension.ypos/3;
   int w = center.xpos;
   int h = center.ypos;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      GLfloat xpos = width * cos(theta) + w;
      GLfloat ypos = height * sin(theta) + h;
      glVertex2f(xpos,ypos);  
   }
   glEnd();
 

}

void ellipse::draw_border(const vertex& center, int thickness,
                          const string& color) const{
      //cout <<"HELLO" << endl;
      glBegin(GL_POLYGON);
      glEnable(GL_LINE_SMOOTH);
      rgbcolor scolor{color};
      glColor3ubv (scolor.ubvec);
   const float delta = 2 * M_PI / 32;
   float width = (this->dimension.xpos /3) + thickness  ;
   float height = (this->dimension.ypos  / 3) + thickness  ;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float xpos = width * cos (theta) + center.xpos;
      float ypos = height * sin (theta) + center.ypos ;
      glVertex2f (xpos, ypos);
   }
   glEnd();

}

void polygon::draw (const vertex& center, const rgbcolor& color,
                    bool border = false) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_POLYGON);
   glColor3ubv (color.ubvec);
   auto begin = this->vertices.begin();
   auto end = this->vertices.end();
   
   while(begin != end) {
      glVertex2f (begin->xpos + center.xpos, begin->ypos + center.ypos);
      ++begin; 
   }  
   glEnd(); 
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

