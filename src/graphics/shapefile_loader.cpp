#include "graphics/shapefile_loader.hpp"



/*
We have two options:

A) draw a indexed shape with drawarrayelements
B) create array with STOP elements (but then how do I create the rings?

0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
A, B, A, B, C, A, B, A, B, C, D;
line requires two indices
0, 1
2, 3, 3, 4, 4, 2



*/
void LoadShapefile() {
    // Testing ShapeFile loading
    SHPHandle test_shape = SHPOpen("./assets/shapefiles/polygons", "rb");
    // int ipart;
    int num_entities;
    int shapetype; // 1 - Points; 3 - Arcs (Lines); 5 - Polygons; 8 - MultiPoint (related points)
    double min_bounds[4], max_bounds[4];

    SHPGetInfo(test_shape, &num_entities, &shapetype, min_bounds, max_bounds);

    std::vector<glm::vec3> vertices;
    std::vector<int> indices;

    SHPObject *test_shape_data;
    const char 	*pszPlus;
    int last_index = 0;
    int first_index = 0;
    int part_v_count = 0;
    for (int i = 0; i < num_entities; i++) {
        test_shape_data = SHPReadObject(test_shape, i);
        if (test_shape_data != NULL) {
            for (int j = 0, iPart = 1; j < test_shape_data->nVertices; j++) {
                const char	*pszPartType = "";
                if (j == 0 && test_shape_data->nParts > 0)
                    pszPartType = SHPPartTypeName(test_shape_data->panPartType[0]);

                if (iPart < test_shape_data->nParts
                    && test_shape_data->panPartStart[iPart] == j)
                {
                    pszPartType = SHPPartTypeName(test_shape_data->panPartType[iPart]);
                    iPart++;
                    pszPlus = "+";
                }
                else
                    pszPlus = " ";
                // 0 ,1, 1, 2, 2, 3, 3, 4, 4, 0

                vertices.push_back(glm::vec3(
                    test_shape_data->padfX[j],
                    test_shape_data->padfY[j],
                    test_shape_data->padfZ[j]
                ));
                if (part_v_count > 0) {
                    indices.push_back(last_index);
                    ++last_index;
                    if (strcmp(pszPlus, "+")) {
                        indices.push_back(first_index);
                        first_index = last_index;
                    }
                    else {
                        indices.push_back(last_index);
                    }
                }
                else {
                    ++last_index;
                }
                // double x, y, z;
                printf("   %s (x: %f, y: %f, z: %f) %s \n",
                    pszPlus,
                    test_shape_data->padfX[j],
                    test_shape_data->padfY[j],
                    test_shape_data->padfZ[j],
                    pszPartType
                );
                part_v_count += 1;
                if (strcmp(pszPlus, "+")) {
                    part_v_count = 0;
                }
            }
            
        }
        part_v_count = 0;
        printf("Got Some shape data\n");
    }
    SHPDestroyObject(test_shape_data);

    printf("Shape Loaded\n");
}
