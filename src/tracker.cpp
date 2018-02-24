#include "tracker.hpp"

class Point {
public:
    Point(float x, float y, float z);
    Point(float x, float y);
    const float X;
    const float Y;
    const float Z=0.0f;
private:
};

Point::Point(float x, float y, float z) : X(x), Y(y), Z(z) {}
Point::Point(float x, float y) : X(x), Y(y) {}

class Line {
public:
    void insert_point(Point p);
    const long uid = get_uid();
    void get_line_data(std::vector<glm::vec3> &buffer) {
        for (Point &p : points) {
            buffer.push_back(glm::vec3(p.X, p.Y, p.Z));
        }
    }
private:
    std::vector<Point> points;
};

void Line::insert_point(Point p) {
    points.push_back(p);
}

std::map<const long, Line> map_line;

const long new_line() {
    Line line{};
    map_line.emplace(line.uid, line);
    return line.uid;
}

bool insert_into_line(const long uid, float x, float y) {
    if (map_line.count(uid)) {
        map_line[uid].insert_point(Point{x, y});
        return true;
    }
    return false;
}

bool buffer_empty = true;
GLuint vertex_array_id;
GLuint vertex_buffer_id;
GLuint element_buffer_id;
int buffer_size = 0;
int element_size = 0;

void build_line_data() {
    std::vector<glm::vec3> data_buffer;
    std::vector<unsigned int> index_buffer;
    buffer_size = 0;
    for (std::pair<const long, Line> &kv: map_line) {
        kv.second.get_line_data(data_buffer);

        
    }
    buffer_size = data_buffer.size();

    data_buffer.insert(data_buffer.begin(), 2.0f * data_buffer[0] - data_buffer[1] );
    data_buffer.push_back( 2.0f * data_buffer[data_buffer.size() - 1] - data_buffer[data_buffer.size() -2]);

    for( size_t i = 1; i < data_buffer.size() - 2; ++i ) {
				index_buffer.push_back( i - 1 );
				index_buffer.push_back( i );
				index_buffer.push_back( i + 1 );
				index_buffer.push_back( i + 2 );
    }
    element_size = index_buffer.size();

    if (buffer_empty) {
        vertex_array_id = BufferMeshData(data_buffer, index_buffer, vertex_buffer_id, element_buffer_id);
        buffer_empty = false;
    } else {
        UpdateBufferData(data_buffer, index_buffer, vertex_buffer_id, element_buffer_id);
    }

}

void draw_lines() {
    glBindVertexArray(vertex_array_id);
    glDrawElements(GL_LINES_ADJACENCY, element_size, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_LINES_ADJACENCY, 0, buffer_size);
}