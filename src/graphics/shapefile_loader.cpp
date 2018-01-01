#include "graphics/shapefile_loader.hpp"

#define PI 3.14159265

GLuint shapefile_buffer;
GLuint indice_count;
GLuint vertice_count;
GLuint shapefile_buffer2;
GLuint vertice_count2;
GLuint EBO;


float RadianToDegrees(float radians) {
    return radians * 180 / PI;
}


float PointsAngle(glm::vec2 p1, glm::vec2 center, glm::vec2 p2) {
    glm::vec2 edge_a_vector = glm::vec2(p1.x - center.x, p1.y - center.y);
    glm::vec2 edge_b_vector = glm::vec2(center.x - p2.x, center.y - p2.y);

    edge_a_vector = glm::normalize(edge_a_vector);
    edge_b_vector = glm::normalize(edge_b_vector);
    glm::vec2 normal = edge_a_vector + edge_b_vector;
    normal = glm::normalize(normal);

    return (std::atan2(normal.x, normal.y) + PI * 0.5);
}


void LoadPolygon(SHPHandle handle) {
    std::vector<glm::vec3> vertices;
    int num_entities;
    SHPGetInfo(handle, &num_entities, 0, 0, 0);
    for (int i = 0; i < num_entities; i++) {
        std::vector<p2t::Point*> polygon;
        SHPObject *shp_data;
        shp_data = SHPReadObject(handle, i);

        std::vector<glm::vec3> point_array;
        std::vector<int> start_indices;
        std::vector<int> end_indices;

        if (shp_data != NULL) {
            for (int j = 0; j < shp_data->nVertices; j++) {
                polygon.push_back(new p2t::Point(
                    shp_data->padfX[j],
                    shp_data->padfY[j]
                ));
            }
        }
        // Need to remove the last vertex since it is the same as the first
        polygon.pop_back();
        // Triangulate the vertices here
        // TODO: Check for duplicate vertices, p2t requires no duplicates
        p2t::CDT* cdt = new p2t::CDT(polygon);
        // TODO: Need to add any inner polygons to p2t
        cdt->Triangulate();
        vector<p2t::Triangle*> shape_tris;
        list<p2t::Triangle*> map;
        shape_tris = cdt->GetTriangles();
        map = cdt->GetMap();
        for (uint i = 0; i < shape_tris.size(); i++) {
            p2t::Triangle t0 = *shape_tris[i];
            p2t::Point p0 = *t0.GetPoint(0);
            p2t::Point p1 = *t0.GetPoint(1);
            p2t::Point p2 = *t0.GetPoint(2);
            vertices.push_back(glm::vec3(p0.x, p0.y, 0.0f)); // VERTEX A
            vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // COLOR
            vertices.push_back(glm::vec3(p1.x, p1.y, 0.0f)); // VERTEX B
            vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // COLOR
            vertices.push_back(glm::vec3(p2.x, p2.y, 0.0f)); // VERTEX C
            vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // COLOR
        }
    }
    shapefile_buffer = BufferMeshDataV(vertices);
    vertice_count = vertices.size() / 2;
}


typedef struct {
    glm::vec2 v1;
    glm::vec2 v2;
} segment;

segment CalculateLinePoints(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, float thickness) {
    glm::vec2 line = p1 - p0;
    glm::vec2 normal = glm::normalize(glm::vec2(-line.y, line.x));

    glm::vec2 tangent = glm::normalize(glm::normalize(p2 - p1) + glm::normalize(p1 - p0));
    glm::vec2 miter = glm::vec2( -tangent.y, tangent.x);
    float length = thickness / glm::dot(miter, normal);

    segment result;
    result.v1 = p1 - length * miter;
    result.v2 = p1 + length * miter;
    return result;
}


void LoadLine(SHPHandle handle, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), float thickness = 0.01f) {
    std::vector<glm::vec3> vertices;
    int num_entities;
    SHPGetInfo(handle, &num_entities, 0, 0, 0);
    for (int i = 0; i < num_entities; i++) {
        std::vector<glm::vec2> polygon;
        SHPObject *shp_data;
        shp_data = SHPReadObject(handle, i);

        std::vector<glm::vec3> point_array;
        std::vector<int> start_indices;
        std::vector<int> end_indices;

        if (shp_data != NULL) {
            for (int j = 0; j < shp_data->nVertices; j++) {
                polygon.push_back(glm::vec2(
                    shp_data->padfX[j],
                    shp_data->padfY[j]
                ));
            }
        }
        polygon.pop_back();
        uint previous_index = polygon.size() - 1;
        uint current_index = 0;
        for (; current_index < polygon.size(); current_index++) {
            // Codes here
            uint next_index = current_index + 1;
            if (next_index >= polygon.size()) {
                next_index = next_index - polygon.size();
            }
            uint next_index_2 = current_index + 2;
            if (next_index_2 >= polygon.size()) {
                next_index_2 = next_index_2 - polygon.size();
            }
            
            glm::vec2 p0 = polygon[previous_index];
            glm::vec2 p1 = polygon[current_index];
            glm::vec2 p2 = polygon[next_index];
            glm::vec2 p3 = polygon[next_index_2];

            segment segment_a = CalculateLinePoints(p0, p1, p2, thickness);
            segment segment_b = CalculateLinePoints(p1, p2, p3, thickness);

            vertices.push_back(glm::vec3(segment_a.v1.x, segment_a.v1.y, 0.0f)); // TRI 1 VERTEX A
            vertices.push_back(color); // COLOR
            vertices.push_back(glm::vec3(segment_b.v1.x, segment_b.v1.y, 0.0f)); // TRI 1 VERTEX A
            vertices.push_back(color); // COLOR
            vertices.push_back(glm::vec3(segment_a.v2.x, segment_a.v2.y, 0.0f)); // TRI 1 VERTEX A
            vertices.push_back(color); // COLOR

            vertices.push_back(glm::vec3(segment_b.v1.x, segment_b.v1.y, 0.0f)); // TRI 1 VERTEX A
            vertices.push_back(color); // COLOR
            vertices.push_back(glm::vec3(segment_b.v2.x, segment_b.v2.y, 0.0f)); // TRI 1 VERTEX A
            vertices.push_back(color); // COLOR
            vertices.push_back(glm::vec3(segment_a.v2.x, segment_a.v2.y, 0.0f)); // TRI 1 VERTEX A
            vertices.push_back(color); // COLOR
            previous_index = current_index;
        }
    }
    shapefile_buffer2 = BufferMeshDataV(vertices);
    vertice_count2 = vertices.size() / 2;
}


void LoadShapeFile() {
    char* file_name = "/home/campbell/Examples/GLRenderSHP/Sample/Polygon";
    //char* file_name = "/home/campbell/Projects/WorldEngine/assets/shapefiles/star.shp";
    SHPHandle test_shape = SHPOpen(file_name, "rb");

    // TODO: There is no vertice reuse, they should be converted to use a 
    // element array
    if (test_shape == NULL) {
        printf("WARNING::SHAPEFILE:: Empty Shapefile %s\n", file_name);
    }

    if (test_shape->nShapeType == SHPT_POINT) {
        printf("INFO::SHAPEFILE:: Shapefile is Points %s\n", file_name);
    }

    if (test_shape->nShapeType == SHPT_ARC || true) {
        printf("INFO::SHAPEFILE:: Shapefile is Arc %s\n", file_name);
        LoadLine(test_shape);
    }

    if (test_shape->nShapeType == SHPT_POLYGON) {
        printf("INFO::SHAPEFILE:: Shapefile is Polygon %s\n", file_name);
        //LoadPolygon(test_shape);
    }

    //SHPObject *test_shape_data;
    //SHPDestroyObject(test_shape_data);

    printf("Shape Loaded\n");
}


void draw_shapes() {
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBindVertexArray(shapefile_buffer2);
    glDrawArrays(GL_TRIANGLES, 0, vertice_count2);

    glBindVertexArray(shapefile_buffer);
    glDrawArrays(GL_TRIANGLES, 0, vertice_count);

    //glDrawElements(GL_TRIANGLES, indice_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}