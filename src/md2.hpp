#ifndef MD2_HPP
#define MD2_HPP

#include "md2Types.hpp"
#include <vector>
#include <string>
#include <map>

using std::string;
using std::vector;
using std::map;

class Keyframe {

public:
    int frameSide;
    
    Keyframe( const char* pathToFrameFile, int frameSide );
    ~Keyframe();

    void printVoxelRepresentation();
    bool voxelExistsAtPosition( int x, int y, int z );

private:
    const char* pathToFrameFile;
    vector< vector< vector<char> > > voxelRepresentation;

    vector< vector< vector<char> > > getVoxelRepresentation();
};

class Animation {

public:
    int numberOfFrames;
    vector<Keyframe> frames;

    ~Animation();

private:
    int getNumberOfFrames();
};

class Md2Maker {

public:
    const char* pathToModelDir;

    Vertex* verticesContainer;
    Md2Header header;
    Md2Data data;

    Md2Maker();
    Md2Maker( const char* pathToModelDirectory );
    ~Md2Maker();

    // Returns 'false' if some error ocurred
    bool createMd2File();

private:

    int frameSide;
    int numberOfFrames;
    int numberOfCubes;

    vector<string> animationsName;
    int numberOfAnimations;
    map<string, Animation> animations;

    unsigned int actualIndexInVertices;
    unsigned int actualIndexInTriangles;
    unsigned int actualIndexInFrames;


    int getFrameSide();
    int getTotalNumberOfFrames();

    vector<string> getAnimationsName();
    map<string, Animation> getAnimations();

    void getVerticesFromCubeAtPosition( int x, int y, int z, Vertex* verticesContainer );
    void getTrianglesFromCubeWithVertices( const short* indexToVerticesFromCube, Triangle* trianglesContainer );
    Vertex getHiddenPoint();
    void storeKeyframeInformation( const char* frameName, Keyframe& keyFrame, unsigned int frameId);  

    Md2Header getMd2Header();
    void generateMd2Data();
};

#endif
