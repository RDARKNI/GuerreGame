# GUERRE

## Compilation Instructions

### Non-Cross Compilation (Unix-like Systems)

1. **Ensure Dependencies**: Make sure the `ncurses` library is installed on your system. It is typically pre-installed on most Unix-like systems.

2. **Create and Navigate to Build Directory**:
    ```bash
    mkdir build
    cd build
    ```

3. **Configure the Project with CMake**:
    ```bash
    cmake ..
    ```

4. **Build the Project**:
    ```bash
    make
    ```

### Cross-Compiling from Unix to Windows

1. **Install Zig**: Ensure you have Zig installed and added to your system’s PATH. If Zig is not on your PATH, specify the path directly in the compile command.

2. **Compile Using Zig**:
    ```bash
    zig c++ -o ./bin/windows/guerre.exe -I./extern/ ./src/main.cpp ./extern/libpdcurses.a ./src/input_parser.cpp ./src/ui/menu_ui.cpp ./src/ui/ncurses_setup.cpp ./src/game_model/pion.cpp ./src/game_model/game.cpp ./src/controller/controller.cpp ./src/controller/data_handler.cpp ./src/controller/network_helper.cpp -std=c++23 -target x86_64-windows -lws2_32
    ```

## Dependencies and Licensing

This project relies on the following external libraries:

- **ncurses**: Licensed under the [NCURSES License](https://www.gnu.org/software/ncurses/ncurses.html).
- **pdcurses**: Licensed under the [PDCurses License](https://pdcurses.sourceforge.net/).

Please review the respective licenses for more details.

## Additional Information

- **Repository Structure**: Your repository includes source code, a `CMakeLists.txt` for configuration, a static PDCurses library and a `README.md` with build instructions.
- **Building on Windows**: If you encounter issues with cross-compilation, ensure that all paths and dependencies are correctly specified and available.
