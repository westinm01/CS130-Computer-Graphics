#include "reflective_shader.h"
#include "ray.h"
#include "render_world.h"

vec3 Reflective_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    vec3 color;
    vec3 cr;
    vec3 co = shader->Shade_Surface(ray,intersection_point,normal,recursion_depth);
    if(recursion_depth<world.recursion_depth_limit){
        
        vec3 itocam = (world.camera.position-intersection_point).normalized();//intersection to camera direction
        Ray r;
        r.endpoint=intersection_point;
        r.direction=((2*dot(itocam, normal)*normal)-itocam).normalized();
        cr=world.Cast_Ray(r,++recursion_depth);
    }
    color=co+reflectivity*(cr-co);
    return color;
}
