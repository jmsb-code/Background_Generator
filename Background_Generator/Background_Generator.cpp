// Background_Generator.cpp : Defines the entry point for the application.
//

#include "Background_Generator.h"

#include <png.h>
#include <zlib.h>

#define cimg_use_png 1
#include <CImg.h>
using cimg = cimg_library::CImg<unsigned char>;

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


using namespace std;

int main()
{

	Background bg = Background("test");
	cout << bg.tiles[5][12] << endl;
	return 0;
}

Background::Background(const char* filename) {
	std::string lvlFilename = _RELATIVE_PATH + std::string("levels/") + std::string(filename) + std::string("/") + std::string(filename) + ".json";

	std::ifstream lf = std::ifstream(lvlFilename);
	bool fileExists = lf.good();
	fileExists = fileExists && !lf.eof();
	fileExists = fileExists && lf.is_open();
	json backgroundData = json::parse(lf);  //pull data from json file

	tileW = backgroundData["TileWidth"];
	tileH = backgroundData["TileHeight"];
	int tileSize = backgroundData["TileSize"];

	int tileSizeSS = backgroundData["TileSizeSS"];
	tsfname = backgroundData["Tileset"];
	ssID = backgroundData["TilesetSS"];

	//create array of empty tiles
	tiles = new int* [tileW];
	bool** tileData = new bool* [tileW];
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
	  // bottomRight >> bottomLeft >> topRight >> topLeft >> down >> right >> up >> left

	std::map<int, int> neighborInTSIndexOut = {
		{ 0b10001100, 0 },  { 0b11001101, 1 },  { 0b01001001,  2 }, { 0b00001000, 3 },  { 0b01111111, 4 },  { 0b10111111, 5 },  { 0b11001111, 6 },
		{ 0b10101110, 7 },  { 0b11111111, 8 },  { 0b01011011, 9 },  { 0b00001010, 10 }, { 0b11011111, 11 }, { 0b11101111, 12 }, { 0b10101111, 13 },
		{ 0b00100110, 14 }, { 0b00110111, 15 }, { 0b00100011, 16 }, { 0b00000010, 17 }, { 0b01001111, 18 }, { 0b00101111, 19 }, { 0b01011111, 20 },
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

	for (json data : backgroundData["Assets"]) {
	}

	//free memory from bool arrays
	for (int i = 0; i < tileW; i++) {
		delete[] tileData[i];
	}
	delete[] tileData;

	//Flag that textures have not been loaded for this background's tiles
	texturesSet = false;


}