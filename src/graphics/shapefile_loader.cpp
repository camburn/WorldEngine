#include "graphics/shapefile_loader.hpp"


void LoadShapefile() {
    // Testing ShapeFile loading
    SHPHandle test_shape = SHPOpen("./assets/shapefiles/polylines", "rb");
    int ipart;
    int num_entities;
    int shapetype; // 1 - Points; 3 - Arcs (Lines); 5 - Polygons; 8 - MultiPoint (related points)
    double min_bounds[4], max_bounds[4];

    SHPGetInfo(test_shape, &num_entities, &shapetype, min_bounds, max_bounds);

    SHPObject *test_shape_data;
    const char 	*pszPlus;
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

                double x, y, z;
                printf("   %s (x: %f, y: %f, z: %f) %s \n",
                    pszPlus,
                    test_shape_data->padfX[j],
                    test_shape_data->padfY[j],
                    test_shape_data->padfZ[j],
                    pszPartType);
            }
        }
        printf("Got Some shape data\n");
    }
    SHPDestroyObject(test_shape_data);


    printf("Shape Loaded");
}