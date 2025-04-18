Lightweight map texture generator using vendored image libraries (CImg using libpng and by extension zlib, as well as the nlohmann json parsing package).
Program takes json as input and bakes together tiles from an aesprite 47-tileset with other static images to create in-game level backdrops. 

Input formatting:
    To process a level, create a json file named your_level_name.json in root/levels

    The json file must share the name of its directory in root/levels and should fill the following fields:	
        {
            "TileWidth": $, <- number of tiles in the x direction
            "TileHeight": $, <- number of tiles in the y direction
            "Tileset": "$$$.png", <- name of tileset png file located in levels/tilesets*
            "TileSizeSS": $, <- size of each tile as on tileset, in pixels
            "Tiles": [
                {
                    "StartX": $ (inclusive),
                    "EndX": $ (inclusive),
                    "StartY": $ (exclusive),
                    "EndY": $ (exclusive)
                },
                {
                    .
                    .
                    .
                    .
                }
            ],
            "Assets": [
                {
                    "X": $, <- x position in pixels of topleft corner of asset render position
                    "Y": $, <- y position in pixels of topleft corner of asset render position
                    "Width": $, <- optional, width to resize to
                    "Height": $, <- optional, height to resize to
                    "Filename": "$$$.png", <- name of asset png file located in levels/assets
                    "BelowTiles": true/false <- optional, default is false

                },
                {
                    .
                    .
                    .
                    .
                }
            ]
        }

  *  The "Tileset" field must be the full name of a png file located in root/levels/tilesets
     Tilesets should follow the aesprite 47-tileset format. 
     Example tileset 'tileset.png' is provided for you the example directory.
     Credit to Stuart, the original author of this tileset.


How to use:
    1. Create a root/build* directory and navigate to it.

    2. Run CMake to configure the project:
       ```
       cmake ..
       ```  

    2.1. If you experience build issues see note on Zlib at the end of this file.

    3. navigate to the build/src directory and run the program:
       ```
       cd src
       ./Background_Generator <level_name1> <level_name2> ...
       ```

    4. The program will generate a texture for each level name provided as an argument.

    5. A file with the name `<level_name>.png` will be generated into the root/levels*/textures directory.

  *  If you want to build to a different directory or use a different directory for input/output, 
     edit the variable _RELATIVE_PATH defined in root/CMakeLists.txt accordingly.
     It must define the path from the executable to the input/output directory; 'root/build/src --> root/levels' by default

Example level:
  There is a sample level called 'example' with necessary files provided for you in the appropriate directories
  A copy of the example level directory is provided in root/example

To test the program with the example:
  Make sure example.json is in root/levels, 'tileset.png' is in levels/tilesets, and 'clouds.png' and 'asset.png' are in levels/assets
  Run the program with 'example' as an argument to rewrite 'example.png' into levels/textures.


If ZLib gives you compilation errors:
  Navigate to root/vendor and run the following command:
    ```
    cmake -S zlib-1.3.1 -B build-zlib -DCMAKE_INSTALL_PREFIX=${PWD}/zlib-install  -DBUILD_SHARED_LIBS=OFF
    ```
  Then, run the following:
    ```
    cmake --build build-zlib --target install
    ```
  
  If the program still won't compile, install zlib using your package manager and remove set(ZLIB_LIBRARY) and set(ZLIB_INCLUDE_DIR) from vendor/CMakeLists.txt
                                                                                      
Good luck and happy map designing!
    -James ~\(^o^)/~
