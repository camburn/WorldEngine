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
    glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_LINES, &line_data[0], GL_DYNAMIC_DRAW);
    // Specify the vertex position attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
}

void UpdateBuffers() {
    // This updates the buffers with new data (how do I resize?)
    glBindBuffer(GL_ARRAY_BUFFER, DebugLinesVBOID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_DEBUG_LINES, &line_data[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DebugCreateLine(vec3 point1, vec3 point2) {
    // TODO: need to delete lines
    line_data.push_back(point1);
    line_data.push_back(point2);
    //glUseProgram(debug_program);
    //BufferData();
    //UpdateBuffers();
    
}

void DebugInit() {
    // Create our debug shaders and program
    debug_program = BuildGlProgram("./src/shaders/simple_vertex_shader.glsl",
                                   "./src/shaders/simple_fragment_shader.glsl");
    DebugCreateLine(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 5.0f, 0.0f));
    DebugCreateLine(vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 5.0f, 0.0f));
    DebugCreateLine(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 5.0f, 1.0f));
    DebugCreateLine(vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 5.0f, 1.0f));
    DebugCreateLine(vec3(2.0f, 0.0f, -4.0f), vec3(2.0f, 5.0f, -4.0f));

    glUseProgram(debug_program);
    BufferData();
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
    glDrawArrays(GL_LINES, 0, line_data.size());
    glBindVertexArray(0);
}

void DebugDraw(mat4 Projection, mat4 View) {
    DrawLines(Projection, View);
}