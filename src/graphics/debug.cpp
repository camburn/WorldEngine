#include "graphics/debug.hpp"
/*
Ideally we have the mesh data buffered in by the init

The draw method can calculate the correct translation based on the supplied rect 
*/

// A single line is sizeof(GLfloat) * 12
const GLuint MAX_DEBUG_LINES = 65536;

GLfloat line[] = {
    5.0f, 2.0f, 5.0f,
    -5.0f, 2.0f, -5.0f
};

GLint lineCount = 3;

GLuint MVPMatID;
GLuint DebugLinesVAOID;
GLuint DebugLinesVBOID;
GLuint debug_program;
GLuint offset = 0;
GLuint totalLines = 0;

std::vector<vec3> line_data;

std::map<std::string, bool> flags = {
    { "render:draw_normals", false },
    { "render:draw_texcoords", false },
    { "render:disable_lighting", false }
};

void BufferData() {
    // This creates the buffers and sends the debug data to the GPU
    // Create our VAO
    glGenVertexArrays(1, &DebugLinesVAOID);
    glBindVertexArray(DebugLinesVAOID);
    // Buffer data to VBO
    glGenBuffers(1, &DebugLinesVBOID);
    glBindBuffer(GL_ARRAY_BUFFER, DebugLinesVBOID);
    glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_LINES, 0, GL_DYNAMIC_DRAW);
    // Specify the vertex position attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

void UpdateBuffers() {
    // This updates the buffers with new data (how do I resize?)
    glBindBuffer(GL_ARRAY_BUFFER, DebugLinesVBOID);
    // TODO: We are rebuffering everything, this should only add in the new data
    totalLines += (line_data.size() / 2);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(GLfloat) * (3 * line_data.size()), &line_data[0]);
    offset += sizeof(GLfloat) * (3 * line_data.size());
    line_data.clear();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DebugCreateLine(float x1, float y1, float z1, float x2, float y2, float z2) {
    DebugCreateLine(vec3(x1, y1, z1), vec3(x2, y2, z2), vec3(1, 1, 1));
}

void DebugCreateLine(float x1, float y1, float z1, float x2, float y2, float z2, float r, float g, float b) {
    DebugCreateLine(vec3(x1, y1, z1), vec3(x2, y2, z2), vec3(r, g, b));
}

void DebugCreateLine(vec3 point1, vec3 point2, vec3 color) {
    // TODO: need to delete lines
    line_data.push_back(point1);
    line_data.push_back(color);
    line_data.push_back(point2);
    line_data.push_back(color);
    UpdateBuffers();
}

void SendData(vec3 a, vec3 b, vec3 color) {
    line_data.push_back(a);
    line_data.push_back(color);
    line_data.push_back(b);
    line_data.push_back(color);
}

void DebugCreateCube(float x, float y, float z, float width, float height, float depth) {
    // Create a debug cube with a default white colour
    DebugCreateCube(vec3(x, y, z), width, height, depth, vec3(1, 1, 1));
}

void DebugCreateCube(float x, float y, float z, float width, float height, float depth, float r, float g, float b) {
    //Create a debug cube with a specified colour
    DebugCreateCube(vec3(x, y, z), width, height, depth, vec3(r, g, b));
}

void DebugCreateCube(vec3 point1, float width, float height, float depth, vec3 color) {
    // We need to generate all the vertices
    // For any real vertice we need to create 3 lines
    vec3 point2 = point1 + vec3(width, 0, depth);
    vec3 point3 = point1 + vec3(0, height, depth);
    vec3 point4 = point1 + vec3(width, height, 0);
     
    SendData(point1, point1 + vec3(width, 0, 0), color);
    SendData(point1, point1 + vec3(0, height, 0), color);
    SendData(point1, point1 + vec3(0, 0, depth), color);

    SendData(point2, point2 + vec3(-width, 0, 0), color);
    SendData(point2, point2 + vec3(0, height, 0), color);
    SendData(point2, point2 + vec3(0, 0, -depth), color);

    SendData(point3, point3 + vec3(width, 0, 0), color);
    SendData(point3, point3 + vec3(0, -height, 0), color);
    SendData(point3, point3 + vec3(0, 0, -depth), color);

    SendData(point4, point4 + vec3(-width, 0, 0), color);
    SendData(point4, point4 + vec3(0, -height, 0), color);
    SendData(point4, point4 + vec3(0, 0, depth), color);
    UpdateBuffers();
}

void DebugInit() {
    // Create our debug shaders and program
    debug_program = opengl::BuildGlProgram("./src/shaders/simple_vertex_shader.glsl",
                                           "./src/shaders/simple_fragment_shader.glsl");
    glUseProgram(debug_program);
    BufferData();
    DebugCreateLine(vec3(0.0f, 0.0f, 0.0f), vec3(3.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f));
    DebugCreateLine(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 3.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    DebugCreateLine(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 3.0f), vec3(0.25f, 0.25f, 1.0f));
    DebugCreateCube(vec3(1.0f, 0.0f, 1.0f), 1.0f, 1.0f, 1.0f, vec3(0.75f, 0.75f, 0.0f));
    MVPMatID = glGetUniformLocation(debug_program, "MVP");
}

void DebugClear() {
    //Resets the debug buffers
    offset = 0;
    totalLines = 0;
}

void DrawLines(mat4 Projection, mat4 View) {
    glUseProgram(debug_program);
    mat4 model = mat4(1.0f);
    mat4 mvp = Projection * View * model;
    // mat3 normalMat = (mat3)transpose(inverse(model));
    glUniformMatrix4fv(MVPMatID, 1, GL_FALSE, &mvp[0][0]);
    glBindVertexArray(DebugLinesVAOID);
    glDrawArrays(GL_LINES, 0, totalLines);
    glBindVertexArray(0);
}

void DebugDraw(mat4 Projection, mat4 View) {
    DrawLines(Projection, View);
}

std::string DebugFlagToggle(const char* flag) {
    std::string result = flag;
    if (flags.count(flag)) {
        if (flags[flag]) {
            flags[flag] = false;
            result += ": Disabled";
            return result;
        }
        else {
            flags[flag] = true;
            result += ": Enabled";
            return result;
        }
    }
    result += ": Flag does not exist";
    return result;
}

std::string DebugFlagList() {
    std::string result = "";
    for (auto const& x : flags) {
        result += x.first;
        result += ": ";
        result += x.second ? "true" : "false";
        result += "\n";
    }
    return result;
}

bool DebugGetFlag(std::string flag) {
    if (flags.count(flag)) {
        return flags[flag];
    }
    return false;
}
