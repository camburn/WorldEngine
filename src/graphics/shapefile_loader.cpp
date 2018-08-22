#include "graphics/shapefile_loader.hpp"

#define PI 3.14159265

GLuint shapefile_buffer;
GLuint indice_count;
GLuint vertice_count;
GLuint shapefile_buffer2;
GLuint vertice_count2;
GLuint EBO;


/* -- EXAMPLE DRAWING -- 

// In Setup
LoadShapeFile("./assets/shapefiles/Australia.shp");

// In Main Loop
// ========= SHAPE DRAWING =========
{
    glUseProgram(simple_program);
    glUniform1i(glGetUniformLocation(simple_program, "use_uniform_color"), false);
    glm::mat4 plane_model = glm::mat4(1.0f);
    rotated_view = getView();

    glm::mat4 mvp = Projection * rotated_view * plane_model;
    // Load camera to OpenGL
    renderer.active().set_uniform("MVP", mvp);

    draw_shapes();
}
// ========= END SHAPE DRAWING =========
frame_section("Shape Drawing", glfwGetTime());

*/


template<typename Iter>
Iter removeDuplicates(Iter begin,Iter end)
{
    //polygon.erase(removeDuplicates(polygon.begin(),polygon.end()),polygon.end());
    auto it = begin;
    while(it != end)
    {
        auto next = std::next(it);
        if(next == end)
        {
            break;
        }
        end = std::remove(next,end,*it);
        it = next;
    }

    return end;
}


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

        SHPObject *shp_data;
        shp_data = SHPReadObject(handle, i);
        if (shp_data == NULL) {
            return;
        }

        int num_parts = shp_data->nParts;
        int part_start;
        int part_end;
        for (int part = 0; part < num_parts; part++) {

            Vector2dVector polygon;
            // For each part
            part_start = shp_data->panPartStart[part];
            if (part == num_parts-1) {
                // Last part, last index is size-1
                part_end = shp_data->nVertices -1;
            } else {
                part_end = shp_data->panPartStart[part+1];
            }

            for (int j = part_start; j < part_end; j++) {
                polygon.push_back(Vector2d(
                    shp_data->padfX[j],
                    shp_data->padfY[j]
                ));
            }
            // Need to remove the last vertex since it is the same as the first
            polygon.pop_back();
            // Triangulate the vertices here
            Vector2dVector result;
            Triangulate::Process(polygon, result);
            int tcount = result.size()/3;

            for (int i=0; i<tcount; i++) {
                const Vector2d &p1 = result[i*3+0];
                const Vector2d &p2 = result[i*3+1];
                const Vector2d &p3 = result[i*3+2];

                vertices.push_back(glm::vec3(p1.GetX(), p1.GetY(), 0.0f)); // VERTEX A
                vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // COLOR
                vertices.push_back(glm::vec3(p2.GetX(), p2.GetY(), 0.0f)); // VERTEX B
                vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // COLOR
                vertices.push_back(glm::vec3(p3.GetX(), p3.GetY(), 0.0f)); // VERTEX C
                vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // COLOR
            }
        }
    }
    shapefile_buffer = BufferMeshDataV(vertices);
    vertice_count = vertices.size() / 2;
}


struct segment {
    glm::vec2 v1;
    glm::vec2 v2;
};

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


class Polygon {
private:

public:
    std::vector<glm::vec2> vertices;
};


Polygon create_polygon(SHPObject *shp_data) {
    Polygon p{};
    return p;
}


void LoadLine(SHPHandle handle, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), float thickness = 0.01f) {
    std::vector<glm::vec3> vertices;
    int num_entities;
    SHPGetInfo(handle, &num_entities, 0, 0, 0);
    for (int i = 0; i < num_entities; i++) {
        
        SHPObject *shp_data;
        shp_data = SHPReadObject(handle, i);
        if (shp_data == NULL) {
            return;
        }

        int num_parts = shp_data->nParts;
        int part_start;
        int part_end;
        for (int part = 0; part < num_parts; part++) {

            std::vector<glm::vec2> polygon;
            // For each part
            part_start = shp_data->panPartStart[part];
            if (part == num_parts-1) {
                // Last part, last index is size-1
                part_end = shp_data->nVertices -1;
            } else {
                part_end = shp_data->panPartStart[part+1];
            }

            // Iterate the range of the shape
            for (int j = part_start; j < part_end; j++) {
                polygon.push_back(glm::vec2(
                    shp_data->padfX[j],
                    shp_data->padfY[j]
                ));
            }
            // Remove the final vertex (same as the first)
            polygon.pop_back();

            unsigned int previous_index = polygon.size() - 1;
            unsigned int current_index = 0;
            for (; current_index < polygon.size(); current_index++) {
                // Codes here
                unsigned int next_index = current_index + 1;
                if (next_index >= polygon.size()) {
                    next_index = next_index - polygon.size();
                }
                unsigned int next_index_2 = current_index + 2;
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
    }
    shapefile_buffer2 = BufferMeshDataV(vertices);
    vertice_count2 = vertices.size() / 2;
}


void LoadShapeFile(const std::string file_name) {
    //char* file_name = "./assets/shapefiles/TM_WORLD_BORDERS-0.3.shp";
    //char* file_name = "./assets/shapefiles/Australia.shp";
    //char* file_name = "/home/campbell/Projects/WorldEngine/assets/shapefiles/star.shp";
    SHPHandle test_shape = SHPOpen(file_name.c_str(), "rb");

    // TODO: There is no vertice reuse, they should be converted to use a 
    // element array
    if (test_shape == NULL) {
        std::cout << "WARNING::SHAPEFILE:: Empty Shapefile" << file_name << std::endl;
        exit(1);
    }

    if (test_shape->nShapeType == SHPT_POINT) {
        std::cout << "INFO::SHAPEFILE:: Shapefile is Points" << file_name << std::endl;
    }

    if (test_shape->nShapeType == SHPT_ARC || true) {
        std::cout << "INFO::SHAPEFILE:: Shapefile is Arc" << file_name << std::endl;
        LoadLine(test_shape);
    }

    if (test_shape->nShapeType == SHPT_POLYGON) {
        std::cout << "INFO::SHAPEFILE:: Shapefile is Polygon" << file_name << std::endl;
        LoadPolygon(test_shape);
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