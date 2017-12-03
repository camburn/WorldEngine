#include "graphics/planes.hpp"

float Z = -2.0;
SpriteMesh static_plane_mesh(256, false, "static");
GLuint tex_id;

struct TexRect {
    float u;
    float v;
    float w;
    float h;
};

struct Plane {
    float w;
    float h;
    glm::vec2 pos;
    string tex_name;
    int model_index;
    int start_bytes;
};

map<string, TexRect> texture_map;
map<string, int> plane_positions;
vector<glm::mat4> model_matrices;
vector<int> update_list;
vector<Plane> plane_list;


void MatrixWidget(glm::mat4 matrix) {
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(matrix, scale, rotation, translation, skew, perspective);
    glm::vec3 eular_rot = glm::eulerAngles(rotation);

    ImGui::Columns(4, "mycolumns"); // 4-ways, with border
    ImGui::Separator();
    ImGui::Text("Type"); ImGui::NextColumn();
    ImGui::Text("X"); ImGui::NextColumn();
    ImGui::Text("Y"); ImGui::NextColumn();
    ImGui::Text("Z"); ImGui::NextColumn();
    ImGui::Separator();

    ImGui::Text("Translation"); ImGui::NextColumn();
    ImGui::Text(to_string(translation.x).c_str()); ImGui::NextColumn();
    ImGui::Text(to_string(translation.y).c_str()); ImGui::NextColumn();
    ImGui::Text(to_string(translation.z).c_str()); ImGui::NextColumn();

    ImGui::Text("Scale"); ImGui::NextColumn();
    ImGui::Text(to_string(scale.x).c_str()); ImGui::NextColumn();
    ImGui::Text(to_string(scale.y).c_str()); ImGui::NextColumn();
    ImGui::Text(to_string(scale.z).c_str()); ImGui::NextColumn();

    ImGui::Text("Rotation"); ImGui::NextColumn();
    ImGui::Text(to_string(eular_rot.x).c_str()); ImGui::NextColumn();
    ImGui::Text(to_string(eular_rot.y).c_str()); ImGui::NextColumn();
    ImGui::Text(to_string(eular_rot.z).c_str()); ImGui::NextColumn();

    ImGui::Columns(1);
    ImGui::Separator();
}

void DisplayPlaneData(bool* p_open) {
    //ImGui::SetNextWindowPos(ImVec2(50,50));
    //ImGui::SetNextWindowSize(ImVec2(430,450), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Planes", p_open))
	{
		ImGui::End();
		return;
	}
    ImGui::Text("Plane Data");
    if (ImGui::TreeNode("Planes")) {
        for (auto const &data: plane_list) {
            ImGui::Text("Texture %s; ", data.tex_name.c_str());
            ImGui::SameLine();
            ImGui::Text("w: %f, h: %f, x: %f, y: %f, m_loc: %i start_bytes: %ib", 
                data.w, data.h, data.pos.x, data.pos.y, data.model_index, data.start_bytes);
        }
        ImGui::Text("Planes");
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Textures")) {
        for (auto const &data: texture_map) {
            ImGui::Text("Name %s; ", data.first.c_str());
            ImGui::SameLine();
            ImGui::Text("u: %f, v: %f, w: %f, h: %f", 
                data.second.u, data.second.v, data.second.w, data.second.h);
        }
        ImGui::TreePop();

    }
    if (ImGui::TreeNode("Positions")) {
        for (auto const &data: plane_positions) {
            ImGui::Text("Name: %s, Index: %i", data.first.c_str(), data.second);
            MatrixWidget(model_matrices[data.second]);

        }
        ImGui::TreePop();
    }
    ImGui::End();
}

void InitPlanes(GLuint sprite_program) {
    static_plane_mesh.Init(sprite_program);
    SetTextureMap("quad_texture.png");
    glm::mat4 m = mat4(1.0f);
    UpdatePlaneMatrix("default", m);
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

void TranslatePlaneMatrix(string name, float x, float y, float z) {
    glm::mat4 model_matrix;
    int matrix_index = plane_positions[name];
    model_matrix = model_matrices[matrix_index];
    model_matrix = glm::translate(model_matrix, vec3(x, y, z));
    UpdatePlaneMatrix(name, model_matrix);
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
    while (!update_list.empty()) {
        int matrix_index = update_list.back();
        update_list.pop_back();
        static_plane_mesh.UpdateModelMatrix(matrix_index * 64, model_matrices[matrix_index]);
    }
}

void DrawPlanes(GLuint shader) {
    // Draw all planes currently in the array
    glBindTexture(GL_TEXTURE_2D, tex_id);
    static_plane_mesh.Draw(shader);
}


int UpdatePlaneBuffers(float width, float height, string texture, vec2 pos, int model_loc) {
    // This should be called when we have new planes that need to be packed 
    // into our array
    // Update whole thing or just the difference.
    vector<SpriteVertex> vertex_data;
    vector<GLuint> index_data;
    CreatePlane(pos, width, height, vertex_data, index_data, 
                static_plane_mesh.VertexCount(), texture_map[texture], model_loc);
    int start_bytes = static_plane_mesh.AppendData(vertex_data, index_data);
    Plane plane_data = {width, height, pos, texture, model_loc, start_bytes};
    plane_list.push_back(plane_data);
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
