#include <cstdio>

struct SomeData {
    short shortVar;
    char charVar;
    unsigned int unsignedVar;
    int intVar;
    float floatVar;
};

int main() {

    SomeData myStruct = { 2, 'a', 3, 4, 5.3 };

    FILE* file;
    file = fopen("myStruct.bin", "wb");
    fwrite(&myStruct , sizeof(char), sizeof(myStruct), file);
    fclose(file);

    return 0;
}
