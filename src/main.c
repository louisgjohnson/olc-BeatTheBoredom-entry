#include "raylib.h"
#include <time.h>
#include <stdlib.h>
#include <string.h> 
#include <stdio.h>


#define SCALE (1)
#define TILE_SIZE (64)
#define PADDING (32)
#define TILE_SIZE_SCALE (TILE_SIZE * SCALE) 
#define Y_OFFSET (0 * 4)
#define ITEM_LENGTH (5)
#define SCREEN_SIZE (TILE_SIZE_SCALE * 10)
#define COLOUR ((Color) { r: 34, g: 35, b: 35, a: 255 })
#define ASSET_PATH "assets/"

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    } while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

Rectangle pick_by_index(int x, int y) {
    return (Rectangle) {
        x: x * TILE_SIZE + x * PADDING + PADDING,
        y: y * TILE_SIZE + y * PADDING + PADDING, 
        width: TILE_SIZE,
        height: TILE_SIZE
    };
}

typedef struct TexturesStorage {
    char *keys[50];
    Texture2D textures[50];
    int counter;

} TexturesStorage;



TexturesStorage InitTextureStorage(char *keys[50]) {
    TexturesStorage texture_storage = {
        counter: 0
    };

    for (int i = 0; i > 50; i++) {

    }
}

void AddTexture(TexturesStorage *t, char *key, Texture2D texture) {
    t->keys[t->counter] = key;
    t->textures[t->counter] = texture;
    t->counter++;
}

Texture GetTexture(TexturesStorage *t, char *key) {
    for (int i = 0; i < t->counter; ++i) {
        
        if(!strcmp(t->keys[i], key))
        {
            return t->textures[i];
        }
    }
    
}

void FreeTextureStorage(TexturesStorage *t) {
    for (int i = 0; i < t->counter; i++) {
        UnloadTexture(t->textures[i]);
    }
}

void DrawTextureRecEx(Texture2D texture, Rectangle sourceRec, Vector2 position, float scale, Color tint)
{
    Rectangle destRec = { position.x, position.y, (float)sourceRec.width*scale, (float)sourceRec.height*scale };
    Vector2 origin = { 0.0f, 0.0f };
    DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, tint);
}

Image MuteColorInImage(Image image, Color color) {
    Color *pixels = GetImageData(image);
    for (int i = 0, k = 0; i < image.width*image.height; i++) {
        if (pixels[i].r == color.r && pixels[i].g == color.g && pixels[i].b == color.b) {
            pixels[i].a = 0;
        }
    }
    Image i = LoadImageEx(pixels, image.width, image.height);
    UnloadImage(image);
    return i;
}


int main()
{
    srand(time(NULL));
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(SCREEN_SIZE, SCREEN_SIZE + Y_OFFSET, "Window title");

    // Texture2D texture = LoadTexture(ASSET_PATH"tilemap.png");
    SetTargetFPS(60);
    TexturesStorage texture_storage = (TexturesStorage) {
        counter: 0
    };

    AddTexture(&texture_storage, "tilemap", LoadTexture(ASSET_PATH"tilemap.png"));
    AddTexture(&texture_storage, "ui", LoadTexture(ASSET_PATH"ui.png"));
    AddTexture(&texture_storage, "open_hand", LoadTexture(ASSET_PATH"open_hand.png"));
    AddTexture(&texture_storage, "closed_hand", LoadTexture(ASSET_PATH"closed_hand.png"));
    HideCursor();


    int current_level = 0;
    int current_points = 0;
    

    int selected = 0;
    bool selected_item = false;

    Rectangle items[ITEM_LENGTH + 1];
    {
        items[1] = pick_by_index(5, 3); // TREE
        items[2] = pick_by_index(7, 3); // TREE 2
        items[3] = pick_by_index(5, 6); // HOUSE
        items[4] = pick_by_index(9, 6); // FACTORY
        items[5] = pick_by_index(10, 6); // SHOP  
    }
    
    int item_spots[10][10] = {{ 0 }};

    int points_caps[5];
    points_caps[0] = 100;
    points_caps[1] = 200;
    points_caps[2] = 300;
    points_caps[3] = 400;
    points_caps[4] = 500;
    

    Vector2 positions[ITEM_LENGTH + 1];
    int bar_width = GetTexture(&texture_storage, "ui").width;
    int spacing = bar_width / 5;

    {
        for (int i = 1; i <= ITEM_LENGTH; i++) {
            int initial = SCREEN_SIZE / 2 - bar_width / 2;
            positions[i] = (Vector2) { x: initial + 15 + spacing * (i-1), y: 495};
        }
    }

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        Vector2 mousePosWithOffset = (Vector2) {
            x: GetMousePosition().x - (TILE_SIZE / 2),
            y: GetMousePosition().y - (TILE_SIZE / 2)
        };
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                for (int i = 1; i < ITEM_LENGTH + 1; i++) {
                    Rectangle rec = (Rectangle) {
                        x: positions[i].x - 10,
                        y: positions[i].y - 10,
                        width: items[i].width + 20,
                        height: items[i].height + 20
                    };
                    if (CheckCollisionPointRec(GetMousePosition(), rec)) {
                        selected = i;
                        selected_item = true;
                        break;
                    }
                }
            }
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                if (selected_item) {
                    for (int x = 0; x < 10; x++) {
                        for (int y = 0; y < 10; y++) {
                            Rectangle rec = (Rectangle) {
                                x: TILE_SIZE * x,
                                y: TILE_SIZE * y,
                                width: TILE_SIZE,
                                height: TILE_SIZE
                            };
                            if (CheckCollisionPointRec(GetMousePosition(), rec)) {
                                item_spots[x][y] = selected;
                            }
                        }
                    }
                }
                
                
                selected_item = false;
            }

            if (current_points > points_caps[current_level]) {
                current_level++;
            }
        }
        // Draw
        {
            BeginDrawing();
            ClearBackground(COLOUR);
            for (int x = 0; x < 10; x++) {
                for (int y = 0; y < 10; y++) {
                    int r = rand();
                    int i = (x + y) % 2; 
                    Vector2 pos = (Vector2) { x: TILE_SIZE * x, y: Y_OFFSET + (y * TILE_SIZE)};
                    DrawTextureRecEx(GetTexture(&texture_storage, "tilemap"), pick_by_index(i, 0), pos, SCALE, WHITE);
                    if (item_spots[x][y]) {
                        Rectangle item = items[item_spots[x][y]];
                        DrawTextureRecEx(GetTexture(&texture_storage, "tilemap"), item, pos, SCALE, WHITE);
                    }
                    if (selected_item) {
                        Rectangle rec = (Rectangle) {
                            x: pos.x,
                            y: pos.y,
                            width: TILE_SIZE,
                            height: TILE_SIZE
                        };
                        if (CheckCollisionPointRec(GetMousePosition(), rec)) {
                            Color base = item_spots[x][y] ? RED :  YELLOW;
                            Color boxColour = (Color) {
                                r: base.r,
                                g: base.g,
                                b: base.b,
                                a: 55
                            };
                            DrawRectangleRounded(rec, 0.1, 0, boxColour);
                            Color borderColour = (Color) {
                                r: base.r,
                                g: base.g,
                                b: base.b,
                                a: 125
                            };
                            DrawRectangleRoundedLines((Rectangle) {
                            x: pos.x + 3,
                            y: pos.y + 3,
                            width: TILE_SIZE - 6,
                            height: TILE_SIZE - 6
                        }, 0.1, 0, 3, borderColour);
                        }
                        
                    }
                }
            }
            Texture2D ui_texture = GetTexture(&texture_storage, "ui");
            int x = SCREEN_SIZE / 2 - ui_texture.width / 2;
            DrawTexture(ui_texture, x, 480, WHITE);
            

            if (selected_item) {
                // DrawRectangle(GetMousePosition().x / 10, GetMousePosition().y / 10, TILE_SIZE, TILE_SIZE, YELLOW);
                DrawTextureRecEx(GetTexture(&texture_storage, "tilemap"), items[selected], mousePosWithOffset, SCALE, WHITE);
            }

            for (int i = 1; i <= ITEM_LENGTH; i++) {
                DrawTextureRecEx(GetTexture(&texture_storage, "tilemap"), items[i], positions[i], SCALE, WHITE);
            }

            // UI
            char one[50], two[50], three[50];

            itoa(current_points, one);
            strcpy(two, "/");
            itoa(points_caps[current_level], three);
            strcat(one, two);
            strcat(one, three);

            DrawText(one, 10, 10, 30, WHITE);

            DrawTextureEx(IsMouseButtonDown(MOUSE_LEFT_BUTTON) && selected ? GetTexture(&texture_storage, "closed_hand") : GetTexture(&texture_storage, "open_hand"), GetMousePosition(), 0, 1, WHITE);
            EndDrawing();
        }
    }

    // De-Initialization
    FreeTextureStorage(&texture_storage);
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context

    return 0;
}