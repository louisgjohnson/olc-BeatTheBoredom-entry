#include "raylib.h"


#define SCALE (8)

#define TILE_SIZE (8 * SCALE) 

#define SCREEN_SIZE (TILE_SIZE * 10)

#define COLOUR ((Color) { r: 33, g: 34, b: 34, a: 255 })

// Change this depending on the path of your executable relative to the assets folder
#define ASSET_PATH "assets/"

Rectangle pick_by_index(int x, int y) {
    return (Rectangle) {
        x: x * TILE_SIZE,
        y: y * TILE_SIZE, 
        width: TILE_SIZE,
        height: TILE_SIZE
    };
}

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(SCREEN_SIZE, SCREEN_SIZE, "Window title");

    Image image = LoadImage(ASSET_PATH"tilemap.png");
    ImageResizeNN(&image, image.width * SCALE, image.height * SCALE);
    Texture2D texture = LoadTextureFromImage(image);
    SetTargetFPS(60);    
    
    Vector2 position = (Vector2) { x: 0, y: 0 };
    bool did_player_move = false;

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        {
            Vector2 pot_position = position;
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) { pot_position.x-= TILE_SIZE; }
            else if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) { pot_position.x+= TILE_SIZE; }
            else if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) { pot_position.y-= TILE_SIZE; }
            else if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) { pot_position.y+= TILE_SIZE; }

            if (pot_position.x < SCREEN_SIZE && pot_position.y < SCREEN_SIZE && pot_position.x >= 0 && pot_position.y >= 0) {
                did_player_move = position.x != pot_position.x || position.y != pot_position.y;
                position = pot_position;
            }
        }

        // Draw
        {
            BeginDrawing();
            ClearBackground(COLOUR);
            
            DrawTextureRec(texture, pick_by_index(4, 0), position, WHITE);
            EndDrawing();
        }
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context

    return 0;
}