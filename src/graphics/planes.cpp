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

float Z = 1.0;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;
};
vector<Vertex> vertex_dat;

vector<vec3> vertices;
vector<vec3> normals;
vector<vec2> tex_coords;

vector<uint> indices;

/*
How should I do positions and updates?
These are for 2d (and 3d?).
Could supply another buffer of model matrices (4x4 - 64bytes)
(model matrices represent pos rotation and scale)
Or a buffer of worldspace positions (then I would have to transform on the GPU)
*/

void DrawPlanes() {
    // Draw all planes currently in the array
}

int UpdatePlaneBuffers() {
    // This should be called when we have new planes that need to be packed 
    // into our array
    // Update whole thing or just the difference.

    if (vertices.size() != normals.size() || 
        vertices.size() != tex_coords.size()) {
        printf("Warning plane buffers are misaligned\n");
        return 0;
    }
    int plane_length = vertices.size() * 3 + normals.size() * 3 + tex_coords.size() * 2;
    float plane_data[plane_length];

    for (int i = 0; i < vertices.size(); i++;) {
        plane_data
    }
    //BufferMeshDataVNT()
    return 1;
}

void CreatePlane(vec2 pos, float width, float height) {
    vec3 p1 = vec3(pos, Z);
    vec3 p2 = vec3(pos + vec2(0, height), Z);
    vec3 p3 = vec3(pos + vec2(width, 0), Z);
    vec3 p4 = vec3(pos + vec2(width, height), Z);

    uint index1 = vertices.size();
    vertices.push_back(p1);
    uint index2 = vertices.size();
    vertices.push_back(p2);
    uint index3 = vertices.size();
    vertices.push_back(p3);
    uint index4 = vertices.size();
    vertices.push_back(p4);

    // Triangle 1
    indices.push_back(index1);
    indices.push_back(index2);
    indices.push_back(index3);
    vec3 normalA = glm::triangleNormal(p1, p2, p3);

    // Triangle 2
    indices.push_back(index2);
    indices.push_back(index4);
    indices.push_back(index3);
    vec3 normalB = glm::triangleNormal(p2, p4, p3);
    
    normals.push_back(normalA);
    normals.push_back(normalB);

    tex_coords.push_back(vec2(0.0, 0.0));
    tex_coords.push_back(vec2(0.0, 1.0));
    tex_coords.push_back(vec2(1.0, 1.0));
    tex_coords.push_back(vec2(1.0, 0.0));
}

void createTexturedPlane() {

}
