#include "driver_state.h"
#include <cstring>

driver_state::driver_state()
{
}

driver_state::~driver_state()
{
    delete [] image_color;
    delete [] image_depth;
}

// This function should allocate and initialize the arrays that store color and
// depth.  This is not done during the constructor since the width and height
// are not known when this class is constructed.
void initialize_render(driver_state& state, int width, int height)
{
    state.image_width=width;
    state.image_height=height;
    
    state.image_color = new pixel[width*height];
    state.image_depth=new float[width*height];
    for(int i =0; i<width*height;i++){
        state.image_color[i]=make_pixel(0,0,0);
        state.image_depth[i]=1;
    }

    
    //std::cout<<"TODO: allocate and initialize state.image_depth."<<std::endl;
}

// This function will be called to render the data that has been stored in this class.
// Valid values of type are:
//   render_type::triangle - Each group of three vertices corresponds to a triangle.
//   render_type::indexed -  Each group of three indices in index_data corresponds
//                           to a triangle.  These numbers are indices into vertex_data.
//   render_type::fan -      The vertices are to be interpreted as a triangle fan.
//   render_type::strip -    The vertices are to be interpreted as a triangle strip.
void render(driver_state& state, render_type type)
{
    data_geometry *d = new data_geometry[state.num_vertices];
    //data_geometry array containing all important info for each vertex
    
    for(int i=0; i<state.num_vertices;i++){
        d[i].data=new float[state.floats_per_vertex];
    }
    //We now have all memory allocated!
    data_vertex * v;
    for(int i=0;i<state.num_vertices;i++){
        v=new data_vertex;
        v->data=&state.vertex_data[i*state.floats_per_vertex];
        state.vertex_shader(*v,d[i],state.uniform_data); //populates d[i] with appropriate info from *v.

        delete v;
    }
    
    switch(type){
        case render_type::triangle:
            state.num_triangles=state.num_vertices/3; //just to be safe..?
            for(int i=0;i<state.num_vertices;i=i+3){
                rasterize_triangle(state,d[i],d[i+1],d[i+2]);
            }
            break;
        case render_type::indexed:
            for(int i=0;i<state.num_triangles*3;i+=3){
                data_vertex * v1=new data_vertex;
                data_vertex * v2=new data_vertex;
                data_vertex *v3=new data_vertex;
                
                v1->data=&state.vertex_data[state.index_data[i]*state.floats_per_vertex];
                v2->data=&state.vertex_data[state.index_data[i+1]*state.floats_per_vertex];
                v3->data=&state.vertex_data[state.index_data[i+2]*state.floats_per_vertex];
                if(i>=state.num_vertices){
                    data_geometry *extra = new data_geometry[3];//allocate extra memory for more triangles than vertices
                    
                    for(int j=0; j<3;j++){
                        extra[j].data=new float[state.floats_per_vertex];
                    }

                    state.vertex_shader(*v1,extra[0],state.uniform_data);
                    state.vertex_shader(*v2, extra[1],state.uniform_data);
                    state.vertex_shader(*v3, extra[2],state.uniform_data);
                    rasterize_triangle(state,extra[0],extra[1],extra[2]); 
                }
                else{
                    state.vertex_shader(*v1,d[i],state.uniform_data);
                    state.vertex_shader(*v2, d[i+1],state.uniform_data);
                    state.vertex_shader(*v3, d[i+2],state.uniform_data);
                    rasterize_triangle(state, d[i],d[i+1],d[i+2]);
                }
                
            }
        break;
        case render_type::fan:
            for(int i=1;i<state.num_vertices;i++){
                rasterize_triangle(state,d[0],d[i],d[(i+1)%state.num_vertices]);
            }
        break;
        case render_type::strip:
            for(int i=0;i<state.num_vertices-2;i++){
                rasterize_triangle(state,d[i],d[i+1],d[i+2]);
            }
        default:
        break;
    }

    
    
}


// This function clips a triangle (defined by the three vertices in the "in" array).
// It will be called recursively, once for each clipping face (face=0, 1, ..., 5) to
// clip against each of the clipping faces in turn.  When face=6, clip_triangle should
// simply pass the call on to rasterize_triangle.
void clip_triangle(driver_state& state, const data_geometry& v0,
    const data_geometry& v1, const data_geometry& v2,int face)
{
    if(face==6)
    {
        rasterize_triangle(state, v0, v1, v2);
        return;
    }
    std::cout<<"TODO: implement clipping. (The current code passes the triangle through without clipping them.)"<<std::endl;
    clip_triangle(state,v0,v1,v2,face+1);
}

// Rasterize the triangle defined by the three vertices in the "in" array.  This
// function is responsible for rasterization, interpolation of data to
// fragments, calling the fragment shader, and z-buffering.
void rasterize_triangle(driver_state& state, const data_geometry& v0,
    const data_geometry& v1, const data_geometry& v2)
{
    
    float sw=state.image_width-1;
    float sh=state.image_height-1;

    //We first find the NDC coordinates for each vertex, then we convert them to the actual pixel space coordinates.
    float ax= v0.gl_Position[0]/v0.gl_Position[3];
    float ay= v0.gl_Position[1]/v0.gl_Position[3];
    float az = v0.gl_Position[2]/v0.gl_Position[3];
    float ai = (ax+1)/2*sw;
    float aj= (ay+1)/2*sh;
    //std::cout<<state.image_width<<", "<<state.image_height<<std::endl;
    //std::cout<<ai<<", "<<aj<<std::endl;

    float bx= v1.gl_Position[0]/v1.gl_Position[3];
    float by= v1.gl_Position[1]/v1.gl_Position[3];
    float bz=v1.gl_Position[2]/v1.gl_Position[3];
    float bi = (bx+1)/2*sw;
    float bj= (by+1)/2*sh;
    //std::cout<<bi<<", "<<bj<<std::endl;

    float cx= v2.gl_Position[0]/v2.gl_Position[3];
    float cy= v2.gl_Position[1]/v2.gl_Position[3];
    float cz = v2.gl_Position[2]/v2.gl_Position[3];
    float ci = (cx+1)/2*sw;
    float cj= (cy+1)/2*sh;
    //std::cout<<ci<<", "<<cj<<std::endl;

    float abc= .5*((bi*cj-bj*ci)+(aj*ci-ai*cj)+(ai*bj-aj*bi)); //area of triangle abc

    float alpha, beta, gamma;
    for (int i=0;i<state.image_width;i++){
        for(int j=0;j<state.image_height;j++){
            alpha=0.5* ((bi*cj-ci*bj)+(ci*j-i*cj)+(i*bj-bi*j))/abc;
            beta = 0.5*((i*cj-ci*j)+(ci*aj-ai*cj)+(ai*j-i*aj))/abc;
            gamma=0.5*((bi*j-i*bj)+(i*aj-ai*j)+(ai*bj-bi*aj))/abc;
            if(alpha>0.0 && beta>0.0 && gamma>0.0){
                //check interp_rules
                data_fragment *df= new data_fragment[1];
                df->data=new float [state.floats_per_vertex];
                for(int rule=0;rule<state.floats_per_vertex;rule++){
                    if(state.interp_rules[rule]==interp_type::flat){
                        df->data[rule]=v0.data[rule];
                        
                    }
                    else if(state.interp_rules[rule]==interp_type::smooth){
                        float k = alpha/v0.gl_Position[3]+beta/v1.gl_Position[3]+gamma/v2.gl_Position[3];
                        df->data[rule]=alpha/v0.gl_Position[3]*v0.data[rule]+beta/v1.gl_Position[3]*v1.data[rule]+gamma*v2.data[rule]/v2.gl_Position[3];
                        df->data[rule]=df->data[rule]/k;
                    }
                    else if(state.interp_rules[rule]==interp_type::noperspective){
                        df->data[rule]=alpha*v0.data[rule]+beta*v1.data[rule]+gamma*v2.data[rule];
                    }
                }
                
                data_output * o= new data_output;
                state.fragment_shader(df[0],*o,state.uniform_data);
                pixel color=make_pixel(255*o->output_color[0],255* o->output_color[1],255* o->output_color[2]) ;
                if(state.image_depth[j*state.image_width+i]<alpha*az + beta*bz + gamma*cz){
                    
                }
                else{
                    state.image_depth[j*state.image_width+i]=alpha*az + beta*bz +gamma*cz;
                    state.image_color[j*state.image_width+i]=color;
                }
                
            }
        }
    }
}

