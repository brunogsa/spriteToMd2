#ifndef MD2_TYPES_HPP
#define MD2_TYPES_HPP

#ifndef byte
    typedef unsigned char byte;
#endif 

/*
 *  The basic architecture is:
 *
 *  1 model has N animations;
 *  1 animation has M frames;
 *  1 frame has Md2Header::numberOfVerticesPerFrame vertices;
 *
 */

struct Md2Header {
    // If magicNumber != 844121161 or version != 8, so file isn't a MD2
    int magicNumber;
    int version;

    int textureWidth;  
    int textureHeight;  

    // This will be used to memory allocation
    int frameSizeInBytes;

    int numberOfTextures;
    int numberOfVerticesPerFrame;
    int numberOfTextureCoordinates;
    int numberOfTriangles;
    int numberOfOpenGlCommands;
    int numberOfFrames;

    int offsetToTextureNames;
    int offsetToTextureCoordinates;
    int offsetToTriangles;
    int offsetToFrames;
    int offsetToOpenGlCommands;
    int offsetToEndOfFile;
};

typedef char Skin[64];

struct TextureCoordinates {
    short compressedX;                                              // Divide by Md2Header::textureWidth to get the uncompressed value
    short compressedY;                      // Divide by Md2Header::textureHeight to get the uncompressed value
};

struct Triangle {
    short indexToVertices[3];               // 3 vertices make 1 triangle
    short indexToTextures[3];               // Each vertex has 1 associated texture
};

struct Vertex {
    unsigned char compressedCoordinate[3];  // X Y Z
    unsigned char indexToLightNormalVector; // Index to a table containing pre-calculated normals
};

struct Frame {
    float scaleOfCoordinate[3];             // Use this to uncompress vertex coordinates
    float translateOfCoordinate[3];         // Use this to uncompress vertex coordinates

    char name[16];
    Vertex* firstVertexOfThisFrame;
};

struct Md2Data {
    Skin* skins;
    TextureCoordinates* textureCoordinates;
    Triangle* triangles;
    Frame* frames;
    int* glCommandBuffer;
};

struct Md2 {
    Md2Header header;
    Md2Data data;
};

#endif
