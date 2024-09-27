#include<iostream>
#include<raylib.h>
#include<raymath.h>
using namespace std;

int playerScore = 0;
int cpuScore = 0;

Color green = Color{38,185,154,255};
Color darkGreen = Color{20,160,133,255};
Color lightGreen = Color{129,204,184,255};
Color yellow = Color{243,213,91,255};

class Ball
{
    public:
        float x ,y;
        int speed_x, speed_y;
        int radius;

        void Draw(){
            DrawCircle(x, y, radius, yellow);


        }
        void Update(){
            x +=speed_x;
            y +=speed_y;

            if(y + radius >= GetScreenHeight() || y - radius <=0 )
            {
                speed_y *= -1;
            }
            if(x + radius >= GetScreenWidth() )//cpu wins
            {
                cpuScore++;
                resetBall();
            }
            
            if(x - radius <=0 )//player wins
            {
                playerScore++;
                resetBall();
            }
        }

        void resetBall()
        {
            x = GetScreenWidth() /2;
            y = GetScreenHeight()/2;

            int speedChoices[2] = {-1, 1};
            speed_x *= speedChoices[GetRandomValue(0,1)];
            speed_y *= speedChoices[GetRandomValue(0,1)];


        }


};

class Paddle
{
    //Abstraction!
    protected:
        void LimitMovement()
        {
            if(y <= 0)
            {
                y = 0;
            }
            if(y + height >= GetScreenHeight())
            {
                y = GetScreenHeight() - height;
            }
        }


    public:
        float x ,y;
        float width, height;
        int speed;

        void Draw()
        {
            DrawRectangleRounded(Rectangle{x,y,width,height}, 0.8, 0, WHITE);


        }
        void Update()
        {
            if(IsKeyDown(KEY_UP))
            {
                y = y - speed;
            }
            if(IsKeyDown(KEY_DOWN))
            {
                y = y + speed;
            }
            LimitMovement();

        }

};

//Inheritannce example!
class CpuPaddle : public Paddle
{
    public:
    //Method overriding example!
    void Update(int ball_y)
    {
        if(y + height/2 > ball_y)
        {
            y = y - speed;
        }
        if(y + height/2 < ball_y)
        {
            y = y + speed;
        }
        LimitMovement();
    }

};

Ball ball;
Paddle player;
CpuPaddle cpu;

int main(){
    cout << "Starting Pong Game!!!" << endl;
    const int screenWidth = 1280;
    const int screenHeight = 800;


    InitWindow(screenWidth,screenHeight,"Pong game");
    SetTargetFPS(90); 
    //Initializing all values
    ball.radius = 20;
    ball.x = screenWidth/2;
    ball.y = screenHeight/2;
    ball.speed_x = 6;
    ball.speed_y = 6;

    player.width =25;
    player.height = 120;
    player.x = screenWidth - player.width - 10;
    player.y = screenHeight/2 - player.height/2;
    player.speed = 4;

    cpu.width =25;
    cpu.height = 120;
    cpu.x = 10;
    cpu.y = screenHeight/2 - cpu.height/2;
    cpu.speed = 4;


    //game loop
    while(WindowShouldClose() == false)
    {
        BeginDrawing();

        //Updates
        player.Update();
        ball.Update();
        cpu.Update(ball.y);

        //Collision check
        if(CheckCollisionCircleRec(Vector2{ball.x, ball.y},ball.radius, Rectangle{player.x, player.y, player.width, player.height}))
        {
            ball.speed_x  *= -1;
        }
        if(CheckCollisionCircleRec(Vector2{ball.x, ball.y},ball.radius, Rectangle{cpu.x, cpu.y, cpu.width, cpu.height}))
        {
            ball.speed_x  *= -1;
        }

        //Drawing
        ClearBackground(darkGreen);
        DrawRectangle(screenWidth/2, 0, screenWidth , screenHeight,green);//Player half color
        DrawCircle(screenWidth/2 ,screenHeight/2, 150, lightGreen);
        DrawLine(screenWidth/2, 0, screenWidth/2, screenHeight, WHITE);//int startPosX, int startPosY, int endPosX, int endPosY, Color color
        player.Draw();
        cpu.Draw();
        ball.Draw();
        
        
        DrawText(TextFormat("%i",cpuScore) , screenWidth/4 - 20, 20, 80,WHITE);
        DrawText(TextFormat("%i",playerScore) , 3*screenWidth/4 - 20, 20, 80,WHITE);
        

        EndDrawing();
    }



 
    CloseWindow();
    return  0;
}