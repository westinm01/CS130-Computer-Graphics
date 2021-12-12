varying vec4 position;
varying vec3 normal;
varying vec4 world_position;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    normal = normalize(gl_NormalMatrix * gl_Normal);
    world_position = gl_ModelViewMatrix * gl_Vertex;
}
