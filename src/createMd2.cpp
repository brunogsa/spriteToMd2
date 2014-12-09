#include "md2.hpp"
#include "md2Types.hpp"
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>

using namespace std;

int main( int argc, char* argv[] ) {

    if (argc != 2) {
        cout << "Error on createMd2: path to model directory wasn't given!" << endl;
        return -1;
    }

    const char* pathToModelDir = argv[1];
    Md2Maker md2 = Md2Maker( pathToModelDir );

    bool wasSuccessful = md2.createMd2File();

    if( wasSuccessful == false )
        cout << "Some error ocurred on creation of MD2 file!" << endl;
    else
        cout << "MD2 File was created successfully!" << endl;

    return 0;
}
