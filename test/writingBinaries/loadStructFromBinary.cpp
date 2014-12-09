#include <cstdio>
#include <iostream>

using namespace std;

struct SomeData {
    short shortVar;
    char charVar;
    unsigned int unsignedVar;
    int intVar;
    float floatVar;
};

int main() {

    SomeData myStruct;

    FILE* file;
    file = fopen("myStruct.bin", "rb");
    fread( &myStruct, sizeof(char) , sizeof(myStruct), file );
    fclose(file);

    cout << myStruct.shortVar << endl;
    cout << myStruct.charVar << endl;
    cout << myStruct.unsignedVar << endl;
    cout << myStruct.intVar << endl;
    cout << myStruct.floatVar << endl;

    return 0;
}
