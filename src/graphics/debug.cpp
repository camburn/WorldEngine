#include "graphics/debug.hpp"
/*
Ideally we have the mesh data buffered in by the init

The draw method can calculate the correct translation based on the supplied rect 
*/

const GLuint MAX_DEBUG_LINES = 64 * (2 * sizeof(vec3));

GLfloat line[] = {
    5.0f, 2.0f, 5.0f,
    -5.0f, 2.0f, -5.0f
};

GLint lineCount = 3;

GLuint MVPMatID;
GLuint DebugLinesVAOID;
GLuint DebugLinesVBOID;
GLuint debug_program;

std::vector<vec3> line_data;

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
    glDisableVertexAttribArray(0);
}

void UpdateBuffers() {
    // This updates the buffers with new data (how do I resize?)
    glBindBuffer(GL_ARRAY_BUFFER, DebugLinesVBOID);
    // TODO: We are rebuffering everything, this should only add in the new data
    glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_DEBUG_LINES, &line_data[0]);
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

void DebugInit() {
    // Create our debug shaders and program
    debug_program = BuildGlProgram("./src/shaders/simple_vertex_shader.glsl",
                                   "./src/shaders/simple_fragment_shader.glsl");
    glUseProgram(debug_program);
    BufferData();
    DebugCreateLine(vec3(0.0f, 0.0f, 0.0f), vec3(3.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f));
    DebugCreateLine(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 3.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    DebugCreateLine(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 3.0f), vec3(0.25f, 0.25f, 1.0f));
    MVPMatID = glGetUniformLocation(debug_program, "MVP");
}

void DrawBox() {
    // Code for drawing a box

}

void DrawLines(mat4 Projection, mat4 View) {
    glUseProgram(debug_program);
    mat4 model = mat4(1.0f);
    mat4 mvp = Projection * View * model;
    mat3 normalMat = (mat3)transpose(inverse(model));
    glUniformMatrix4fv(MVPMatID, 1, GL_FALSE, &mvp[0][0]);
    glBindVertexArray(DebugLinesVAOID);
    glDrawArrays(GL_LINES, 0, line_data.size() / 2);
    glBindVertexArray(0);
}

void DebugDraw(mat4 Projection, mat4 View) {
    DrawLines(Projection, View);
}