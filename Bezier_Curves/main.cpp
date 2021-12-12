// Name:
// Quarter, Year:
// Lab:
//
// This file is to be modified by the student.
// main.cpp
////////////////////////////////////////////////////////////
#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

#include <vector>
#include <cstdio>
#include <math.h>
#include "vec.h"
#include <iostream>

using namespace std;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
vector <double> controls;//vector of control points

void coefficients (double * c, int n, double t){
    for(int i=0;i<=n;i++){
        if(i==0){
            c[0]=1;
        }
        else{
            c[i]=c[i-1]*(n-i+1)/i*t;
        }
    }
    for(int i=n;i>=0;i--){
        if(i!=n){
            c[i]=c[i+1]*(1-t)/(c[i]*t*(n-i+2)/(i+1));
        }
    }
}

double factorial(int n){
    if(n<=1){
        return 1;
    }
    return n*factorial(n-1);

}

double combination(int n, int k){
    double nf=factorial(n);
    double kf=factorial(k);
    double knf=factorial(n-k);
    return nf/(kf*knf);
}
double polynomial(int n, int k, double t){
    return combination(n,k)*pow(t,k)*pow((1-t),n-k);
}
/*vec2 b(unsigned int p1, unsigned int p2, double t){
    if(p2-p1==2){
        vec2 point1={controls.at(p1),controls.at(p1+1)};
        vec2 point2={controls.at(p2),controls.at(p2+1)};
        return (1-t)*point1 + t*point2;
    }
    return (1-t)*b(p1,p2-2)+t*b(p2-2,p2);
}*/
void GL_render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();

    glBegin(GL_LINE_STRIP);
    glColor3f(1.0f,0.0f,0.0f);

    // TODO: just for example, you will need to change this.
    /*glVertex2f(-.5f,-.5f);
    glVertex2f(.5f,-.5f);
    glVertex2f(.5f,.5f);
    glVertex2f(-.5f,.5f);*/
    /*for(int i=0;i<controls.size();i++){
        glVertex2f(controls.at(i),controls.at(++i));
    }*/
    if(controls.size()>=4){
    //double * c= new double [101];
    unsigned int n=controls.size()/2;
    for(double t=0;t<=1;t+=0.01){
        //coefficients(c,n,t);
        double sumx=0;
        double sumy=0;
        for(unsigned int i=0;i<2*n;i+=2){
            double x,y;
            x=controls.at(i)*polynomial(n,i/2,t);
            y=controls.at(i+1)*polynomial(n,i/2,t);
            sumx+=x;
            sumy+=y;
        }
        glVertex2f(sumx,sumy);
    }}
    glEnd();
    
    glFlush();
}

void GL_mouse(int button,int state,int x,int y)
{
    y=WINDOW_HEIGHT-y;
    GLdouble mv_mat[16];
    GLdouble proj_mat[16];
    GLint vp_mat[4];
    glGetDoublev(GL_MODELVIEW_MATRIX,mv_mat);
    glGetDoublev(GL_PROJECTION_MATRIX,proj_mat);
    glGetIntegerv(GL_VIEWPORT,vp_mat);

    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
        double px,py,dummy_z; // we don't care about the z-value but need something to pass in
        gluUnProject(x,y,0,mv_mat,proj_mat,vp_mat,&px,&py,&dummy_z);
        
        controls.push_back(px);
        controls.push_back(py);//px and py have been pushed back as control points!
        glutPostRedisplay();
    }
}

//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
    glutInit(argc, argv);
    //glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    //glMatrixMode(GL_PROJECTION_MATRIX);
    //glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    glutCreateWindow("CS 130 - Westin Montano");
    glutDisplayFunc(GL_render);
    glutMouseFunc(GL_mouse);
}


int main(int argc, char** argv)
{
    GLInit(&argc, argv);
    glutMainLoop();
    return 0;
}
