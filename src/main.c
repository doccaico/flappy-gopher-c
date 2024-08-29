#include "raylib.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"


typedef struct {
    char* key;
    Texture2D value;
}StringMap;

typedef enum {
    Title,
    Game_Play,
    Game_Over,
}Scene;

typedef struct {
    float x, y;
    int width, height;
}Gopher;

typedef struct {
    int wall_x;
    int hole_y;
}Wall;

typedef struct {
    Gopher gopher;
    float velocity;
    float gravity;
    int frames;
    int old_score;
    int new_score;
    const char* score_string;
    Wall* walls;
    Scene scene;
    bool retry;
}Game;


// Forward Declaration
void load_textures(const char* assets_path, const char** file_names, const char* suffix, size_t len);
void init_game (Game* game);
void draw_title(Game* game);
void draw_game(Game* game);
void draw_game_over(Game* game);

// Global Variables
StringMap* textures = NULL;


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 360

void draw_title(Game* game)
{
    DrawTexture(shget(textures, "sky"), 0, 0, WHITE);
    DrawText("Click!", (SCREEN_WIDTH / 2) - 40, SCREEN_HEIGHT / 2, 20, WHITE);
    DrawTexture(shget(textures, "gopher"), (int)game->gopher.x, (int)game->gopher.y, WHITE);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        game->scene = Game_Play;
    }
}

#define JUMP (-4.0)
#define INTERVAL 120 // 壁の追加間隔
#define WALL_START_X 640 // 壁の初期x座標
#define HOLE_Y_MAX 150 // 穴のy座標の最大値
#define GOPHER_WIDTH 60 // GOPHERの幅
#define GOPHER_HEIGHT 75 // GOPHERの高さ
#define HOLE_HEIGHT 170 // 穴のサイズ（高さ）
#define WALL_HEIGHT 360 // 壁の高さ
#define WALL_WIDTH 20 // 壁の幅

void draw_game(Game* game)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        game->velocity = JUMP;
    }
    game->velocity += game->gravity;
    game->gopher.y += game->velocity;

    game->frames += 1;
    if (game->frames % INTERVAL == 0) {
        int min = 0, max = HOLE_Y_MAX - 1;
        int rand_num = rand() % (max - min + 1) + min;
        arrput(game->walls, ((Wall){.wall_x = WALL_START_X, .hole_y = rand_num}));
    }

    // wallを左へ移動
    for (int i = 0; i < arrlen(game->walls); ++i) {
        game->walls[i].wall_x -= 2;
    }

    // スコアを計算
    for (int i = 0; i < arrlen(game->walls); ++i) {
        if (game->walls[i].wall_x < (int)game->gopher.x) {
            game->new_score = (int)i + 1;
        }
    }
    // スコアの文字列を生成
    if (game->new_score != game->old_score) {
        game->score_string = TextFormat("Score: %d", game->new_score);
        game->old_score = game->new_score;
    }

    DrawTexture(shget(textures, "sky"), 0, 0, WHITE);
    DrawTexture(shget(textures, "gopher"), (int)game->gopher.x, (int)game->gopher.y, WHITE);


    for (int i = 0; i < arrlen(game->walls); ++i) {
        int wall_x = game->walls[i].wall_x;
        int hole_y = game->walls[i].hole_y;
        float x = game->gopher.x;
        float y = game->gopher.y;

        // 上の壁の描画
        DrawTexture(shget(textures, "wall"), wall_x, hole_y - WALL_HEIGHT, WHITE);
        // 下の壁の描画
        DrawTexture(shget(textures, "wall"), wall_x, hole_y + HOLE_HEIGHT, WHITE);

        // gopherを表す四角形を作る
        int g_left = (int)x;
        int g_top = (int)y;
        int g_right = (int)x + GOPHER_WIDTH;
        int g_bottom = (int)y + GOPHER_HEIGHT;

        // 上の壁を表す四角形を作る
        int w_left = wall_x;
        int w_top = hole_y - WALL_HEIGHT;
        int w_right = wall_x + WALL_WIDTH;
        int w_bottom = hole_y;

        // 上の壁との当たり判定
        if (g_left < w_right && w_left < g_right && g_top < w_bottom && w_top < g_bottom) {
            game->scene = Game_Over;
        }

        // 下の壁を表す四角形を作る
        w_left = wall_x;
        w_top = hole_y + HOLE_HEIGHT;
        w_right = wall_x + WALL_WIDTH;
        w_bottom = hole_y + HOLE_HEIGHT + WALL_HEIGHT;

        // 下の壁との当たり判定
        if (g_left < w_right && w_left < g_right && g_top < w_bottom && w_top < g_bottom) {
            game->scene = Game_Over;
        }
    }

    // スコアを描画
    DrawText(game->score_string, 10, 10, 20, RED);

    // 上の壁との当たり判定
    if (game->gopher.y < 0) {
        game->scene = Game_Over;
    }
    // 地面との当たり判定
    if (360 - GOPHER_HEIGHT < game->gopher.y) {
        game->scene = Game_Over;
    }
}

void draw_game_over(Game* game)
{
    DrawTexture(shget(textures, "sky"), 0, 0, WHITE);
    DrawTexture(shget(textures, "gopher"), (int)game->gopher.x, (int)game->gopher.y, WHITE);

    for (int i = 0; i < arrlen(game->walls); ++i) {
        int wall_x = game->walls[i].wall_x;
        int hole_y = game->walls[i].hole_y;
        DrawTexture(shget(textures, "wall"), wall_x, hole_y - WALL_HEIGHT, WHITE);
        DrawTexture(shget(textures, "wall"), wall_x, hole_y + HOLE_HEIGHT, WHITE);
    }
    DrawText("Game Over", (SCREEN_WIDTH / 2) - 60, (SCREEN_HEIGHT / 2) - 60, 20, WHITE);

    game->score_string = TextFormat("Score: %d", game->new_score);
    DrawText(game->score_string, (SCREEN_WIDTH / 2) - 60, (SCREEN_HEIGHT / 2) - 40, 20, WHITE);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        game->scene = Title;
        arrfree(game->walls);
        init_game(game);
    }
}

void load_textures(const char* assets_path, const char** file_names, const char* suffix, size_t len)
{
    char path[32];
    for (int i = 0; i < len; ++i) {
        strcpy(path, assets_path);
        strcat(path, file_names[i]);
        strcat(path, suffix);

        Image asset = LoadImage(path);
        Texture2D texture = LoadTextureFromImage(asset);

        const char* key = file_names[i];
        shput(textures, key, texture);

        UnloadImage(asset);
    }
}

void init_game (Game* game)
{
    game->gopher = (Gopher){
        .x      = 200.0f,
        .y      = 150.0f,
        .width  = 60,
        .height = 75,
    };
    game->velocity = 0.0f;
    game->gravity = 0.1f;
    game->frames = 0;
    game->old_score = 0;
    game->new_score = 0;
    game->score_string = "Score: 0";
    game->scene = Title;
    game->walls = NULL;
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "flappy gopher");
    SetTargetFPS(60);

    const char* assets_path = "assets/";
    const char* file_names[] = {
        "gopher",
        "sky",
        "wall",
    };
    const char* suffix = ".png";
    load_textures(assets_path, file_names, suffix, sizeof(file_names) / sizeof(file_names[0]));

    Game game;
    init_game(&game);

    RenderTexture2D render_texture = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

    while (!WindowShouldClose()) {
        BeginTextureMode(render_texture);
        {
            switch (game.scene) {
                case Title:
                    draw_title(&game);
                    break;
                case Game_Play:
                    draw_game(&game);
                    break;
                case Game_Over:
                    draw_game_over(&game);
                    break;
                default:
                    break;
            }
        }
        EndTextureMode();

        BeginDrawing();
        {
            float w = (float)render_texture.texture.width;
            float h = (float)render_texture.texture.height;
            Rectangle source = {0, 0, w, h};
            Rectangle dest = source;
            source.height = -source.height;
            DrawTexturePro(
                    render_texture.texture,
                    source,
                    dest,
                    (Vector2){0.0f, 0.0f},
                    0,
                    WHITE
                    );
        }
        EndDrawing();
    }

    CloseWindow();

    UnloadRenderTexture(render_texture);
    shfree(textures);
    arrfree(game.walls);

    return 0;
}
