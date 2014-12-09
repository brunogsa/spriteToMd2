#include "md2Types.hpp"
#include "md2.hpp"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

int main() {
    Md2Maker md2Maker = Md2Maker();
    md2Maker.pathToModelDir = "../src";


    // Header
    Md2Header cubeMd2Header;

    cubeMd2Header.magicNumber = 844121161;
    cubeMd2Header.version = 8;

    cubeMd2Header.textureWidth = 0;
    cubeMd2Header.textureHeight = 0;
    cubeMd2Header.numberOfTextures = 0;
    cubeMd2Header.numberOfTextureCoordinates = 0;
    cubeMd2Header.numberOfOpenGlCommands = 0;

    cubeMd2Header.numberOfVerticesPerFrame = 8;
    cubeMd2Header.frameSizeInBytes = sizeof(char) * 16 + sizeof(float) * 6 + sizeof(Vertex) * cubeMd2Header.numberOfVerticesPerFrame ;
    cubeMd2Header.numberOfFrames = 1;
    cubeMd2Header.numberOfTriangles = 12 * cubeMd2Header.numberOfFrames;

    const int offsetEndOfHeader = sizeof(Md2Header);
    cubeMd2Header.offsetToTextureNames = offsetEndOfHeader;

    const int maxNumberOfCharsPerTextureName = 64; 
    cubeMd2Header.offsetToTextureCoordinates = cubeMd2Header.offsetToTextureNames + cubeMd2Header.numberOfTextures * maxNumberOfCharsPerTextureName;

    const int sizeOfTextureCoordinateInBytes = sizeof(TextureCoordinates);
    cubeMd2Header.offsetToTriangles = cubeMd2Header.offsetToTextureCoordinates + cubeMd2Header.numberOfTextureCoordinates * sizeOfTextureCoordinateInBytes;

    const int sizeOfTriangleInBytes = sizeof(Triangle);
    cubeMd2Header.offsetToFrames = cubeMd2Header.offsetToTriangles + cubeMd2Header.numberOfTriangles * sizeOfTriangleInBytes;

    cubeMd2Header.offsetToOpenGlCommands = cubeMd2Header.offsetToFrames + cubeMd2Header.numberOfFrames * cubeMd2Header.frameSizeInBytes;

    const int sizeOfOpenGlCommandInBytes = sizeof(int);
    cubeMd2Header.offsetToEndOfFile = cubeMd2Header.offsetToOpenGlCommands + cubeMd2Header.numberOfOpenGlCommands * sizeOfOpenGlCommandInBytes;


    // Data
    Md2Data cubeMd2Data;

    cubeMd2Data.skins = (Skin *) malloc( sizeof(Skin) * cubeMd2Header.numberOfTextures );
    cubeMd2Data.textureCoordinates = (TextureCoordinates *) malloc( sizeof(TextureCoordinates) * cubeMd2Header.numberOfTextureCoordinates );
    cubeMd2Data.glCommandBuffer = (int *) malloc( sizeof(int) * cubeMd2Header.numberOfOpenGlCommands );

    cubeMd2Data.triangles = (Triangle *) malloc( sizeof(Triangle) * cubeMd2Header.numberOfTriangles );
    cubeMd2Data.frames = (Frame *) malloc( sizeof(Frame) * cubeMd2Header.numberOfFrames );

    Vertex* verticesContainer = (Vertex *) malloc( sizeof(Vertex) * cubeMd2Header.numberOfVerticesPerFrame * cubeMd2Header.numberOfFrames );
    
    // Fill skins
/*
 *    string s = "rgbPixels.png";
 *    for( int c = 0; c < 16; c++ ) {
 *
 *        if (c < s.size() )
 *            cubeMd2Data.skins[0][c] = s[c];
 *        else
 *            cubeMd2Data.skins[0][c] = '\0';
 *    }
 *
 *    // Fill texture coordinates
 *    TextureCoordinates tc;
 *
 *    tc.compressedX = 0;
 *    tc.compressedY = 0;
 *
 *    cubeMd2Data.textureCoordinates[0] = tc;
 */

    // Fill triangles
    Triangle t;

    t.indexToTextures[0] = 0;
    t.indexToTextures[1] = 1;
    t.indexToTextures[2] = 2;

    t.indexToVertices[0] = 0;
    t.indexToVertices[1] = 1;
    t.indexToVertices[2] = 2;
    cubeMd2Data.triangles[0] = t;

    t.indexToVertices[0] = 1;
    t.indexToVertices[1] = 2;
    t.indexToVertices[2] = 3;
    cubeMd2Data.triangles[1] = t;

    t.indexToVertices[0] = 1;
    t.indexToVertices[1] = 0;
    t.indexToVertices[2] = 4;
    cubeMd2Data.triangles[2] = t;

    t.indexToVertices[0] = 1;
    t.indexToVertices[1] = 4;
    t.indexToVertices[2] = 5;
    cubeMd2Data.triangles[3] = t;

    t.indexToVertices[0] = 4;
    t.indexToVertices[1] = 5;
    t.indexToVertices[2] = 6;
    cubeMd2Data.triangles[4] = t;

    t.indexToVertices[0] = 5;
    t.indexToVertices[1] = 6;
    t.indexToVertices[2] = 7;
    cubeMd2Data.triangles[5] = t;

    t.indexToVertices[0] = 2;
    t.indexToVertices[1] = 3;
    t.indexToVertices[2] = 6;
    cubeMd2Data.triangles[6] = t;

    t.indexToVertices[0] = 3;
    t.indexToVertices[1] = 6;
    t.indexToVertices[2] = 7;
    cubeMd2Data.triangles[7] = t;

    t.indexToVertices[0] = 0;
    t.indexToVertices[1] = 4;
    t.indexToVertices[2] = 6;
    cubeMd2Data.triangles[8] = t;

    t.indexToVertices[0] = 0;
    t.indexToVertices[1] = 6;
    t.indexToVertices[2] = 2;
    cubeMd2Data.triangles[9] = t;

    t.indexToVertices[0] = 1;
    t.indexToVertices[1] = 3;
    t.indexToVertices[2] = 7;
    cubeMd2Data.triangles[10] = t;

    t.indexToVertices[0] = 1;
    t.indexToVertices[1] = 5;
    t.indexToVertices[2] = 7;
    cubeMd2Data.triangles[11] = t;

    // Fill vertices 
    Vertex v;
    v.indexToLightNormalVector = 0;

    v.compressedCoordinate[0] = 0;
    v.compressedCoordinate[1] = 0;
    v.compressedCoordinate[2] = 0;
    verticesContainer[0] = v;

    v.compressedCoordinate[0] = 1;
    v.compressedCoordinate[1] = 0;
    v.compressedCoordinate[2] = 0;
    verticesContainer[1] = v;

    v.compressedCoordinate[0] = 0;
    v.compressedCoordinate[1] = 1;
    v.compressedCoordinate[2] = 0;
    verticesContainer[2] = v;

    v.compressedCoordinate[0] = 1;
    v.compressedCoordinate[1] = 1;
    v.compressedCoordinate[2] = 0;
    verticesContainer[3] = v;

    v.compressedCoordinate[0] = 0;
    v.compressedCoordinate[1] = 0;
    v.compressedCoordinate[2] = 1;
    verticesContainer[4] = v;

    v.compressedCoordinate[0] = 1;
    v.compressedCoordinate[1] = 0;
    v.compressedCoordinate[2] = 1;
    verticesContainer[5] = v;

    v.compressedCoordinate[0] = 0;
    v.compressedCoordinate[1] = 1;
    v.compressedCoordinate[2] = 1;
    verticesContainer[6] = v;

    v.compressedCoordinate[0] = 1;
    v.compressedCoordinate[1] = 1;
    v.compressedCoordinate[2] = 1;
    verticesContainer[7] = v;

    md2Maker.verticesContainer = verticesContainer;
     
    
    // Fill frame
    Frame f;

    const float scale = 1;
    f.scaleOfCoordinate[0] = scale;
    f.scaleOfCoordinate[1] = scale;
    f.scaleOfCoordinate[2] = scale;

    const float translate = 0;
    f.translateOfCoordinate[0] = translate;
    f.translateOfCoordinate[1] = translate;
    f.translateOfCoordinate[2] = translate;

    f.name[0] = 's';
    f.name[1] = 't';
    f.name[2] = 'a';
    f.name[3] = 'n';
    f.name[4] = 'd';
    f.name[5] = '\0';

    f.firstVertexOfThisFrame = verticesContainer;

    cubeMd2Data.frames[0] = f;
    

    // Generating MD2
    md2Maker.header = cubeMd2Header;
    md2Maker.data = cubeMd2Data;

    bool wasSuccessful = md2Maker.createMd2File();

    if( wasSuccessful == false )
        cout << "Some error ocurred on creation of MD2 file!" << endl;
    else
        cout << "MD2 File was created successfully!" << endl;

    return 0;
}
