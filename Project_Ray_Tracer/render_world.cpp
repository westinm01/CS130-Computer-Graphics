#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"
#include <limits>

extern bool disable_hierarchy;

Render_World::Render_World()
    :background_shader(0),ambient_intensity(0),enable_shadows(true),
    recursion_depth_limit(3)
{}

Render_World::~Render_World()
{
    delete background_shader;
    for(size_t i=0;i<objects.size();i++) delete objects[i];
    for(size_t i=0;i<lights.size();i++) delete lights[i];
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
Hit Render_World::Closest_Intersection(const Ray& ray)
{
    double min_t = std::numeric_limits<double>::max();//assigns min_t as background.
    Hit closestHit;
    closestHit.object=nullptr;
    closestHit.dist=min_t;
    closestHit.part=-1;
    for(Object * o: objects){
        Hit h = o->Intersection(ray,-1);
        if(h.dist<min_t && h.dist>=small_t){
            closestHit=h;
            min_t=h.dist;//save min_t with current closestHit's distance
            closestHit.part=h.part;
        }
    }
    return closestHit;
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{//Bug in here
    vec3 direction=camera.World_Position(pixel_index)-camera.position; //direction is a unit vector that can be scaled by a scalar t.
    //direction=direction.normalized();
    vec3 endpoint = camera.position; //endpoint is simply the pixel_index when z=0;
    Ray ray=Ray(endpoint,direction);
    vec3 color=Cast_Ray(ray,1);
    camera.Set_Pixel(pixel_index,Pixel_Color(color));
}

void Render_World::Render()
{
    if(!disable_hierarchy)
        Initialize_Hierarchy();

    for(int j=0;j<camera.number_pixels[1];j++){
        for(int i=0;i<camera.number_pixels[0];i++){
            Render_Pixel(ivec2(i,j));
        }
    }
}

int Render_World::findobj_index(const Object*obj){
    for(size_t i=0;i<objects.size();i++){
        if(objects[i]==obj){
            return i;
        }
    }
    return -1;
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth)
{
    vec3 color=background_shader->Shade_Surface(ray,vec3(0,0,0),vec3(0,0,0),recursion_depth);
    if(debug_pixel){//debugging
        std::cout<<"cast ray: end ="<<ray.endpoint<<"; dir = "<<ray.direction<<std::endl;
        
    }
    Hit h = Closest_Intersection(ray);
    if(debug_pixel){
        std::cout<<"intersection with obj["<<findobj_index(h.object)<<"]; part "<<h.part<<"; dist = " <<h.dist<<std::endl;
    }
    if(h.dist==std::numeric_limits<double>::max()|| h.dist<=small_t){
        return color;
    }
    if(h.object!=nullptr){//if the closest intersection hit an object, color is assigned to the color of the object at that intersection.
        
        if(debug_pixel){
            //vec3 hn=ray.direction*h.dist+ray.endpoint;
            std::cout<<"call Shade_Surface with: location = "<<ray.direction*h.dist+ray.endpoint<<"; normal = "<<h.object->Normal(ray.direction*h.dist+ray.endpoint,0)<<std::endl;
        }
        
        color=h.object->material_shader->Shade_Surface(ray,ray.direction*h.dist+ray.endpoint, h.object->Normal(ray.direction*h.dist+ray.endpoint,0),recursion_depth);//
        
    }
    
    return color;
}

void Render_World::Initialize_Hierarchy()
{
    TODO; // Fill in hierarchy.entries; there should be one entry for
    // each part of each object.

    hierarchy.Reorder_Entries();
    hierarchy.Build_Tree();
}
