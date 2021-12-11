#include "plane.h"
#include "hit.h"
#include "ray.h"
#include <cfloat>
#include <limits>
#include <math.h>

// Intersect with the half space defined by the plane.  The plane's normal
// points outside.  If the ray starts on the "inside" side of the plane, be sure
// to record a hit with t=0 as the first entry in hits.
Hit Plane::Intersection(const Ray& ray, int part) const
{
    double t;
    Hit h;
    h.object=nullptr;
    h.dist=std::numeric_limits<double>::max();
    vec3 e = ray.endpoint;
    vec3 d = ray.direction;
    double dn = dot(d,normal);
    if(dn==0){
        //no intersection
        return h;
    }
    
    t=dot((x1-e),normal)/dn;
    if(t<small_t){
        return h;
    }
    //if the code reaches here, then the ray eventually hits the plane.
    h.dist=t;
    h.object=this;
    h.part=part;
    return h;
}

vec3 Plane::Normal(const vec3& point, int part) const
{

    
    return normal;
}

// There is not a good answer for the bounding box of an infinite object.
// The safe thing to do is to return a box that contains everything.
Box Plane::Bounding_Box(int part) const
{
    Box b;
    b.hi.fill(std::numeric_limits<double>::max());
    b.lo=-b.hi;
    return b;
}

