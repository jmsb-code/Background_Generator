Lightweight map texture generator using vendored image libraries (CImg using libpng and by extension zlib, as well as the nlohmann json parsing package, with aesprite 47-tilesets. 
ZLib may give you compilation errors, install it globally if this is the case and that should fix the program

Input formatting:
    To process a level, create a json file named your_level_name.json in root/levels

    The json file must share the name of its directory in root/levels and should follow the following structure:	
        {
            "TileWidth": $, = number of tiles in the x direction
            "TileHeight": $, = number of tiles in the y direction
            "Tileset": "$$$.png", = name of the tileset png file*
            "TileSizeSS": $, = size of each tile as on tileset, in pixels
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
  There is a sample level file in the root/example directory named 'example.json'.
  The corresponding output png file 'example.png' is provided in the same directory, as generated with 'tileset.png'.

To test the program with the example:
  Copy the example.json file to root/levels and copy 'tileset.png' to levels/tilesets as inputs
  Run the program with 'example' as an argument to generate a copy of 'example.png'.

Happy map designing!
    -James ~\(^o^)/~
