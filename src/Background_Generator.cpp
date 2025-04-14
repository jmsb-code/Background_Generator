// Background_Generator.cpp : Defines the entry point for the application.
//

#include "Background_Generator.h"

#include <iostream>

#include <png.h>
#include <zlib.h>

#define cimg_use_png 1
#include <CImg.h>
using cimg = cimg_library::CImg<unsigned char>;

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;



using namespace std;

int main(int argc, char* argv[])
{

	for (int i = 1; i < argc; i++) {
		Generate_Texture(argv[i]);
	}
	
	return 0;
}

//draw source onto target at (x, y), only altering pixels that are non-transparent in source
void draw_nontransparent(cimg* target, int x, int y, cimg* source) {
	cimg_forXY(*source, x1, y1) {
		if ((*source)(x1, y1, 0, 3) > 5) {
			(*target)(x + x1, y + y1, 0, 0) = (*source)(x1, y1, 0, 0);
			(*target)(x + x1, y + y1, 0, 1) = (*source)(x1, y1, 0, 1);
			(*target)(x + x1, y + y1, 0, 2) = (*source)(x1, y1, 0, 2);
			//target(x + x1, y + y1, 0, 3) = source(x1, y1, 0, 3);
		}
	}
}

void Generate_Texture(const char* filename) {

	std::ifstream lf = std::ifstream(_RELATIVE_PATH + std::string(filename) + ".json");
	json backgroundData = json::parse(lf);  //pull data from json file

	int tileW = backgroundData["TileWidth"];
	int tileH = backgroundData["TileHeight"];

	int tileSizeSS = backgroundData["TileSizeSS"];
	std::string tsfname = backgroundData["Tileset"];

	//create empty arrays
	int** tiles = new int* [tileW]; //to be filled with tileset index of each tile
	bool** tileData = new bool* [tileW]; //tileData[i][j] = true if tile is filled, false if empty
	for (int i = 0; i < tileW; i++) {
		tiles[i] = new int[tileH];
		tileData[i] = new bool[tileH];

		for (int j = 0; j < tileH; j++) {
			tiles[i][j] = -1;
			tileData[i][j] = false;
		}
	}


	int numTiles = 0;

	//fill out bool array with tiles to be filled
	for (json data : backgroundData["Tiles"]) {

		for (int i = data["StartX"]; i < data["EndX"]; i++) {
			for (int j = data["StartY"]; j < data["EndY"]; j++) {

				tileData[i][j] = true;

				numTiles++;
			}
		}
	}


	//neighbor_score int representation:
	  // left + 2*up + 4*right + 8*down + 16*topLeft + 32*topRight + 64*bottomLeft + 128*bottomRight

	//neighbor_score binary (bitwise) representation:
	  // bottomRight >> bottomLeft >> topRight >> topLeft >> down >> right >> up >> left 00010011

	std::map<int, int> neighborInTSIndexOut = {
		{ 0b10001100, 0 },  { 0b11001101, 1 },  { 0b01001001,  2 }, { 0b00001000, 3 },  { 0b01111111, 4 },  { 0b10111111, 5 },  { 0b11001111, 6 },
		{ 0b10101110, 7 },  { 0b11111111, 8 },  { 0b01011011, 9 },  { 0b00001010, 10 }, { 0b11011111, 11 }, { 0b11101111, 12 }, { 0b10101111, 13 },
		{ 0b00100110, 14 }, { 0b00110111, 15 }, { 0b00010011, 16 }, { 0b00000010, 17 }, { 0b01001111, 18 }, { 0b00101111, 19 }, { 0b01011111, 20 },
		{ 0b00000100, 21 }, { 0b00000101, 22 }, { 0b00000001, 23 }, { 0b00000000, 24 }, { 0b00011111, 25 }, { 0b10001111, 26 }, { 0b00111111, 27 },
		{ 0b00101110, 28 }, { 0b00011011, 29 }, { 0b01001101, 30 }, { 0b10001101, 31 }, { 0b00001100, 32 }, { 0b00001101, 33 }, { 0b00001001, 34 },
		{ 0b10001110, 35 }, { 0b01001011, 36 }, { 0b00010111, 37 }, { 0b00100111, 38 }, { 0b00001110, 39 }, { 0b00001111, 40 }, { 0b00001011, 41 },
		                    { -1, 43 },         { 0b10011111, 44 }, { 0b01101111, 45 }, { 0b00000110, 46 }, { 0b00000111, 47 }, { 0b00000011, 48 }
	};


	//store tileset index of each tile in tiles array, including transparent tiles
	for (int i = 0; i < tileW; i++) {
		for (int j = 0; j < tileH; j++) {
			if (!tileData[i][j]) {
				tiles[i][j] = neighborInTSIndexOut[-1];
				continue;
			}
			bool left = false;
			bool up = false;
			bool right = false;
			bool down = false;
			bool topLeft = false;
			bool topRight = false;
			bool bottomLeft = false;
			bool bottomRight = false;

			if (i - 1 < 0) {
				left = true;
				topLeft = true;
				bottomLeft = true;
			}
			if (j - 1 < 0) {
				up = true;
				topLeft = true;
				topRight = true;
			}
			if (i + 1 >= tileW) {
				right = true;
				topRight = true;
				bottomRight = true;
			}
			if (j + 1 >= tileH) {
				down = true;
				bottomLeft = true;
				bottomRight = true;
			}

			if (!left && tileData[i - 1][j]) left = true;
			if (!up && tileData[i][j - 1]) up = true;
			if (!right && tileData[i + 1][j]) right = true;
			if (!down && tileData[i][j + 1]) down = true;

			if (!topLeft && tileData[i - 1][j - 1]) topLeft = true;
			if (!topRight && tileData[i + 1][j - 1]) topRight = true;
			if (!bottomLeft && tileData[i - 1][j + 1]) bottomLeft = true;
			if (!bottomRight && tileData[i + 1][j + 1]) bottomRight = true;

			//sets up cases where corners are ignored to create duplicate bitmask for duplicate tiles
			//restrict bitmask to 47 possible values (?)
			if (!left || !up) topLeft = false;
			if (!left || !down) bottomLeft = false;
			if (!right || !up) topRight = false;
			if (!right || !down) bottomRight = false;


			int neighbor_score = left + 2 * up + 4 * right + 8 * down + 16 * topLeft + 32 * topRight + 64 * bottomLeft + 128 * bottomRight;


			tiles[i][j] = neighborInTSIndexOut[neighbor_score];
		}
	}

	//load tileset
	cimg* tileset = new cimg[49];

	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			tileset[i * 7 + j] = cimg((_RELATIVE_PATH + std::string("tilesets/") + tsfname).c_str());
			tileset[i * 7 + j].crop((j * tileSizeSS), (i * tileSizeSS), (j * tileSizeSS) + tileSizeSS - 1, ((i * tileSizeSS) + tileSizeSS - 1) );
		}
	}


	//create transparent png output image of appropriate size
	cimg level = cimg(tileW * tileSizeSS, tileH * tileSizeSS, 1, 4, 0);


	//draw backdrop
	if (backgroundData.contains("Backdrop")) {
		if (backgroundData["Backdrop"].contains("Color")) {
			level = cimg(tileW * tileSizeSS, tileH * tileSizeSS, 1, 4, 0);

			unsigned char a = 255;

			if (backgroundData["Backdrop"]["Color"].contains("A")) a = backgroundData["Backdrop"]["Color"]["A"];

			cimg_forXY(level, x, y) {
				level(x, y, 0, 0) = backgroundData["Backdrop"]["Color"]["R"];
				level(x, y, 0, 1) = backgroundData["Backdrop"]["Color"]["G"];
				level(x, y, 0, 2) = backgroundData["Backdrop"]["Color"]["B"];
				level(x, y, 0, 3) = a;
			}
		}
		if (backgroundData["Backdrop"].contains("Filename")) {

			//load backdrop textures
			std::string bdfname = backgroundData["Backdrop"]["Filename"];
			cimg backdrop = cimg((_RELATIVE_PATH + std::string("assets/") + bdfname).c_str());

			//resize texture and tile accordingly if backdrop is tiled
			if (backgroundData["Backdrop"].contains("Tiled") && backgroundData["Backdrop"]["Tiled"]){
				int w = backgroundData["Backdrop"]["Width"];
				int h = backgroundData["Backdrop"]["Height"];

				backdrop.resize(backgroundData["Backdrop"]["Width"], backgroundData["Backdrop"]["Height"], 1, 4, 3);
				
				int bgW = (tileW * tileSizeSS) / w;
				int bgH = (tileH * tileSizeSS) / h;

				for (int i = 0; i < bgW; i++) {
					for (int j = 0; j < bgH; j++) {
						draw_nontransparent(&level, i * w, j * h, &backdrop);
					}
				}

				int spareX = (tileSizeSS * tileW) - ((bgW) * (w));
				int spareY = (tileSizeSS * tileH) - ((bgH) * (h));

				if (spareX > 2) {
					cimg backdrop_trim = cimg(backdrop);
					for (int j = 0; j < bgH; j++) {
						backdrop_trim.crop(0, 0, spareX - 1,  h);
						draw_nontransparent(&level, bgW* w, j * h, &backdrop_trim);
					}
					if (spareY > 2) {
						backdrop_trim.crop(0, 0, spareX - 1, spareY - 1);
						
						draw_nontransparent(&level, bgW* w, bgH* h, &backdrop_trim);
						
					}
				}

				if (spareY > 2) {
					cimg backdrop_trim = cimg(backdrop);
					for (int i = 0; i < bgW; i++) {
						backdrop_trim.crop(0, 0, w, spareY - 1);
						draw_nontransparent(&level, i * w, bgH* h, &backdrop_trim);
					}
					if (spareX > 2) {
						backdrop_trim.crop(0, 0, spareX - 1, spareY - 1);
						draw_nontransparent(&level, bgW* w, bgH* h, &backdrop_trim);
					}
				}
			}
			else {
				backdrop.resize(tileW*tileSizeSS, tileH*tileSizeSS, 1, 4, 3);
				draw_nontransparent(&level, 0, 0, &backdrop);
			}
		}
	}

	//Draw bottom static assets
	for (json data : backgroundData["Assets"]) {

		if ((!data.contains("BelowTiles") || !data["BelowTiles"]) || (data.contains("Animated") && data["Animated"])) continue;

		std::string afname = data["Filename"];

		cimg assetTex = cimg((_RELATIVE_PATH + std::string("assets/") + afname).c_str());

		//resize texture if new dimensions are given
		if (data.contains("Width") && data.contains("Height")) assetTex.resize(data["Width"], data["Height"], 1, 4, 3);

		draw_nontransparent(&level, data["X"], data["Y"], &assetTex);

	}

	//draw each tile to image, don't bother drawing transparent tiles to prevent artifacts + save time
	for (int i = 0; i < tileW; i++) {
		for (int j = 0; j < tileH; j++) {
			if(tileData[i][j]) draw_nontransparent(&level, i*tileSizeSS, j*tileSizeSS, &tileset[tiles[i][j]]);
		}
	}

	//Draw top static assets
	for (json data : backgroundData["Assets"]) {

		if (data.contains("BelowTiles") && data["BelowTiles"] || (data.contains("Animated") && data["Animated"])) continue;

		std::string afname = data["Filename"];

		cimg assetTex = cimg((_RELATIVE_PATH + std::string("assets/") + afname).c_str());

		//resize texture if new dimensions are given
		if (data.contains("Width") && data.contains("Height")) assetTex.resize(data["Width"], data["Height"], 1, 4, 3);

		draw_nontransparent(&level, data["X"], data["Y"], &assetTex);

	}

	//save image to file
	level.save_png((_RELATIVE_PATH + std::string("textures/") + std::string(filename) + std::string(".png")).c_str());

	return;
}