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
GLuint tex_id;

struct TexRect {
    float u;
    float v;
    float w;
    float h;
};

map<string, TexRect> texture_map;

void InitPlanes() {
    plane_mesh.Init();
    SetTextureMap("quad_texture.png");
}

void SetTextures(string texture_name) {
    tex_id = BufferTextureDataFromFile(texture_name.c_str(), "./assets/textures/");
}

void SetTextureMap(string texture_name) {
    texture_map.insert(pair<string, TexRect>("grass", TexRect{0.0,0.0,0.5,0.5}));
    texture_map.insert(pair<string, TexRect>("dirt", TexRect{0.5,0.0,0.5,0.5}));
    texture_map.insert(pair<string, TexRect>("sand", TexRect{0.0,0.5,0.5,0.5}));
    texture_map.insert(pair<string, TexRect>("rock", TexRect{0.5,0.5,0.5,0.5}));
    SetTextures("quad_texture.png");
}

void DrawPlanes(GLuint shader) {
    // Draw all planes currently in the array
    glBindTexture(GL_TEXTURE_2D, tex_id);
    plane_mesh.Draw(shader);
}

int UpdatePlaneBuffers(vec2 pos, float width, float height, string texture) {
    // This should be called when we have new planes that need to be packed 
    // into our array
    // Update whole thing or just the difference.

    vector<Vertex> vertex_data;
    vector<GLuint> index_data;
    CreatePlane(pos, width, height, vertex_data, index_data, 
                plane_mesh.VertexCount(), texture_map[texture]);
    plane_mesh.AppendData(vertex_data, index_data);

    return 0;
}

void CreatePlane(vec2 pos, float width, float height, 
    vector<Vertex>& vertex_data, vector<GLuint>& index_data, 
    int start_index, TexRect texture) {

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
    
    // Should average normal A and B
    v1.Normal = normalA;
    v2.Normal = normalA;
    v3.Normal = normalA;
    v4.Normal = normalA;

    v1.TexCoord = vec2(texture.u, texture.v);
    v2.TexCoord = vec2(texture.u, texture.v + texture.h);
    v3.TexCoord = vec2(texture.u + texture.w, texture.v);
    v4.TexCoord = vec2(texture.u + texture.w, texture.v + texture.h);

    vertex_data.push_back(v1);
    vertex_data.push_back(v2);
    vertex_data.push_back(v3);
    vertex_data.push_back(v4);
}

void createTexturedPlane() {

}
