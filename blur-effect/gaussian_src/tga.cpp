/*
 * tga.cpp
 *
 *  Created on: Jan 26, 2010
 *      Author: aqnuep
 *
 *  Note: Do not use this module in any product as it is a very rough
 *        TGA image loader made only for the demo program and is far
 *        from product quality
 */

#include <iostream>
#include <fstream>
#include "tga.h"

using namespace std;

TGAImage::TGAImage(const char* filename) {

	this->data = NULL;

	ifstream file(filename, ios::in | ios::binary);
	if (!file) {
		cerr << "Unable to open file: " << filename << endl;
		return;
	}

	TGAHeader header;
	file.read((char*)&header, sizeof(header));

	if ((header.ImageType != 2) or (header.CMapType != 0)) {
		cerr << "Invalid file format: " << filename << endl;
		return;
	}

	if ((header.Depth != 32) and (header.Depth != 24)) {
		cerr << "Unsupported color depth: " << (int)header.Depth << " (" << filename << ")" << endl;
		return;
	}

	this->width = header.Width;
	this->height = header.Height;
	this->depth = header.Depth;

	int dataSize = (this->width * this->height * this->depth) / 8;
	this->data = malloc(dataSize);

	file.seekg(header.IDLength, ios_base::cur);
	file.read((char*)(this->data), dataSize);

}

TGAImage::~TGAImage() {
	if (this->data != NULL) free(this->data);
}
