#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include "md2Types.hpp"
#include "md2.hpp"

using namespace std;

int main( int argc, char* argv[] ) {

    if (argc != 2) {
        cout << "Error on createMd2: path to model directory wasn't given!" << endl;
        return -1;
    }

    const char* pathToModelDir = argv[1];
    FILE* file;
    Md2* model;
    char buffer[99999];
    int i;

    model = (Md2 *) malloc( sizeof(Md2) );
    if (!model)
        return 0;                                                                                                                                                                             

    file = fopen( pathToModelDir, "rb" );
    if (!file) {
        free(model);
        return -1;
    }

    /* initialize model and read header */
    memset( model, 0, sizeof(Md2) );
    fread( &model->header, sizeof(Md2Header), 1, file );

    cout << " magicNumber:                " << model->header.magicNumber                << endl;
    cout << " version:                    " << model->header.version                    << endl;
    cout << " textureWidth:               " << model->header.textureWidth               << endl;
    cout << " textureHeight:              " << model->header.textureHeight              << endl;
    cout << " frameSizeInBytes:           " << model->header.frameSizeInBytes           << endl;
    cout << " numberOfTextures:           " << model->header.numberOfTextures           << endl;
    cout << " numberOfVerticesPerFrame:   " << model->header.numberOfVerticesPerFrame   << endl;
    cout << " numberOfTextureCoordinates: " << model->header.numberOfTextureCoordinates << endl;
    cout << " numberOfTriangles:          " << model->header.numberOfTriangles          << endl;
    cout << " numberOfOpenGlCommands:     " << model->header.numberOfOpenGlCommands     << endl;
    cout << " numberOfFrames:             " << model->header.numberOfFrames             << endl;
    cout << " offsetToTextureNames:       " << model->header.offsetToTextureNames       << endl;
    cout << " offsetToTextureCoordinates: " << model->header.offsetToTextureCoordinates << endl;
    cout << " offsetToTriangles:          " << model->header.offsetToTriangles          << endl;
    cout << " offsetToFrames:             " << model->header.offsetToFrames             << endl;
    cout << " offsetToOpenGlCommands:     " << model->header.offsetToOpenGlCommands     << endl;
    cout << " offsetToEndOfFile:          " << model->header.offsetToEndOfFile          << endl;

	if (model->header.magicNumber != 844121161 || model->header.version != 8)
	{
		fclose(file);
		free(model);
		return -1;
	}

	// Read Skins 
	fseek(file, model->header.offsetToTextureNames, SEEK_SET);

	if (model->header.numberOfTextures > 0) {

		model->data.skins = (Skin *) malloc(sizeof (Skin) * model->header.numberOfTextures);
		if (!model->data.skins) {
		    fclose(file);
		    free(model);
			return -1;
		}

		for (i = 0; i < model->header.numberOfTextures; i++)
			fread( &model->data.skins[i], sizeof(Skin), 1, file);
	}

	// Read Texture Coordinates
	fseek(file, model->header.offsetToTextureCoordinates, SEEK_SET);

	if (model->header.numberOfTextureCoordinates > 0) {

		model->data.textureCoordinates = (TextureCoordinates *) malloc( sizeof(TextureCoordinates) * model->header.numberOfTextureCoordinates);
		if (!model->data.textureCoordinates) {
		    fclose(file);
			free(model);
			return -1;
		}

		for (i = 0; i < model->header.numberOfTextureCoordinates; i++)
			fread( &model->data.textureCoordinates[i], sizeof(TextureCoordinates), 1, file );
	}

	// Read Triangles
	fseek(file, model->header.offsetToTriangles, SEEK_SET);

	if (model->header.numberOfTriangles > 0) {

		model->data.triangles = (Triangle *) malloc( sizeof(Triangle) * model->header.numberOfTriangles );
		if (!model->data.triangles) {
		    fclose(file);
			free(model);
			return -1;
		}

		for (i = 0; i < model->header.numberOfTriangles; i++)
			fread( &model->data.triangles[i], sizeof(Triangle), 1, file );
	}

	cout << "------------" << endl;
	for( int t = 0; t < model->header.numberOfTriangles; t++ ) 
	    cout << "Triangle " << t << ": ( " 
	        << model->data.triangles[t].indexToVertices[0] << ", " 
	        << model->data.triangles[t].indexToVertices[1] << ", " 
	        << model->data.triangles[t].indexToVertices[2] << " )" << endl;
	cout << "------------" << endl;

	// Read OpenGL Commands
	fseek( file, model->header.offsetToOpenGlCommands, SEEK_SET);
	if (model->header.numberOfOpenGlCommands > 0) {

		model->data.glCommandBuffer = (int *) malloc( sizeof(int) * model->header.numberOfOpenGlCommands );
		if (!model->data.glCommandBuffer) {
		    fclose(file);
			free(model);
			return -1;
		}

		fread( model->data.glCommandBuffer, sizeof(int), model->header.numberOfOpenGlCommands, file );
	}

	cout << "------------" << endl;
	cout << "OpenGL Commands: ";
	for( int c = 0; c < model->header.numberOfOpenGlCommands; c++ ) 
	    cout << model->data.glCommandBuffer[c] << " ";
	cout << endl;
	cout << "------------" << endl;

	// Read Frames
	fseek( file, model->header.offsetToFrames, SEEK_SET );
	if (model->header.numberOfFrames > 0) {

		model->data.frames = (Frame *) malloc( sizeof(Frame) * model->header.numberOfFrames );
		if (!model->data.frames) {
		    fclose(file);
			free(model);
			return -1;
		}

        for( int f = 0; f < model->header.numberOfFrames; f++ ) {
		    fread( model->data.frames[f].scaleOfCoordinate, sizeof(float), 3, file );
		    fread( model->data.frames[f].translateOfCoordinate, sizeof(float), 3, file );
		    fread( model->data.frames[f].name, sizeof(char), 16, file );

		    model->data.frames[f].firstVertexOfThisFrame = (Vertex *) malloc( sizeof(Vertex) * model->header.numberOfVerticesPerFrame );
		    fread( model->data.frames[f].firstVertexOfThisFrame, sizeof(Vertex), model->header.numberOfVerticesPerFrame, file );
        }
	}

	cout << "------------" << endl;
    for( int f = 0; f < model->header.numberOfFrames; f++ ) {
	    cout << "Frame " << f << ":" << endl;
	    cout << endl;

	    cout << "Name: " << model->data.frames[f].name << endl;

	    cout << "Scales: ( " 
	        << model->data.frames[f].scaleOfCoordinate[0] << ", " 
	        << model->data.frames[f].scaleOfCoordinate[1] << ", " 
	        << model->data.frames[f].scaleOfCoordinate[2] << " )" << endl;

	    cout << "Translates: ( " 
	        << model->data.frames[f].translateOfCoordinate[0] << ", " 
	        << model->data.frames[f].translateOfCoordinate[1] << ", " 
	        << model->data.frames[f].translateOfCoordinate[2] << " )" << endl;

	    cout << endl;
	    for( int v = 0; v < model->header.numberOfVerticesPerFrame; v++ )
	        cout << "v(" << v << ") = [ " 
	            << model->data.frames[f].firstVertexOfThisFrame[v].compressedCoordinate[0] << ", " 
	            << model->data.frames[f].firstVertexOfThisFrame[v].compressedCoordinate[1] << ", " 
	            << model->data.frames[f].firstVertexOfThisFrame[v].compressedCoordinate[2] << " ]" << endl; 

	    cout << endl << endl;
	}
	cout << "------------" << endl;

	fclose (file);
    return 0;
}

