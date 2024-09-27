#include<iostream>
#include<raylib.h>
#include <deque>
#include<raymath.h>

using namespace std;

Color green = {172, 204, 96, 255}; //RGB AND ALPHA
Color darkGreen = {43, 51, 24, 255};

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool eventTriggered(double interval)
{
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval){
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

//returns true if element is in deque(for spawning food properly)
bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for(unsigned int i=0; i < deque.size(); i++){
        if(Vector2Equals(element,deque[i])){
            return true;
        }
    }
    return false;
}

class Snake{
    public:
    deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
    Vector2 direction = {1,0};//Moving right
    bool addSegment = false;

    void Draw()
    {
        for(unsigned int i = 0; i < body.size(); i++){
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset + x * cellSize,offset +  y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment,0.5, 0.6, darkGreen);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if(addSegment == true){//takes care of eating food
            
            addSegment = false;
        }
        else
        {//takes care of moving the snake
            body.pop_back();
        }
        
    }

    void Reset()
    {
        body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        direction = {1,0};

    }



};

class Food{


    public:
        Vector2 position; //struct Vector2 {x coord, y coord} => dont represent pixels but instead represent cells
        Texture2D texture;

        //Constructor
        Food(deque<Vector2> snakeBody){
            Image image = LoadImage("Graphics/food.png");
            texture = LoadTextureFromImage(image);
            UnloadImage(image); //Basically Free
            position = GenerateRandomPos(snakeBody);
        }

        //Destructor
        ~Food(){
            UnloadTexture(texture);
        }

        //Drawing object
        void Draw()
        {
            DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
        }

        Vector2 GenerateRandomCell()
        {
            float x = GetRandomValue(0, cellCount - 1);
            float y = GetRandomValue(0, cellCount - 1);
            return Vector2{x, y};
        }
        
        Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
        {
            
            Vector2 position = GenerateRandomCell();
            while(ElementInDeque(position, snakeBody))
            {
                position = GenerateRandomCell();
            }
            return position;
            
        }


};

class Game{
    public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score =0;
    Sound eatSound;
    Sound wallSound;

    Game(){
        InitAudioDevice();//sets up audio sys to play sound
        eatSound = LoadSound("Sounds/eat.mp3");
        wallSound = LoadSound("Sounds/wall.mp3");
    }
    ~Game(){
      
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw()
    {
        food.Draw();
        snake.Draw();
    }

    void Update()
    {
        if(running)
        {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
    }

        void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandomPos(snake.body);
            score++;
            snake.addSegment = true;
            PlaySound(eatSound);
        }
    }

    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wallSound);

    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }


};

int main(){
    cout<<"Started snake game"<<endl;
    InitWindow(2*offset + cellCount * cellSize, 2* offset + cellCount * cellSize,"Snake Game");
    SetTargetFPS(120);

    Game game = Game();

    while(WindowShouldClose() == false)
    {
        BeginDrawing();

        if(eventTriggered(0.11) == true){
            game.Update();
        }
        //moving the snake
        if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
        {
            game.snake.direction = {0, -1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.snake.direction = {0, 1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
        {
            game.snake.direction = {-1, 0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
        {
            game.snake.direction = {1, 0};
            game.running = true;
        }

        //Drawing
        ClearBackground(green); //BG is cleaned and painted green
        DrawRectangleLinesEx(Rectangle{(float)offset-5,(float) offset -5, (float)cellSize * cellCount+10, (float) cellSize * cellCount+10},5, darkGreen); //rect, thickness, color
        DrawText("Retro Snake", offset - 5, 20, 40,darkGreen );//text, posx, posy, fontsize, color
        DrawText(TextFormat("%i",game.score), offset - 5, offset + cellSize*cellCount + 10, 40,darkGreen );
        game.Draw();



        EndDrawing();
    }



    CloseWindow();
    return 0;
}