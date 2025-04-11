// Background_Generator.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>


struct Background {
	int tileW;
	int tileH;
	int** tiles; //neighbor values for tiles
	std::string tsfname;
	uint8_t ssID;
	bool texturesSet = false;

	Background(const char* filename);
};

// TODO: Reference additional headers your program requires here.
