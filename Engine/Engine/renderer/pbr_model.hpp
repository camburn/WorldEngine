
struct Texture {
    int id;
};


class PBRMaterial {
public:
    Texture albedo;
    Texture normal;
    Texture metallic;
    Texture roughness;
    Texture ambient_occulusion;
    Texture emission;
};