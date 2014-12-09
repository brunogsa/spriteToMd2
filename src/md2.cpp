#include "md2.hpp"
#include "md2Types.hpp"
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

string int2string( int Number ) {
    ostringstream ss;
    ss << Number;
    return ss.str();
}

string trim( string& s ) {
    string::iterator end_pos = remove( s.begin(), s.end(), ' ' );
    s.erase( end_pos, s.end( ));

    return s;
}

vector<string> split( const string& s, char delim ) {
    vector<string> elems;
    stringstream stream(s);
    string item;

    while( getline( stream, item, delim ) ) {
        elems.push_back( trim(item) );
    }

    return elems;
}

Md2Maker::Md2Maker() {}

Md2Maker::Md2Maker( const char* pathToModelDirectory ) : pathToModelDir( pathToModelDirectory ) {
    frameSide = getFrameSide();
    numberOfFrames = getTotalNumberOfFrames();
    numberOfCubes = frameSide * frameSide * frameSide * numberOfFrames;

    animationsName = getAnimationsName();
    numberOfAnimations = animationsName.size();
    animations = getAnimations();

    header = getMd2Header();
    generateMd2Data();
}

Md2Maker::~Md2Maker() {
    free( data.textureCoordinates );
    free( data.triangles );
    free( data.glCommandBuffer );

    for( int f = 0; f < header.numberOfFrames; f++ )
        free( data.frames[f].firstVertexOfThisFrame );
    free( data.frames );

    animationsName.clear();
    animations.clear();
}

bool Md2Maker::createMd2File() {
    string aux = string( pathToModelDir );
    vector<string> splittedPath = split( aux, '/' );
    string nameOfDir = splittedPath[ splittedPath.size() - 1 ];
    aux += "/" + nameOfDir + ".md2";
    const char* pathToFinalMd2File = aux.c_str();

    FILE* file;
    file = fopen( pathToFinalMd2File, "wb" );
    int successfulBytesWritten;

    // Writing Header
    successfulBytesWritten = fwrite( &header, sizeof(Md2Header), 1, file );

    if ( successfulBytesWritten != 1 ) {
        fclose(file);
        return false;
    }

    // Writing Skins
    if (header.numberOfTextures > 0) {

        successfulBytesWritten = fwrite( data.skins, sizeof(Skin), header.numberOfTextures, file );

        if ( successfulBytesWritten != header.numberOfTextures ) {
            fclose(file);
            return false;
        }
    }

    // Writing Texture Coordinates
    if (header.numberOfTextureCoordinates > 0) {

        successfulBytesWritten = fwrite( data.textureCoordinates, sizeof(TextureCoordinates), header.numberOfTextureCoordinates, file );

        if ( successfulBytesWritten != header.numberOfTextureCoordinates ) {
            fclose(file);
            return false;
        }
    }

    // Writing Triangles
    successfulBytesWritten = fwrite( data.triangles, sizeof(Triangle), header.numberOfTriangles, file );

    if ( successfulBytesWritten != header.numberOfTriangles ) {
        fclose(file);
        return false;
    }

    // Writing Frames
    for( int f = 0; f < header.numberOfFrames; f++ ) {

        // scale
        successfulBytesWritten = fwrite( data.frames[f].scaleOfCoordinate, sizeof(float), 3, file );

        if ( successfulBytesWritten != 3 ) {
            fclose(file);
            return false;
        }

        // translate
        successfulBytesWritten = fwrite( data.frames[f].translateOfCoordinate, sizeof(float), 3, file );

        if ( successfulBytesWritten != 3 ) {
            fclose(file);
            return false;
        }

        // name
        successfulBytesWritten = fwrite( data.frames[f].name, sizeof(char), 16, file );

        if ( successfulBytesWritten != 16 ) {
            fclose(file);
            return false;
        }

        // vertices
        successfulBytesWritten = fwrite( data.frames[f].firstVertexOfThisFrame, sizeof(Vertex), header.numberOfVerticesPerFrame, file );

        if ( successfulBytesWritten != header.numberOfVerticesPerFrame ) {
            fclose(file);
            return false;
        }

    }

    // Writing OpenGL Commands
    if (header.numberOfOpenGlCommands > 0) {

        successfulBytesWritten = fwrite( data.glCommandBuffer, sizeof(int), header.numberOfOpenGlCommands, file );

        if ( successfulBytesWritten != header.numberOfOpenGlCommands ) {
            fclose(file);
            return false;
        }
    }

    fclose(file);
    return true;
}

Md2Header Md2Maker::getMd2Header() {
    Md2Header md2Header;

    md2Header.magicNumber = 844121161;
    md2Header.version = 8;

    md2Header.textureWidth = 0;
    md2Header.textureHeight = 0;
    md2Header.numberOfTextures = 0;
    md2Header.numberOfTextureCoordinates = 0;

    const int numberOfVerticesPerCube = 8;
    md2Header.numberOfVerticesPerFrame = numberOfCubes * numberOfVerticesPerCube;

    md2Header.frameSizeInBytes = sizeof(char) * 16 + sizeof(float) * 6 + sizeof(Vertex) * md2Header.numberOfVerticesPerFrame ;
    md2Header.numberOfFrames = numberOfFrames;

    const int numberOfTrianglesPerFace = 2;
    const int numberOfFacesPerCube = 6;
    md2Header.numberOfTriangles = numberOfCubes * numberOfFacesPerCube * numberOfTrianglesPerFace;

    md2Header.numberOfOpenGlCommands = 0;

    const int offsetEndOfHeader = sizeof(Md2Header);
    md2Header.offsetToTextureNames = offsetEndOfHeader;

    const int maxNumberOfCharsPerTextureName = 64; 
    md2Header.offsetToTextureCoordinates = md2Header.offsetToTextureNames + md2Header.numberOfTextures * maxNumberOfCharsPerTextureName;

    const int sizeOfTextureCoordinateInBytes = sizeof(TextureCoordinates);
    md2Header.offsetToTriangles = md2Header.offsetToTextureCoordinates + md2Header.numberOfTextureCoordinates * sizeOfTextureCoordinateInBytes;

    const int sizeOfTriangleInBytes = sizeof(Triangle);
    md2Header.offsetToFrames = md2Header.offsetToTriangles + md2Header.numberOfTriangles * sizeOfTriangleInBytes;

    md2Header.offsetToOpenGlCommands = md2Header.offsetToFrames + md2Header.numberOfFrames * md2Header.frameSizeInBytes;

    const int sizeOfOpenGlCommandInBytes = sizeof(int);
    md2Header.offsetToEndOfFile = md2Header.offsetToOpenGlCommands + md2Header.numberOfOpenGlCommands * sizeOfOpenGlCommandInBytes;

    return md2Header;
}

void Md2Maker::getVerticesFromCubeAtPosition( int x, int y, int z, Vertex* verticesContainer ) {

    verticesContainer[0].compressedCoordinate[0] = frameSide - z;
    verticesContainer[0].compressedCoordinate[1] = y;
    verticesContainer[0].compressedCoordinate[2] = frameSide - x;
    verticesContainer[0].indexToLightNormalVector = 0;


    verticesContainer[1].compressedCoordinate[0] = frameSide - z - 1;
    verticesContainer[1].compressedCoordinate[1] = y;
    verticesContainer[1].compressedCoordinate[2] = frameSide - x;
    verticesContainer[1].indexToLightNormalVector = 0;

    verticesContainer[2].compressedCoordinate[0] = frameSide - z;
    verticesContainer[2].compressedCoordinate[1] = y + 1;
    verticesContainer[2].compressedCoordinate[2] = frameSide - x;
    verticesContainer[2].indexToLightNormalVector = 0;

    verticesContainer[3].compressedCoordinate[0] = frameSide - z - 1;
    verticesContainer[3].compressedCoordinate[1] = y + 1;
    verticesContainer[3].compressedCoordinate[2] = frameSide - x;
    verticesContainer[3].indexToLightNormalVector = 0;

    verticesContainer[4].compressedCoordinate[0] = frameSide - z;
    verticesContainer[4].compressedCoordinate[1] = y;
    verticesContainer[4].compressedCoordinate[2] = frameSide - x - 1;
    verticesContainer[4].indexToLightNormalVector = 0;

    verticesContainer[5].compressedCoordinate[0] = frameSide - z - 1;
    verticesContainer[5].compressedCoordinate[1] = y;
    verticesContainer[5].compressedCoordinate[2] = frameSide - x - 1;
    verticesContainer[5].indexToLightNormalVector = 0;

    verticesContainer[6].compressedCoordinate[0] = frameSide - z;
    verticesContainer[6].compressedCoordinate[1] = y + 1;
    verticesContainer[6].compressedCoordinate[2] = frameSide - x - 1;
    verticesContainer[6].indexToLightNormalVector = 0;

    verticesContainer[7].compressedCoordinate[0] = frameSide - z - 1;
    verticesContainer[7].compressedCoordinate[1] = y + 1;
    verticesContainer[7].compressedCoordinate[2] = frameSide - x - 1;
    verticesContainer[7].indexToLightNormalVector = 0;
};

void Md2Maker::getTrianglesFromCubeWithVertices( const short* indexToVerticesFromCube, Triangle* trianglesContainer ) {

    // Face 1
    trianglesContainer[0].indexToVertices[0] = indexToVerticesFromCube[0];
    trianglesContainer[0].indexToVertices[1] = indexToVerticesFromCube[1];
    trianglesContainer[0].indexToVertices[2] = indexToVerticesFromCube[3];
    trianglesContainer[0].indexToTextures[0] = 0;
    trianglesContainer[0].indexToTextures[1] = 0;
    trianglesContainer[0].indexToTextures[2] = 0;

    trianglesContainer[1].indexToVertices[0] = indexToVerticesFromCube[3];
    trianglesContainer[1].indexToVertices[1] = indexToVerticesFromCube[2];
    trianglesContainer[1].indexToVertices[2] = indexToVerticesFromCube[0];
    trianglesContainer[1].indexToTextures[0] = 0;
    trianglesContainer[1].indexToTextures[1] = 0;
    trianglesContainer[1].indexToTextures[2] = 0;

    // Face 2
    trianglesContainer[2].indexToVertices[0] = indexToVerticesFromCube[0];
    trianglesContainer[2].indexToVertices[1] = indexToVerticesFromCube[4];
    trianglesContainer[2].indexToVertices[2] = indexToVerticesFromCube[5];
    trianglesContainer[2].indexToTextures[0] = 0;
    trianglesContainer[2].indexToTextures[1] = 0;
    trianglesContainer[2].indexToTextures[2] = 0;

    trianglesContainer[3].indexToVertices[0] = indexToVerticesFromCube[5];
    trianglesContainer[3].indexToVertices[1] = indexToVerticesFromCube[1];
    trianglesContainer[3].indexToVertices[2] = indexToVerticesFromCube[0];
    trianglesContainer[3].indexToTextures[0] = 0;
    trianglesContainer[3].indexToTextures[1] = 0;
    trianglesContainer[3].indexToTextures[2] = 0;

    // Face 3
    trianglesContainer[4].indexToVertices[0] = indexToVerticesFromCube[7];
    trianglesContainer[4].indexToVertices[1] = indexToVerticesFromCube[5];
    trianglesContainer[4].indexToVertices[2] = indexToVerticesFromCube[4];
    trianglesContainer[4].indexToTextures[0] = 0;
    trianglesContainer[4].indexToTextures[1] = 0;
    trianglesContainer[4].indexToTextures[2] = 0;

    trianglesContainer[5].indexToVertices[0] = indexToVerticesFromCube[4];
    trianglesContainer[5].indexToVertices[1] = indexToVerticesFromCube[6];
    trianglesContainer[5].indexToVertices[2] = indexToVerticesFromCube[7];
    trianglesContainer[5].indexToTextures[0] = 0;
    trianglesContainer[5].indexToTextures[1] = 0;
    trianglesContainer[5].indexToTextures[2] = 0;

    // Face 4
    trianglesContainer[6].indexToVertices[0] = indexToVerticesFromCube[7];
    trianglesContainer[6].indexToVertices[1] = indexToVerticesFromCube[6];
    trianglesContainer[6].indexToVertices[2] = indexToVerticesFromCube[2];
    trianglesContainer[6].indexToTextures[0] = 0;
    trianglesContainer[6].indexToTextures[1] = 0;
    trianglesContainer[6].indexToTextures[2] = 0;

    trianglesContainer[7].indexToVertices[0] = indexToVerticesFromCube[2];
    trianglesContainer[7].indexToVertices[1] = indexToVerticesFromCube[3];
    trianglesContainer[7].indexToVertices[2] = indexToVerticesFromCube[7];
    trianglesContainer[7].indexToTextures[0] = 0;
    trianglesContainer[7].indexToTextures[1] = 0;
    trianglesContainer[7].indexToTextures[2] = 0;

    // Face 5
    trianglesContainer[8].indexToVertices[0] = indexToVerticesFromCube[0];
    trianglesContainer[8].indexToVertices[1] = indexToVerticesFromCube[2];
    trianglesContainer[8].indexToVertices[2] = indexToVerticesFromCube[6];
    trianglesContainer[8].indexToTextures[0] = 0;
    trianglesContainer[8].indexToTextures[1] = 0;
    trianglesContainer[8].indexToTextures[2] = 0;

    trianglesContainer[9].indexToVertices[0] = indexToVerticesFromCube[6];
    trianglesContainer[9].indexToVertices[1] = indexToVerticesFromCube[4];
    trianglesContainer[9].indexToVertices[2] = indexToVerticesFromCube[0];
    trianglesContainer[9].indexToTextures[0] = 0;
    trianglesContainer[9].indexToTextures[1] = 0;
    trianglesContainer[9].indexToTextures[2] = 0;

    // Face 6
    trianglesContainer[10].indexToVertices[0] = indexToVerticesFromCube[7];
    trianglesContainer[10].indexToVertices[1] = indexToVerticesFromCube[3];
    trianglesContainer[10].indexToVertices[2] = indexToVerticesFromCube[1];
    trianglesContainer[10].indexToTextures[0] = 0;
    trianglesContainer[10].indexToTextures[1] = 0;
    trianglesContainer[10].indexToTextures[2] = 0;

    trianglesContainer[11].indexToVertices[0] = indexToVerticesFromCube[1];
    trianglesContainer[11].indexToVertices[1] = indexToVerticesFromCube[5];
    trianglesContainer[11].indexToVertices[2] = indexToVerticesFromCube[7];
    trianglesContainer[11].indexToTextures[0] = 0;
    trianglesContainer[11].indexToTextures[1] = 0;
    trianglesContainer[11].indexToTextures[2] = 0;
}

Vertex Md2Maker::getHiddenPoint() {
    Vertex hiddenVertex;

    hiddenVertex.compressedCoordinate[0] = 0;
    hiddenVertex.compressedCoordinate[1] = 0;
    hiddenVertex.compressedCoordinate[2] = 0;
    hiddenVertex.indexToLightNormalVector = 0;

    return hiddenVertex;
}

void Md2Maker::storeKeyframeInformation( const char* frameName, Keyframe& keyFrame, unsigned int frameId ) {
    Frame* frame = &data.frames[frameId];

    // Storing frame
    frame->scaleOfCoordinate[0] = 1;
    frame->scaleOfCoordinate[1] = 1;
    frame->scaleOfCoordinate[2] = 1;

    frame->translateOfCoordinate[0] = 0;
    frame->translateOfCoordinate[1] = 0;
    frame->translateOfCoordinate[2] = 0;

    for( int c = 0; c < 16; c++ )
        frame->name[c] = frameName[c];

    frame->firstVertexOfThisFrame = (Vertex *) malloc( sizeof(Vertex) * header.numberOfVerticesPerFrame );
    Vertex* verticesContainer = frame->firstVertexOfThisFrame;
    int actualIndexInVertices = 0;

    // Storing vertices and triangles
    unsigned int frameSide = keyFrame.frameSide;

    for( int z = 0; z < frameSide; z++ ) {
        for( int y = 0; y < frameSide; y++ ) {
            for( int x = 0; x < frameSide; x++ ) {

                // Storing vertices
                Vertex* vertices = (Vertex *) malloc( sizeof(Vertex) * 8 );

                bool voxelExists = keyFrame.voxelExistsAtPosition( x, y, z );
                if( voxelExists ) {
                    getVerticesFromCubeAtPosition( x, y, z, vertices );

                } else {
                    Vertex hiddenVertex = getHiddenPoint();

                    for( int v = 0; v < 8; v++ )
                        vertices[v] = hiddenVertex;
                }

                short* indexToVertices = (short *) malloc( sizeof(short) * 8 );

                for( int v = 0; v < 8; v++ ) {
                    verticesContainer[actualIndexInVertices] = vertices[v];
                    indexToVertices[v] = actualIndexInVertices;
                    actualIndexInVertices++;
                }

                free( vertices );

                // Storing triangles
                Triangle* triangles = (Triangle *) malloc( sizeof(Triangle) * 12 );
                getTrianglesFromCubeWithVertices( indexToVertices, triangles );

                for( int t = 0; t < 12; t++ ) {
                    data.triangles[actualIndexInTriangles] = triangles[t];
                    actualIndexInTriangles++;
                }

                free( triangles );
                free( indexToVertices );

            }
        }
    }

}

string getFormattedFrameName(const string& animationName, int id) {
    string formattedFrameName;
    formattedFrameName += animationName;
    
    string idAsString = to_string(id);
    int numberOfDigits = idAsString.size();

    if( numberOfDigits == 1 )
        formattedFrameName += "0" + idAsString;
    else
        formattedFrameName + idAsString;

    return formattedFrameName;
}

void Md2Maker::generateMd2Data() {
    data.skins = NULL;           // We won't use this

    data.textureCoordinates = (TextureCoordinates *) malloc( sizeof(TextureCoordinates) * header.numberOfTextures );

    data.triangles = (Triangle *) malloc( sizeof(Triangle) * header.numberOfTriangles );
    actualIndexInTriangles = 0;

    data.frames = (Frame *) malloc( sizeof(Frame) * header.numberOfFrames );
    actualIndexInFrames = 0;

    data.glCommandBuffer = (int *) malloc( sizeof(int) * header.numberOfOpenGlCommands );

    // TODO: Fill textureCoordinates

    // Fill triangles and frames
    for( int i = 0; i < numberOfAnimations; i++ ) {
        string animationName = animationsName[i];
        Animation animation = animations[animationName];

        for( int j = 0; j < animation.numberOfFrames; j++ ) {
            Keyframe keyFrame = animation.frames[j];
            const char* frameName = getFormattedFrameName(animationName, j).c_str();

            storeKeyframeInformation(frameName, keyFrame, j);
        }

    }
    
    // TODO: Fill glCommandBuffer
}

int Md2Maker::getFrameSide() {
    int frameSide;

    const char* pathToConfigFile  = ( string( pathToModelDir ) + "/config" ).c_str();
    FILE* file;
    file = fopen( pathToConfigFile, "r" );
    int offsetToFrameSide = 13;
    fseek( file, offsetToFrameSide, SEEK_SET );
    fscanf( file, "%d", &frameSide );
    fclose(file);

    return frameSide;
}

int getDataFromFile( const char* pathToDataFile ) {
    int data;

    FILE* file;
    file = fopen( pathToDataFile, "r" );
    fscanf( file, "%d", &data );
    fclose(file);

    return data;
}

int Md2Maker::getTotalNumberOfFrames() {
    const char* pathToInfoFile  = ( string( pathToModelDir ) + "/totalNumberOfFrames.in" ).c_str();
    int numberOfFrames = getDataFromFile( pathToInfoFile );
    return numberOfFrames;
}

vector<string> Md2Maker::getAnimationsName() {
    vector<string> animationsName;

    const char* pathToConfigFile  = ( string( pathToModelDir ) + "/config" ).c_str();

    ifstream file;
    file.open( pathToConfigFile );
    string line;

    getline(file, line);
    getline(file, line);    // Got second line

    animationsName = split( ( split( line, '=' ) )[1], ',' );
    file.close();

    return animationsName;
}

map<string, Animation> Md2Maker::getAnimations() {
    map<string, Animation> animations;

    for (int i = 0; i < numberOfAnimations; i++) {
        Animation animation;
        string animationName = animationsName[i];

        const char* pathToFramesFromAnimation = ( string(pathToModelDir) + "/" + animationName + "NumberOfFrames.in" ).c_str();
        animation.numberOfFrames = getDataFromFile( pathToFramesFromAnimation );

        for (int i = 0; i < animation.numberOfFrames; i++) {
            const char* pathToKeyframeFile = ( string(pathToModelDir) + "/" + animationName + "Frame" + int2string(i+1) + ".in" ).c_str();

            Keyframe keyframe = Keyframe( pathToKeyframeFile, frameSide );
            animation.frames.push_back( keyframe );
        }

        animations[ animationName ] = animation;
    }

    return animations;
}


Keyframe::Keyframe( const char* pathToFrameFile, int frameSide ) : pathToFrameFile( pathToFrameFile ), frameSide( frameSide ) {
    voxelRepresentation = getVoxelRepresentation();
}

Keyframe::~Keyframe() {
    for( int i = 0; i < frameSide; i++ ) {

        for( int j = 0; j < frameSide; j++ )
            voxelRepresentation[i][j].clear();

        voxelRepresentation[i].clear();    
    }

    voxelRepresentation.clear();
}

vector< vector< vector<char> > > Keyframe::getVoxelRepresentation() {
    vector< vector< vector<char> > > voxelRep;

    voxelRep.resize( frameSide );
    for( int y = 0; y < frameSide; y++ ) {

        voxelRep[y].resize( frameSide );
        for( int z = 0; z < frameSide; z++ )
            voxelRep[y][z].resize( frameSide );
    }

    FILE* file;
    file = fopen( pathToFrameFile, "r" );
    char element;

    int z = 0;
    while( z < frameSide ) {

        int y = 0;
        while( y < frameSide ) {

            int x = 0;
            while( x < frameSide ) {

                element = fgetc(file);
                if( element != '1' && element != '0' )
                    continue;

                voxelRep[y][x][z] = element;
                x++;
            }

            y++;
        }

        z++;
    }

    fclose(file);
    return voxelRep;
}

bool Keyframe::voxelExistsAtPosition( int x, int y, int z ) {
    short voxelRepresentationAtPoint = voxelRepresentation[x][y][z];
    return ( voxelRepresentationAtPoint == '0' );
}

void Keyframe::printVoxelRepresentation() {

    for( int z = 0; z < frameSide; z++ ) {

        for( int y = 0; y < frameSide; y++ ) {

            for( int x = 0; x < frameSide; x++ ) {
                cout << voxelRepresentation[y][x][z] << " ";
            }

            cout << endl;
        }

        cout << endl;
    }
}

Animation::~Animation() {
    frames.clear();
}

int Animation::getNumberOfFrames() {
    return frames.size();
}

