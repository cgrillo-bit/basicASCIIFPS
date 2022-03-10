// ascii_Render.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
/**1>All 178 functions were compiled because no usable IPDB/IOBJ from previous compilation was found.
1 > Finished generating code
1 > ascii_Render.vcxproj->C:\Users\grillo\source\repos\ascii_Render\Release\ascii_Render.exe
1 > Done building project "ascii_Render.vcxproj".
**/ 



#include <iostream>
#include <chrono>
#include <Windows.h>
#include <algorithm>
#include <vector>
using namespace std; 
int screenWidth = 120; 
int screenHeight = 40;

float playerX = 8.0f;
float playerY = 8.0f; 
float playerAn = 0.0f; 

int mapHeight = 16;
int mapWidth = 16; 

float FOV = 3.14159 / 4.00; 
float depth = 16.0f; 
int main()
{

    //generating screen buffer
    wchar_t* screen = new wchar_t[screenWidth * screenHeight]; 
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); 
    SetConsoleActiveScreenBuffer(hConsole); 
    DWORD bytesWritten = 0; 

    std::wstring map; 
    map += L"################";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#.........#....#";
    map += L"#.........#....#";
    map += L"#.......#......#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#.........######";
    map += L"#..............#";
    map += L"#..............#";
    map += L"################";

    auto time1 = chrono::system_clock::now();
    auto time2 = chrono::system_clock::now(); 



    //gamer loop
    while (1)
    {
        //calculating old system time vs new system time point and generated elapsed time as a floating point
        time2 = chrono::system_clock::now(); 
        chrono::duration<float> elapsedT = time2 - time1; 
        time1 = time2;
        float ElapsedT = elapsedT.count(); 


        //game controls in C++ annoy my soul
        //This dirty if also handles CCW
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
            playerAn -= (0.8f) * ElapsedT; 
 
        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
            playerAn += (0.8f) * ElapsedT;
           
        if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
        {
            playerX += sinf(playerAn) * 5.0f * ElapsedT;
            playerY += cosf(playerAn) * 5.0f * ElapsedT; 

            if (map[(int)playerY * mapWidth + (int)playerX] == '#')
            {
                playerX -= sinf(playerAn) * 5.0f * ElapsedT;
                playerY -= cosf(playerAn) * 5.0f * ElapsedT; 
            }
        }

        if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
        {
            playerX -= sinf(playerAn) * 5.0f * ElapsedT; 
            playerY -= cosf(playerAn) * 5.0f * ElapsedT; 

            if (map[(int)playerY * mapWidth + (int)playerX] == '#')
            {
                playerX += sinf(playerAn) * 5.0f * ElapsedT; 
                playerY += cosf(playerAn) * 5.0f * ElapsedT; 
            }
        }


        //creating artifical 3D FOV like wolfeinstein 
        for (int x = 0; x < screenWidth; x++)
        {
            //calculates projected ray angle into space. 
            float rayAngle = (playerAn - FOV / 2.0f) + ((float)x / (float)screenWidth) * FOV; 
            float distanceToWall = 0; 
            bool hitWall = false; 
            bool boundary = false; 

            float eyeX = sinf(rayAngle); //unit vector for player space 
            float eyeY = cosf(rayAngle); 
            while (!hitWall && distanceToWall < depth)
            {
                distanceToWall += 0.1f;

                int testX = (int)(playerX + eyeX * distanceToWall);
                int testY = (int)(playerY + eyeY * distanceToWall);

                //check and test for out of bounds 
                if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight)
                {
                    hitWall = true; // let distance to maximum depth
                    distanceToWall = depth; 
                }
                else
                {
                    if (map[testY * mapWidth + testX] == '#')
                    {
                        hitWall = true; 

                        vector<pair<float, float>> perfectCorner; 

                        for (int tx = 0; tx < 2; tx++)
                            for (int ty = 0; ty < 2; ty++)
                            {
                                float vy = (float)testY + ty - playerY;
                                float vx = (float)testX + tx - playerX;
                                float delta = sqrt(vx * vx + vy * vy); 
                                float dotProduct = (eyeX * vx / delta) + (eyeY * vy / delta); 
                                perfectCorner.push_back(make_pair(delta, dotProduct)); 
                            }
                        //sorts vector pairs from closest to farthest 
                        sort(perfectCorner.begin(), perfectCorner.end(), [](const pair<float, float>& left, const pair<float, float>& right) {return left.first < right.first; }); 
                        float bound = 0.01;
                        if(acos(perfectCorner[0].second) < bound) boundary = true; 
                        if (acos(perfectCorner[1].second) < bound) boundary = true; 
                        //if (acos(perfectCorner[2].second) < bound) boundary = true; 
                    }
                }
            }
            int ceiling = (float)(screenHeight / 2.0) - screenHeight / ((float)distanceToWall); 
            int floor = screenHeight - ceiling; 

            short shader = ' '; 

            //I know these characters are extended UNICODE so they will not work on some machines. 
            if (distanceToWall <= depth / 4.0f)
                shader = 0x2588; //Super close 
            else if (distanceToWall < depth / 3.0f)
                shader = 0x2593;
            else if (distanceToWall < depth / 2.0f)
                shader = 0x2592;
            else if (distanceToWall < depth)
                shader = 0x2591;
            else
                shader = ' '; ;//Too far away to care

            if (boundary)
                shader = ' '; //blacks out boundaries I think? 


            for (int y = 0; y < screenHeight; y++)
            {
                // Each Row
                if (y <= ceiling)
                    screen[y * screenWidth + x] = ' ';
                else if (y > ceiling && y <= floor)
                    screen[y * screenWidth + x] = shader;
                else // Floor
                {
                    // Shade floor based on distance
                    float b = 1.0f - (((float)y - screenHeight / 2.0f) / ((float)screenHeight / 2.0f));
                    if (b < 0.25)		shader = '#';
                    else if (b < 0.5)	shader = 'x';
                    else if (b < 0.75)	shader = '.';
                    else if (b < 0.9)	shader = '-';
                    else				shader = ' ';
                    screen[y * screenWidth + x] = shader; 
                }
            }
        }
        //top left stats display (position and FPS)
        swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", playerX, playerY, playerAn, 1.0f /ElapsedT);

        for (int nx = 0; nx < mapWidth; nx++)
            for (int ny = 0; ny < mapWidth; ny++)
            {
                screen[(ny + 1) * screenWidth + nx] = map[ny * mapWidth + nx]; 
            }

        screen[((int)playerY + 1) * screenWidth + (int)playerX] = 'N'; 

       screen[screenWidth * screenHeight - 1] = '/0'; 
       WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, { 0, 0 }, & bytesWritten); 
    }
    return 0; 
}

