#include <iostream>
#include <raylib.h>
using namespace std;

int player1_score = 0;    // player scores
int player2_score = 0; 

const int winning_score = 5;  // max scores

Color color1 = { 251, 12, 103, 255 };  // colors
Color color2 = { 100, 255, 40, 255 };
Color color3 = { 173, 216, 230, 255 };
Color color4 = { 255, 202, 49, 255 };


// window height and width
const int screen_width = 1080;
const int screen_height = 600;

int half_width = screen_width / 2;
int half_height = screen_height / 2;

float ball_radius = 15;  // ball radius

// speed of ball
int speed_x = 7;
int speed_y = 7;

// paddle properties
float paddle_width = 25;
float paddle_height = 120;

// paddle speed 
int paddle_speed = 8;

// enums for gamestate
enum GameState {
    MENU,
    SETTINGS,
    USER_VS_USER,
    USER_VS_CPU,
    PAUSE,
    EXIT
};

// enum for gamemode
enum GameMode {
    USER_MODE,
    CPU_MODE
};

GameState gameState = MENU;
GameMode gameMode = USER_MODE;

// Win message
class Message {
public:
    void DrawVictoryMessage(const char* message) {
        const int screenWidth = GetScreenWidth();
        const int screenHeight = GetScreenHeight();

        Rectangle gradientRec = { screenWidth / 2 - 200, screenHeight / 2 - 50, 400, 100 };
        DrawRectangleGradientEx(gradientRec, color1, color2, color3, color4);

        DrawText(message, screenWidth / 2 - MeasureText(message, 40) / 2, screenHeight / 2 - 20, 40, WHITE);
    }
};


class GameElement {
public:
    virtual void Update() = 0; // Pure virtual function
    virtual void Draw() const = 0; // Pure virtual function
    virtual ~GameElement() = default;
};

class Ball : public GameElement {
private:
    float x, y;
    int speed_x, speed_y;
    const float radius;

public:
    Ball(float init_x, float init_y, float init_radius, int speedX, int speedY)
        : x(init_x), y(init_y), radius(init_radius), speed_x(speedX), speed_y(speedY) {
    }

    float getRadius() const {
        return radius;
    }

    float getX() const {
        return x;
    }

    float getY() const {
        return y;
    }

    void Draw() const override {
        DrawCircle(x, y, radius, color4);
    }

    void Update() override {
        x += speed_x;
        y += speed_y;

        if (y + radius >= GetScreenHeight() || y - radius <= 0) {
            speed_y *= -1;
        }
    }

    void Bounce() {
        speed_x *= -1;
    }

    void Reposition(float new_x, float new_y) {
        x = new_x;
        y = new_y;
    }

    void Reset(float init_x, float init_y) {
        x = init_x;
        y = init_y;
    }

    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choice[2]{ -1, 1 };
        speed_x = 7 * speed_choice[GetRandomValue(0, 1)];
        speed_y = 7 * speed_choice[GetRandomValue(0, 1)];
    }
};

class Paddle : public GameElement {
private:
    float x, y;
    const float width, height;
    const int speed;

public:
    Paddle(float init_x, float init_y, float init_width, float init_height, int init_speed)
        : x(init_x), y(init_y), width(init_width), height(init_height), speed(init_speed) {
    }

    void Draw() const override {
        DrawRectangleRounded(Rectangle{ x, y, width, height }, 0.5, 0, WHITE);
    }

    void Update() override {
        if (IsKeyDown(KEY_W)) {
            y -= speed;
        }
        if (IsKeyDown(KEY_S)) {
            y += speed;
        }

        if (y <= 0) {
            y = 0;
        }
        if (y + height >= GetScreenHeight()) {
            y = GetScreenHeight() - height;
        }
    }

    void Update(bool isLeft) {
        if (isLeft) {
            if (IsKeyDown(KEY_W)) {
                y -= speed;
            }
            if (IsKeyDown(KEY_S)) {
                y += speed;
            }
        }
        else {
            if (IsKeyDown(KEY_UP)) {
                y -= speed;
            }
            if (IsKeyDown(KEY_DOWN)) {
                y += speed;
            }
        }

        if (y <= 0) {
            y = 0;
        }
        if (y + height >= GetScreenHeight()) {
            y = GetScreenHeight() - height;
        }
    }

    void UpdateCPU(float ballY) {
        if (ballY < y + height / 2) {
            y -= speed;
        }
        if (ballY > y + height / 2) {
            y += speed;
        }

        if (y <= 0) {
            y = 0;
        }
        if (y + height >= GetScreenHeight()) {
            y = GetScreenHeight() - height;
        }
    }

    Rectangle getRectangle() const {
        return Rectangle{ x, y, width, height };
    }

    void Reset(float init_x, float init_y) {
        x = init_x;
        y = init_y;
    }
};

class Button {
public:
    Texture2D texture;
    Vector2 position;
    Vector2 size;
    const char* label;

    Button(const char* imagePath, Vector2 pos, const char* text);

    void Draw();

    bool IsClicked() const {
        Vector2 mousePoint = GetMousePosition();
        return CheckCollisionPointRec(mousePoint, { position.x, position.y, size.x, size.y }) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    ~Button();
};

Button::Button(const char* imagePath, Vector2 pos, const char* text) {
    Image image = LoadImage(imagePath);
    if (!IsImageReady(image)) {
        std::cerr << "Failed to load image. Ensure the file path is correct: " << imagePath << std::endl;
        exit(-1);
    }
    ImageResize(&image, 300, 70);
    texture = LoadTextureFromImage(image);
    UnloadImage(image);
    position = pos;
    size = { 300, 70 };
    label = text;
}

void Button::Draw() {
    Vector2 mousePoint = GetMousePosition();
    Color tint = WHITE;
    Color textColor = WHITE;

    if (CheckCollisionPointRec(mousePoint, { position.x, position.y, size.x, size.y })) {
        tint = GRAY;
        textColor = DARKGRAY;
    }

    DrawTexture(texture, position.x, position.y, tint);
    DrawText(label, position.x + (size.x - MeasureText(label, 20)) / 2, position.y + (size.y - 20) / 2, 20, textColor);
}

Button::~Button() {
    UnloadTexture(texture);
}

void ResetGame(Ball& ball, Paddle& left_paddle, Paddle& right_paddle) {
    player1_score = 0;
    player2_score = 0;
    ball.ResetBall();
    left_paddle.Reset(15, half_height - paddle_height / 2);
    right_paddle.Reset(screen_width - 40, half_height - paddle_height / 2);
}

void playGame() {
    Ball ball(half_width, half_height, ball_radius, speed_x, speed_y);
    Paddle left_paddle(15, half_height - paddle_height / 2, paddle_width, paddle_height, paddle_speed);
    Paddle right_paddle(screen_width - 40, half_height - paddle_height / 2, paddle_width, paddle_height, paddle_speed);

    Message msg;

    bool ballMissed = false;
    bool gameOver = false;
    double resetTime = 0;

    ResetGame(ball, left_paddle, right_paddle); // Reset the game state
    SetTargetFPS(90); 

    
    while (!WindowShouldClose() && gameState != MENU && gameState != EXIT) {
        if (IsKeyPressed(KEY_BACKSPACE)) {
            gameState = MENU;
            return;
        }
        
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (gameState != PAUSE) {
                gameState = PAUSE;
            }
            else {
                gameState = (gameMode == USER_MODE) ? USER_VS_USER : USER_VS_CPU;
            }
        }

        if (gameState == PAUSE) {
            BeginDrawing();
            ClearBackground(color3);
            DrawText("Game Paused", screen_width / 2 - MeasureText("Game Paused", 40) / 2, screen_height / 2 - 20, 40, WHITE);
            EndDrawing();
            continue;
        }

        if (!ballMissed) {
            ball.Update();
        }
        else if (GetTime() - resetTime > 2) {
            ballMissed = false;
        }

        // Update paddles
        if (gameMode == USER_MODE) {
            left_paddle.Update(true);  // Left paddle controlled by 'W' and 'S'
            right_paddle.Update(false);  // Right paddle controlled by 'UP' and 'DOWN'
        }
        else if (gameMode == CPU_MODE) {
            left_paddle.Update(true);  // Left paddle controlled by 'W' and 'S'
            right_paddle.UpdateCPU(ball.getY());  // Right paddle controlled by CPU
        }

        // Check collision with left paddle
        if (CheckCollisionCircleRec(Vector2{ ball.getX(), ball.getY() }, ball.getRadius(), left_paddle.getRectangle())) {
            ball.Bounce();
            ball.Reposition(left_paddle.getRectangle().x + left_paddle.getRectangle().width + ball.getRadius(), ball.getY());
        }

        // Check collision with right paddle
        if (CheckCollisionCircleRec(Vector2{ ball.getX(), ball.getY() }, ball.getRadius(), right_paddle.getRectangle())) {
            ball.Bounce();
            ball.Reposition(right_paddle.getRectangle().x - ball.getRadius(), ball.getY());
        }

        // Check if the ball is missed by the paddles
        if (ball.getX() - ball.getRadius() < 0) {
            ballMissed = true;
            resetTime = GetTime();
            // Increment player 2 score
            player2_score++;
            ball.Reset(half_width, half_height);
        }
        else if (ball.getX() + ball.getRadius() > screen_width) {
            ballMissed = true;
            resetTime = GetTime();
            // Increment player 1 score
            player1_score++;
            ball.Reset(half_width, half_height);
        }

        // Check for win condition
        if (player1_score >= winning_score || player2_score >= winning_score) {
            gameOver = true;
        }

        // Drawing during the game
        BeginDrawing();
        ClearBackground(color3);

        if (!gameOver) {
            ball.Draw();
            left_paddle.Draw();
            right_paddle.Draw();
            DrawLine(half_width, 0, half_width, screen_height, WHITE);
            DrawText(TextFormat("%i", player1_score), screen_width / 4 - 20, 20, 80, WHITE);
            DrawText(TextFormat("%i", player2_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);
        }
        else {
            if (player1_score >= winning_score) {
                msg.DrawVictoryMessage("Player 1 Wins!");
            }
            else if (player2_score >= winning_score) {
                msg.DrawVictoryMessage("Player 2 Wins!");
            }
        }

        EndDrawing();
    }
}

void HandleMenuAndSettings(Button& startButton, Button& settingsButton, Button& exitButton, Button& userButton, Button& cpuButton, Button& backButton) {
    if (gameState == MENU) {
        startButton.Draw();
        settingsButton.Draw();
        exitButton.Draw();

        bool toggle = true;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (startButton.IsClicked()) {
                gameState = USER_VS_USER;
                gameMode = USER_MODE;
                playGame();
            }
            else if (settingsButton.IsClicked()) {
                gameState = SETTINGS;
            }
            else if (exitButton.IsClicked()) {
                gameState = EXIT;
            }
        }
    }
    else if (gameState == SETTINGS) {
        userButton.Draw();
        cpuButton.Draw();
        backButton.Draw();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (userButton.IsClicked()) {
                gameState = USER_VS_USER;
                gameMode = USER_MODE;
                
            }
            else if (cpuButton.IsClicked()) {
                gameState = USER_VS_CPU;
                gameMode = CPU_MODE;
               
            }
            else if (backButton.IsClicked()) {
                gameState = MENU;
            }
        }
    }
}

int main() {
    InitWindow(screen_width, screen_height, "Pong Game");

    Button startButton("resources/images/bar1.png", { screen_width / 2 - 150, 150 }, "Start");
    Button settingsButton("resources/images/bar2.png", { screen_width / 2 - 150, 250 }, "Settings");
    Button exitButton("resources/images/bar5.png", { screen_width / 2 - 150, 350 }, "Exit");

    Button userButton("resources/images/bar4.png", { screen_width / 2 - 150, 200 }, "User");
    Button cpuButton("resources/images/bar5.png", { screen_width / 2 - 150, 300 }, "CPU");
    Button backButton("resources/images/bar5.png", { screen_width / 2 - 150, 400 }, "Back");

    while (!WindowShouldClose() && gameState != EXIT) {
        BeginDrawing();
        ClearBackground(color3);

        HandleMenuAndSettings(startButton, settingsButton, exitButton, userButton, cpuButton, backButton);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
