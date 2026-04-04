#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <ctime>
#include <algorithm>

using namespace std;

const int WIDTH = 10;
const int HEIGHT = 20;

class Point
{
private:
    int x;
    int y;

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
private:
    Point pos;
    bool active;

public:
    Bullet(int x, int y) : pos(x, y), active(true) {}

    bool isActive() { return active; }

    int getX() { return pos.getX(); }
    int getY() { return pos.getY(); }

    void deactivate() { active = false; }

    int nextY()
    {
        return pos.getY() - 1;
    }

    void move()
    {
        pos.move(0, -1);
        if (pos.getY() < 0)
            active = false;
    }
};

class Enemy
{
private:
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
private:
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

    void update()
    {
        for (auto& b : bullets)
        {
            if (!b.isActive()) continue;

            int nextY = b.getY() - 1;

            for (auto& e : enemies)
            {
                if (b.getX() == e.getX())
                {
                    if (b.getY() == e.getY())
                    {
                        b.deactivate();
                        e.respawn();
                        score += 10;
                    }

                    else if (nextY == e.getY())
                    {
                        b.deactivate();
                        e.respawn();
                        score += 10;
                    }
                }
            }
        }

        for (auto& b : bullets)
            if (b.isActive())
                b.move();

        for (auto& e : enemies)
            e.fall();

        bullets.erase(
            remove_if(bullets.begin(), bullets.end(),
                [](Bullet& b) { return !b.isActive(); }),
            bullets.end());
    }

    bool gameOver()
    {
        for (auto& e : enemies)
        {
            if (e.getY() >= HEIGHT - 1)
                return true;

            if (e.getY() == HEIGHT - 1 && e.getX() == tankX)
                return true;
        }

        return false;
    }

    void draw()
    {
        system("cls");

        cout << "0123456789" << endl;

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
                    if (b.isActive() &&
                        b.getX() == x &&
                        b.getY() == y)
                    {
                        cout << "|";
                        printed = true;
                    }
                }

                if (!printed)
                {
                    if (y == HEIGHT - 1 && x == tankX)
                        cout << "Δ";
                    else
                        cout << " ";
                }
            }
            cout << endl;
        }
        cout << "0123456789" << endl;
        cout << "\nScore: " << score << endl;
        cout << "Controls: <- -> move | SPACE shoot | ESC quit\n";
    }
};

int main()
{
    srand(time(0));

    Game game;
    game.draw();

    while (true)
    {
        char key = _getch();

        if (key == 27) break;

        if (key == 75) game.moveTankLeft();
        if (key == 77) game.moveTankRight();
        if (key == ' ') game.shoot();

        game.update();

        if (game.gameOver())
        {
            game.draw();
            cout << "\nGAME OVER\n";
            break;
        }

        game.draw();
    }
}
