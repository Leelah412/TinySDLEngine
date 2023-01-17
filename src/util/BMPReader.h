#ifndef __BMP_READER_H__
#define __BMP_READER_H__

#include <vector>
#include <string>
#include <iostream>

// Reads Bitmaps and returns a 2D array containing RGB information

using namespace std;

typedef struct __RGB__{
	uint8_t r;
	uint8_t g;
	uint8_t b;

	__RGB__(uint8_t _r = 0, uint8_t _g = 0, uint8_t _b = 0)
		: r{_r}, g{_g}, b{_b}{}
} RGB;

class BMPReader{

public:
	BMPReader();
	~BMPReader();

	static vector<vector<RGB>> read_bmp(string path);

};


#endif // !__BMP_READER_H__

