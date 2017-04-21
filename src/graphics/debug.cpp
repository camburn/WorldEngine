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

GLint lineCount = 0;

GLuint DebugLinesVAOID;
GLuint DebugLinesVBOID;

void BufferData(GLfloat *mesh_data) {
    // This creates the buffers and sends the debug data to the GPU

    // Create our VAO
    GLuint DebugLinesVAOID;
    glGenVertexArrays(1, &DebugLinesVAOID);
    glBindVertexArray(DebugLinesVAOID);

    // Buffer data to VBO
    GLuint DebugLinesVBOID;
    glGenBuffers(1, &DebugLinesVBOID);
    glBindBuffer(GL_ARRAY_BUFFER, DebugLinesVBOID);
    glBufferData(GL_ARRAY_BUFFER, MAX_DEBUG_LINES, mesh_data, GL_STATIC_DRAW);
    // Specify the vertex position attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
}

void UpdateBuffers(GLfloat *mesh_data) {
    // This updates the buffers with new data (how do I resize?)
    glBindBuffer(GL_ARRAY_BUFFER, DebugLinesVBOID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_DEBUG_LINES, mesh_data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CreateLine(vec3 point1, vec3 point2) {

}

void DebugInit() {
    // Create our debug shaders and program

}

void DrawBox() {
    // Code for drawing a box

}

void DrawLine() {
    glBindVertexArray(DebugLinesVAOID);
    glDrawArrays(GL_LINES, 0, lineCount);
    glBindVertexArray(0);
}