#include "light.h"
#include "object.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"
#include "vec.h"
#include <math.h>
#include <algorithm>

vec3 Phong_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    vec3 color;
    vec3 lpos;  //light position
    vec3 lightToIntersection;   //vector from light to intesection
    vec3 ltiNormalized;
    double i_d;
    
    vec3 summation;
    vec3 r; //reflection
    vec3 v;
    vec3 specD;
    vec3 diffD;
    int i=0;
    if(debug_pixel){
        std::cout<<"ambient = "<<color_ambient*world.ambient_intensity*world.ambient_color<<std::endl;
    }
    for(Light * m: world.lights){
        //DIFFUSE
        
        lpos=m->position;
        lightToIntersection=m->Emitted_Light(lpos-intersection_point);
        ltiNormalized=lightToIntersection.normalized();
        //i_d=m->brightness;

        if(world.enable_shadows){
            
            Ray shot;
            shot.endpoint=intersection_point;
            shot.direction=(lpos-intersection_point).normalized();
            Hit d = world.Closest_Intersection(shot);
            
            if((d.dist!=(lpos-intersection_point).magnitude() && d.object!=0 && d.dist>=small_t)&& (lpos-intersection_point).magnitude()>d.dist){
                if(debug_pixel){
                    std::cout<<"distance: "<<d.dist<<std::endl;//distances are correct
                }
                continue;
                
            }
        }
        

        double d=dot(normal,(lpos-intersection_point).normalized());
        if(debug_pixel){
            //std::cout<<"dot: "<<d<<std::endl;
            //std::cout<<"light to intersection: "<<lightToIntersection<<std::endl;
        }
        summation+= color_diffuse*lightToIntersection*std::max(0.0,d);  //Rd=color_diffuse, Ld=lightToIntersection.
        diffD=summation;
        //REFLECTION
        r=normal*2*(dot((lpos-intersection_point).normalized(),normal))-(lpos-intersection_point).normalized();
        v=world.camera.position-intersection_point;

        specD=color_specular*pow(std::max(dot(r.normalized(),v.normalized()),0.0),specular_power)*lightToIntersection;
        summation+=specD;
        if(debug_pixel){
            std::cout<<"shading for light "<<i<<": diffuse = "<<diffD<<"; specular = "<<specD<<std::endl;
        }
        
        i++;

    }
    
    
    color=summation+color_ambient*world.ambient_color*world.ambient_intensity;    //Combine with ambient
    if(debug_pixel){
        
        
        std::cout<<"final color: "<<color<<std::endl;
    }
    
    
    return color;
}
