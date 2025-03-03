#include "FluidApplication.hpp"
#include <iostream>
using namespace std;
const string vertex=R"(
attribute vec4 vPosition;
varying vec2 fPos;
void main()                 
{
fPos=vPosition.xy;
float x=vPosition.x;
float y=vPosition.y;
gl_Position=vec4(2.0*x-1.0,2.0*y-1.0,0.0,1.0);
}                           
)";
const string fragment=R"(
precision mediump float;
varying vec2 fPos;
uniform sampler2D intensity;
void main()                                
{                                          
float dn=texture2D(intensity,fPos+vec2(0.0,0.01)).r;
float d=texture2D(intensity,fPos).r;
float dp=texture2D(intensity,fPos-vec2(0.0,0.01)).r;
float D=0.7+2.0*(dp-dn);
const vec4 startColor=vec4(0.0,1.0,0.0,0.4);
const vec4 endColor=vec4(0.0,0.0,0.9,0.9);
gl_FragColor=D*mix(startColor,endColor,d*6.0);
gl_FragColor.a=1.0;
}
)";
void FluidApplication::init()
{
intensity.resize(N*N,128);
program.reset(new ShaderProgram(vertex,fragment));
glGenBuffers(1,&vbo);
glBindBuffer(GL_ARRAY_BUFFER,vbo);
GLfloat vVertices[]={
0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,1.0f,0.0f
};
glBufferData(GL_ARRAY_BUFFER,6*3*sizeof(GLfloat),&vVertices[0],GL_STATIC_DRAW);
glBindBuffer(GL_ARRAY_BUFFER,0);
buildTexture();
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
simulation.dt=1.0f;
simulation.viscosity=0.01f;
simulation.diffusion=0.01f;
}
void FluidApplication::simulate()
{
int mouse_x=0;
int mouse_y=0;
static bool mouse_pressed_previous=false;
static int mouse_x_previous=0;
static int mouse_y_previous=0;
int state=SDL_GetMouseState(&mouse_x,&mouse_y);
bool mouse_left=state&1;
bool mouse_right=state&4;
bool put_matter=mouse_left;
bool put_acceleration=mouse_right!=mouse_left;
if(mouse_left||mouse_right)
{
mouse_x=mouse_x*N/width;
mouse_y=N-1-mouse_y*N/height;
if(mouse_pressed_previous)
{
int Dx=mouse_x-mouse_x_previous;
int Dy=mouse_y-mouse_y_previous;
float dr=sqrt(Dx*Dx+Dy*Dy)+0.001;
float dx=Dx/(dr);
float dy=Dy/(dr);
static const int radius=5;
for(int r=0;r<dr;r++)
{
int X=mouse_x_previous+r*dx;
int Y=mouse_y_previous+r*dy;
int x_start=max(0,X-radius);
int y_start=max(0,Y-radius);
int x_end=min(N-1,X+radius);
int y_end=min(N-1,Y+radius);
for(int x=x_start;x<x_end;++x)
{
for(int y=y_start;y<y_end;++y)
{
double DX=x-X;
double DY=y-Y;
double power=std::max(0.0,radius*radius-DX*DX-DY*DY)/(radius*radius);
{
simulation.velocityX[x+N*y]+=0.5*dx*power*put_acceleration;
simulation.velocityY[x+N*y]+=0.5*dy*power*put_acceleration;
simulation.density[x+N*y]+=0.8*float(width)/N*put_matter*power;
}
}
}
}
}
mouse_x_previous=mouse_x;
mouse_y_previous=mouse_y;
mouse_pressed_previous=true;
}else
{
mouse_pressed_previous=false;
}
simulation.evolve();
updateTexture();
}
void FluidApplication::step()
{
simulate();
{
glUseProgram(program->data());
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D,texture_id);
glUniform1i(glGetUniformLocation(program->data(),"intensity"),0);
glBindBuffer(GL_ARRAY_BUFFER,vbo);
glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
glEnableVertexAttribArray(0);
glDrawArrays(GL_TRIANGLES,0,3*2);
glBindBuffer(GL_ARRAY_BUFFER,0);
}
}
void FluidApplication::buildTexture()
{
glGenTextures(1,&texture_id);
glBindTexture(GL_TEXTURE_2D,texture_id);
glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,N,N,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,(const GLvoid *) intensity.data());
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
glBindTexture(GL_TEXTURE_2D,0);
}
void FluidApplication::updateTexture()
{
for(int i=0;i<N*N;++i)
{
double dx=simulation.velocityX[i];
double dy=simulation.velocityY[i];
double d=simulation.density[i];
intensity[i]=max(0.0,min(d,255.0));
}
glBindTexture(GL_TEXTURE_2D,texture_id);
glTexSubImage2D(GL_TEXTURE_2D,0,0,0,N,N,GL_LUMINANCE,GL_UNSIGNED_BYTE,(const GLvoid *) intensity.data());
glBindTexture(GL_TEXTURE_2D,0);
}
