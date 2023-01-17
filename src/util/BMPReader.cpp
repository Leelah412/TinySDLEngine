#include "BMPReader.h"

//#define DEBUG_BMP_READER

BMPReader::BMPReader(){

}

BMPReader::~BMPReader(){

}

vector<vector<RGB>> BMPReader::read_bmp(string path){
    FILE* f;
    cout << "Opening file " << path.c_str() << endl;
    fopen_s(&f, path.c_str(), "r");
    if(f == 0){
        cerr << "ERROR: couldn't open file at " << path << endl;
        return {};
    }
    unsigned char info[54];

    // read the 54-byte header
    fread(info, sizeof(unsigned char), 54, f);

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    // allocate 3 bytes per pixel
    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size];

    // read the rest of the data at once
    fread(data, sizeof(unsigned char), size, f);
    fclose(f);

    vector<vector<RGB>> rgblst = {};
    uint8_t r,g,b;
    // since bmp data is read upside down, we have to begin from the "top" of the data
    for(int i = height - 1; i >= 0; i--){
        rgblst.push_back({});
        for(int j = 0; j < width; j++){
            r = data[(width * i * 3) + (j * 3)];
            g = data[(width * i * 3) + (j * 3) + 1];
            b = data[(width * i * 3) + (j * 3) + 2];
            rgblst.at(height - 1 - i).push_back(RGB(r, g, b));

        }
    }

    return rgblst;
}