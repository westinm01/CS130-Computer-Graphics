varying vec4 position;
varying vec3 normal;
varying vec4 world_position;

float pi=4.0*atan(1.0);

struct gl_LightSourceParameters
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 position;
};
uniform gl_LightSourceParameters
gl_LightSource[gl_MaxLights];
struct gl_LightModelParameters
{
  vec4 ambient;
};
uniform gl_LightModelParameters gl_LightModel;
struct gl_MaterialParameters
{
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess; // this is the exponent of the specular component
};

void main()
{
    vec4 ambient = vec4(1, 0, 0, 1);
    vec4 diffuse = vec4(0, 1, 0, 1);
    vec4 specular = vec4(0, 0, 1, 1);
    gl_FragColor = ambient + diffuse + specular;
}
