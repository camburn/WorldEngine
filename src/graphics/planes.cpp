#include "graphics/planes.hpp"

/*
I want two planes

colored plane

textured plane

Textures should be able to use a alpha component for transparency

1--2
| /|
|/ |
3--4

*/

// A plane is 4 vertices or 2 triangles
// How should I do my textures?
// A) Large texture and use coords to pick my textures... means 
// a single texture bind and single plane bind, can then draw as many sprites 
// as are required.

// This does make building our texture more complex though and is better suited 
// to smaller planes, larger planes still require a high resolution image and 
// will leave no room for smaller textures

float Z = -2.0;
Mesh plane_mesh(256);
//struct Vertex {
//    glm::vec3 Position;
//    glm::vec3 Normal;
//    glm::vec2 TexCoord;
//};

//vector<vec3> vertices;
//vector<vec3> normals;
//vector<vec2> tex_coords;



/*
How should I do positions and updates?
These are for 2d (and 3d?).
Could supply another buffer of model matrices (4x4 - 64bytes)
(model matrices represent pos rotation and scale)
Or a buffer of worldspace positions (then I would have to transform on the GPU)
*/

void InitPlanes() {
    plane_mesh.Init();
}

void DrawPlanes(GLuint shader) {
    // Draw all planes currently in the array
    plane_mesh.Draw(shader);
}

int UpdatePlaneBuffers(vec2 pos, float width, float height) {
    // This should be called when we have new planes that need to be packed 
    // into our array
    // Update whole thing or just the difference.

    vector<Vertex> vertex_data;
    vector<GLuint> index_data;
    CreatePlane(pos, width, height, vertex_data, index_data, plane_mesh.VertexCount());
    plane_mesh.AppendData(vertex_data, index_data);

    return 0;
}

void CreatePlane(vec2 pos, float width, float height, 
    vector<Vertex>& vertex_data, vector<GLuint>& index_data, int start_index) {

    Vertex v1, v2, v3, v4 = Vertex();

    vec3 p1 = vec3(pos, Z);
    vec3 p2 = vec3(pos + vec2(0, height), Z);
    vec3 p3 = vec3(pos + vec2(width, 0), Z);
    vec3 p4 = vec3(pos + vec2(width, height), Z);

    v1.Position = p1;
    v2.Position = p2;
    v3.Position = p3;
    v4.Position = p4;

    GLuint index1 = start_index;
    GLuint index2 = 1 + start_index;
    GLuint index3 = 2 + start_index;
    GLuint index4 = 3 + start_index;

    // Triangle 1
    index_data.push_back(index3);
    index_data.push_back(index2);
    index_data.push_back(index1);
    vec3 normalA = glm::triangleNormal(p3, p2, p1);

    // Triangle 2
    index_data.push_back(index3);
    index_data.push_back(index4);
    index_data.push_back(index2);
    vec3 normalB = glm::triangleNormal(p2, p4, p3);
    
    // Should average normal A and B
    v1.Normal = normalA;
    v2.Normal = normalA;
    v3.Normal = normalA;
    v4.Normal = normalA;

    v1.TexCoord = vec2(0.0, 0.0);
    v2.TexCoord = vec2(0.0, 1.0);
    v3.TexCoord = vec2(1.0, 0.0);
    v4.TexCoord = vec2(1.0, 1.0);

    vertex_data.push_back(v1);
    vertex_data.push_back(v2);
    vertex_data.push_back(v3);
    vertex_data.push_back(v4);
}

void createTexturedPlane() {

}
