#include "extern/raylib-cpp/include/raylib-cpp.hpp"
int main() {
  // Initialize window (width, height, title)
  raylib::Window window(800, 600, "Hello, raylib!");

  // Set frames per second (optional)
  SetTargetFPS(60);

  // Main game loop
  while (!window.ShouldClose()) {  // Detect window close button or ESC key

    // Update game state (e.g., move player, check inputs)

    // Begin drawing
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw game objects here
    DrawText("Hello, raylib in C++!", 190, 200, 20, LIGHTGRAY);

    EndDrawing();  // End drawing
  }

  // Automatically closes the window and cleans up
  return 0;
}