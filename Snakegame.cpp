#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <cstdlib>

using namespace std;

/* ---------- Keyboard Handling for macOS ---------- */

bool kbhit()
{
    termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return true;
    }
    return false;
}

char getch()
{
    char buf = 0;
    termios old = {0};

    tcgetattr(STDIN_FILENO, &old);
    old.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &old);

    read(STDIN_FILENO, &buf, 1);

    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSADRAIN, &old);

    return buf;
}

/* ---------- Game Variables ---------- */

bool gameOver;
const int width = 20;
const int height = 20;
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100];
int nTail;

enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

/* ---------- Game Functions ---------- */

void Setup()
{
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    nTail = 0;
}

void Draw()
{
    system("clear");

    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j == 0)
                cout << "#";

            if (i == y && j == x)
                cout << "🐍";
            else if (i == fruitY && j == fruitX)
                cout << "🍑";
            else
            {
                bool print = false;
                for (int k = 0; k < nTail; k++)
                {
                    if (tailX[k] == j && tailY[k] == i)
                    {
                        cout << "❗️";
                        print = true;
                        break;
                    }
                }
                if (!print)
                    cout << " ";
            }

            if (j == width - 1)
                cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    cout << "Score: " << score << endl;
    cout << "Controls: W A S D | X to quit" << endl;
}

void Input()
{
    if (kbhit())
    {
        switch (getch())
        {
        case 'a': dir = LEFT;  break;
        case 'd': dir = RIGHT; break;
        case 'w': dir = UP;    break;
        case 's': dir = DOWN;  break;
        case 'x': gameOver = true; break;
        }
    }
}

void Logic()
{
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;

    tailX[0] = x;
    tailY[0] = y;

    for (int i = 1; i < nTail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir)
    {
    case LEFT:  x--; break;
    case RIGHT: x++; break;
    case UP:    y--; break;
    case DOWN:  y++; break;
    default: break;
    }

    if (x >= width) x = 0; else if (x < 0) x = width - 1;
    if (y >= height) y = 0; else if (y < 0) y = height - 1;

    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;

    if (x == fruitX && y == fruitY)
    {
        score += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
        nTail++;
    }
}

/* ---------- Main ---------- */

int main()
{
    Setup();
    while (!gameOver)
    {
        Draw();
        Input();
        Logic();
        usleep(100000);   // 100 ms delay
    }

    system("clear");
    cout << "Game Over!" << endl;
    cout << "Final Score: " << score << endl;
    return 0;
}
