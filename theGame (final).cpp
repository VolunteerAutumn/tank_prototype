#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <ctime>
#include <algorithm>

using namespace std;

const int WIDTH = 10;
const int HEIGHT = 20;

void gotoXY(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

class Point
{
    int x, y;

public:
    Point(int x = 0, int y = 0) : x(x), y(y) {}

    int getX() { return x; }
    int getY() { return y; }

    void set(int nx, int ny)
    {
        x = nx;
        y = ny;
    }

    void move(int dx, int dy)
    {
        x += dx;
        y += dy;
    }
};

class Bullet
{
    Point pos;
    bool active;

public:
    Bullet(int x, int y) : pos(x, y), active(true) {}

    bool isActive() { return active; }

    int getX() { return pos.getX(); }
    int getY() { return pos.getY(); }

    void deactivate() { active = false; }

    void move()
    {
        pos.move(0, -1);
        if (pos.getY() < 0)
            active = false;
    }
};

class Enemy
{
    Point pos;

public:
    Enemy()
    {
        respawn();
    }

    void respawn()
    {
        pos.set(rand() % WIDTH, 0);
    }

    void fall()
    {
        pos.move(0, 1);
    }

    int getX() { return pos.getX(); }
    int getY() { return pos.getY(); }
};

class Game
{
    int tankX;
    int score;

    vector<Bullet> bullets;
    vector<Enemy> enemies;

public:
    Game()
    {
        tankX = WIDTH / 2;
        score = 0;

        for (int i = 0; i < 3; i++)
            enemies.push_back(Enemy());
    }

    void moveTankLeft()
    {
        if (tankX > 0) tankX--;
    }

    void moveTankRight()
    {
        if (tankX < WIDTH - 1) tankX++;
    }

    void shoot()
    {
        bullets.push_back(Bullet(tankX, HEIGHT - 2));
    }

    void spawnEnemy()
    {
        enemies.push_back(Enemy());
    }

    int enemyCount()
    {
        return enemies.size();
    }

    void moveBullets()
    {
        for (auto& b : bullets)
            if (b.isActive())
                b.move();

        bullets.erase(
            remove_if(bullets.begin(), bullets.end(),
                [](Bullet& b) { return !b.isActive(); }),
            bullets.end());
    }

    void moveEnemies()
    {
        for (auto& e : enemies)
            e.fall();
    }

    void checkCollisions()
    {
        for (auto& b : bullets)
        {
            if (!b.isActive()) continue;

            for (auto& e : enemies)
            {
                int nextBulletY = b.getY() - 1;

                if (b.getX() == e.getX() &&
                    (b.getY() == e.getY() || nextBulletY == e.getY()))
                {
                    b.deactivate();
                    e.respawn();
                    score += 10;
                }
            }
        }
    }

    bool gameOver()
    {
        for (auto& e : enemies)
        {
            if (e.getY() >= HEIGHT - 1)
                return true;

            if (e.getY() == HEIGHT - 1 &&
                e.getX() == tankX)
                return true;
        }

        return false;
    }

    void draw()
    {
        gotoXY(0, 0);

        cout << "0123456789\n";

        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                bool printed = false;

                for (auto& e : enemies)
                {
                    if (e.getX() == x && e.getY() == y)
                    {
                        cout << "O";
                        printed = true;
                    }
                }

                for (auto& b : bullets)
                {
                    if (b.getX() == x && b.getY() == y)
                    {
                        cout << "|";
                        printed = true;
                    }
                }

                if (!printed)
                {
                    if (y == HEIGHT - 1 && x == tankX)
                        cout << "T";
                    else
                        cout << " ";
                }
            }
            cout << "\n";
        }

        cout << "0123456789\n";
        cout << "\nScore: " << score << "\n";
        cout << "Controls: <- -> move | SPACE shoot | ESC quit\n";
    }
};

int main()
{
    srand(time(0));

    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    Game game;

    int bulletTimer = 0;
    int enemyTimer = 0;
    int difficultyTimer = 0;

    int enemyFallDelay = 700;
    int maxEnemies = 3;

    while (true)
    {
        if (_kbhit())
        {
            char key = _getch();

            if (key == 224)
                key = _getch();

            if (key == 75) game.moveTankLeft();
            if (key == 77) game.moveTankRight();
            if (key == ' ') game.shoot();
            if (key == 27) break;
        }

        bulletTimer += 50;
        enemyTimer += 50;
        difficultyTimer += 50;

        if (bulletTimer >= 60)
        {
            game.moveBullets();
            bulletTimer = 0;
        }

        if (enemyTimer >= enemyFallDelay)
        {
            game.moveEnemies();
            enemyTimer = 0;
        }

        if (difficultyTimer >= 10000)
        {
            difficultyTimer = 0;

            if (enemyFallDelay > 200)
                enemyFallDelay -= 100;

            if (game.enemyCount() < maxEnemies + 1)
            {
                game.spawnEnemy();
                maxEnemies++;
            }
        }

        game.checkCollisions();

        if (game.gameOver())
        {
            game.draw();
            cout << "\nGAME OVER\n";
            break;
        }

        game.draw();

        Sleep(50);
    }
}
