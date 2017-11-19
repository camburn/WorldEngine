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
SpriteMesh static_plane_mesh(256, false, "static");
GLuint tex_id;

struct TexRect {
    float u;
    float v;
    float w;
    float h;
};

map<string, TexRect> texture_map;
map<string, int> plane_positions;
vector<glm::mat4> model_matrices;
vector<int> update_list;

void InitPlanes(GLuint sprite_program) {
    static_plane_mesh.Init(sprite_program);
    SetTextureMap("quad_texture.png");
    glm::mat4 m = mat4(1.0f);
    UpdatePlaneMatrix("default", m);

    //plane_positions["player"] = 0;
    //plane_positions["player2"] = 1;
    
    //model_matrices.push_back(m);
    //model_matrices.push_back(m);
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

glm::mat4 GetPlaneMatrix(string name) {
    int matrix_index = plane_positions[name];
    return model_matrices[matrix_index];
}

int UpdatePlaneMatrix(string name, mat4 model_matrix) {
    int matrix_index;
    if (plane_positions.count(name) == 1) {
        // Key exists, just update the existing matrix
        matrix_index = plane_positions[name];
        model_matrices[matrix_index] = model_matrix;
    } else {
        // Does not exist, push in a new element
        matrix_index = model_matrices.size();
        plane_positions[name] = matrix_index;
        model_matrices.push_back(model_matrix);
    }
    update_list.push_back(matrix_index);
    return matrix_index;
}

void UpdateMatrixBuffer() {
    // Test moving matrix
    //float x = (float)glm::sin(glfwGetTime());// * 0.01;
    //mat4 m = model_matrices[plane_positions["player"]];
    //mat4 m2 = model_matrices[plane_positions["player2"]];
    //m = glm::translate(m, vec3(0, x, 0));

    while (!update_list.empty()) {
        int matrix_index = update_list.back();
        update_list.pop_back();
        static_plane_mesh.UpdateModelMatrix(matrix_index * 64, model_matrices[matrix_index]);
    }
    
    //static_plane_mesh.UpdateModelMatrix(0, m);
    //static_plane_mesh.UpdateModelMatrix(sizeof(m), m2);
}

void DrawPlanes(GLuint shader) {
    // Draw all planes currently in the array
    glBindTexture(GL_TEXTURE_2D, tex_id);
    static_plane_mesh.Draw(shader);
}


int UpdatePlaneBuffers(vec2 pos, float width, float height, string texture, int model_loc) {
    // This should be called when we have new planes that need to be packed 
    // into our array
    // Update whole thing or just the difference.
    vector<SpriteVertex> vertex_data;
    vector<GLuint> index_data;
    CreatePlane(pos, width, height, vertex_data, index_data, 
                static_plane_mesh.VertexCount(), texture_map[texture], model_loc);
    static_plane_mesh.AppendData(vertex_data, index_data);

    return 0;
}

void CreatePlane(vec2 pos, float width, float height, 
    vector<SpriteVertex>& vertex_data, vector<GLuint>& index_data, 
    int start_index, TexRect texture, int model_loc) {

    SpriteVertex v1, v2, v3, v4 = SpriteVertex();

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

    v1.TexCoord = vec2(texture.u, texture.v);
    v2.TexCoord = vec2(texture.u, texture.v + texture.h);
    v3.TexCoord = vec2(texture.u + texture.w, texture.v);
    v4.TexCoord = vec2(texture.u + texture.w, texture.v + texture.h);

    v1.ModelLoc = model_loc;
    v2.ModelLoc = model_loc;
    v3.ModelLoc = model_loc;
    v4.ModelLoc = model_loc;

    vertex_data.push_back(v1);
    vertex_data.push_back(v2);
    vertex_data.push_back(v3);
    vertex_data.push_back(v4);
}

void createTexturedPlane() {

}
