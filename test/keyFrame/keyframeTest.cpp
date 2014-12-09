#include "md2.hpp"
#include "md2Types.hpp"
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>

using namespace std;

int main( int argc, char* argv[] ) {
    Keyframe keyframe = Keyframe( "../inputs/man/standFrame1.in", 7 );
    keyframe.printVoxelRepresentation();

    return 0;
}
