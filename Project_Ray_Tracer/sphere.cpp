#include "sphere.h"
#include "ray.h"
#include <math.h> 
#include <iostream>
// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    
    //dot((e+tu-center),(e+tu-center))=r^2
    double tpos=0;
    double tneg=0;
    Hit h;
    h.object=nullptr;
    h.dist=std::numeric_limits<double>::max();
    vec3 ec = ray.endpoint-this->center;
    double a = dot(ray.direction,ray.direction);
    double b = dot(ray.direction*2, ec);
    double c = dot(ec,ec)-this->radius*this->radius;
    double sq=b*b-4*a*c;
    if(sq<0 || a==0){
        //0 intersections, misses sphere.
        return h;
    }
    tpos=((-1*b)+sqrt(b*b-4*a*c))/(2*a);
    h.object=this;
    h.part=part;
    if(sq==0){
        // 1 interesection.
        h.dist=(ray.direction*tpos-ray.endpoint).magnitude();
        h.dist=tpos;
        return h;
    }
    //if the code reaches here, there must be 2 intersection points, so we have to find the closest.
    tneg=((-1*b)-sqrt(b*b-4*a*c))/(2*a);
    if(tneg<tpos && tneg>small_t){
        h.dist=(ray.direction*tneg-ray.endpoint).magnitude();
        h.dist=tneg;
    }
    else{
        h.dist=(ray.direction*tpos-ray.endpoint).magnitude();
        h.dist=tpos;
    }
    return h;
}

vec3 Sphere::Normal(const vec3& point, int part) const
{
    vec3 normal;
    //THIS WORKS!!!!
    normal=(point-center).normalized();
    return normal;
}

Box Sphere::Bounding_Box(int part) const
{
    Box box;
    TODO; // calculate bounding box
    return box;
}

