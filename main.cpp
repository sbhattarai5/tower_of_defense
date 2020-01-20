#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <sstream>


#include "Includes.h"
#include "Constants.h"
#include "compgeom.h"
#include "Surface.h"
#include "Event.h"


const int FULL_HEALTH = 100;
const int DEFENSE_RADIUS = 100;

// TODO
//==============================
// Collision
// Score
// Random path generator
// Quadtree for collision
//==============================

class DVector
{
public:
    DVector(double x, double y)
        : _x(x), _y(y)
    {}
    double _x, _y;
    void add(const DVector & vect)
    {
        _x += vect._x;
        _y += vect._y;
        return;
    }
};

class Vector
{
public:
    Vector(int x=0, int y=0)
    : _x(x), _y(y)
    {}
    int _x;
    int _y;
    void add(const Vector & vect)
    {
        _x += vect._x;
        _y += vect._y;
        return;
    }
    
    Vector minus(const Vector & vect)
    {
        return Vector(_x - vect._x, _y - vect._y);
    }

    bool operator==(const Vector & a)
    {
        return (_x == a._x && _y == a._y);
    }
};

class Path
{
public:
    std::vector< Vector > _turning_points;
    static Surface * _surface;
    void add_point(int x, int y)
    {
        _turning_points.push_back(Vector(x, y));
        return;
    }
    void draw()
    {
        int size = _turning_points.size();
        if (_turning_points[size - 1]._x == W || _turning_points[size - 1]._x == 0)
        {
            int x = 0;
            if (_turning_points[size - 1]._x == W)
            {
                x = W - 10;
            }
            _surface->put_rect(x, _turning_points[size - 1]._y - 30, 10, 60, 150, 150, 0);
           
        }
        else
        {
            int y = 0;
            if (_turning_points[size - 1]._y == H)
            {
                y = H - 10;
            }
            _surface->put_rect(_turning_points[size - 1]._x - 30, y, 60, 10, 150, 150, 0);
        }
        for (int i = 0; i < size - 1; ++i)
        {
            Vector a = _turning_points[i];
            Vector b = _turning_points[i + 1];
            _surface->put_line(a._x, a._y, b._x, b._y, 50, 200, 200);
		
        }
        return;
    }
};

class Enemy
{
public:
    Enemy(int x, int y, std::vector< Path > & paths)
        : _position(Vector(x, y)), _motion(Vector(1, 0)), _paths(paths), _health(FULL_HEALTH), _current_t_point(0), _isalive(true), _playerscoresubtracted(false)
    {
        set_path();
    }
    Enemy(const Vector & v, std::vector< Path > & paths)
        : _position(v), _motion(Vector(1, 0)), _paths(paths), _health(FULL_HEALTH), _current_t_point(0), _isalive(true), _playerscoresubtracted(false)
    {
        set_path();
    }
    int _path_index;
    int _current_t_point;
    std::vector< Path > & _paths;
    Vector _position;
    Vector _motion;
    int _health;
    bool _isalive;
    bool _playerscoresubtracted;
    static Surface * _surface;
    void draw()
    {
        _surface->put_circle(_position._x, _position._y, 20, 255, 0, 0);
        if (_isalive) move();
        return;
    }
    void set_path()
    {
        _path_index = rand() % _paths.size();
        return;
    }
    void move()
    {
        _position.add(_motion);
        if (_position == _paths[_path_index]._turning_points[_current_t_point + 1])
        {
            _current_t_point++;
            if (_current_t_point == _paths[_path_index]._turning_points.size() - 1)
            {
                _isalive = false;
            }
            else
            {
                Vector temp = _paths[_path_index]._turning_points[_current_t_point + 1].minus(_paths[_path_index]._turning_points[_current_t_point]);
                _motion = Vector(0, 0);
                if (temp._x != 0) _motion._x = 1; 
                if (temp._y != 0) _motion._y = 1;
            }
        }
        return;
    }
};

class Bullet
{
public:
    Bullet(double x, double y, double dx=1, double dy=1)
        : _position(DVector(x, y)), _motion(DVector(dx, dy))
    {}
    DVector _position;
    DVector _motion;
    static Surface * _surface;
    void draw()
    {
        _surface->put_circle(_position._x, _position._y, 5, 255, 255, 255);
        if (_position._x >= W || _position._x < 0) _motion._x *= -1;
        if (_position._y >= H || _position._y < 0) _motion._y *= -1;
        _position.add(_motion);
        return;
    }

};

class Tower
{
public:
    Tower(int x, int y)
        : _position(Vector(x, y)), _health(FULL_HEALTH), _defense_radius(DEFENSE_RADIUS), _last_attack_time(-2000)
    {}
    Vector _position;
    int _health;
    int _last_attack_time;
    int _defense_radius;
    static Surface * _surface;
    void draw()
    {
        //Image image("images/Tower.JPG");	// loads image
        //Rect rect = image.getRect();
        //rect.x = _position._x;
        //rect.y = _position._y;
        //_surface->put_image(image, rect); // blit image at rect on surface 
        //_surface->put_unfilled_circle(_position._x + rect.w/2, _position._y + rect.h/2, DEFENSE_RADIUS, 255, 255, 255);	

        _surface->put_rect(_position._x, _position._y, 20, 40, 0, 0, 255);
        _surface->put_unfilled_circle(_position._x + 10, _position._y + 20, DEFENSE_RADIUS, {255, 255, 255});	
        return;
    }
    void attack(std::vector< Enemy > & enemies, std::vector< Bullet > & bullets)
    {
        int current_time = getTicks();
        if (current_time - _last_attack_time >= 2500)
        {
            for (int i = 0; i < enemies.size(); ++i)
            {
                if (enemies[i]._isalive)
                {
                    double distance = sqrt(pow(enemies[i]._position._y - _position._y, 2) + pow(enemies[i]._position._x - _position._x, 2));
                    if (distance <= DEFENSE_RADIUS)
                    {
                        Vector temp1 = enemies[i]._position;
                        temp1.add(enemies[i]._motion);
                        temp1.add(enemies[i]._motion);
                        Vector temp = temp1.minus(_position);
                        int max = temp._x;
                        if (temp._y > max) max = temp._y;
                        if (max < 0) max *= -1;
                        std::cout << temp._x << ' ' << temp._y << std::endl;
                        bullets.push_back(Bullet(_position._x + 10, _position._y + 20, temp._x * 2.0 / max, temp._y * 2.0 / max));
                        _last_attack_time = current_time;
                        break;
                    }
                }
            }
        }   
    }
};


class Player
{
public:
    Player()
        : _nooftowersleft(5), _score(0), _health(FULL_HEALTH)
    {};
    int _nooftowersleft;
    int _score;
    int _health;
    static Surface * _surface;
    void draw()
    {
        Font font("fonts/FreeMonoBold.ttf", 16);
        char s[100] = "Score: \0";
        char c[100];
        std::stringstream out;
        out << _score;
        strcpy(c, out.str().c_str());
        strcat(s, c);
        Image image1(font.render(s, WHITE));
        Rect rect1 = image1.getRect();
        rect1.x = W - rect1.w;
        char t[100] = "Number of towers available: \0";
        std::stringstream out1;
        out1 << _nooftowersleft;
        strcpy(c, out1.str().c_str());
        strcat(t, c);
        Image image2(font.render(t, WHITE));
        Rect rect2 = image2.getRect();
        rect2.y = rect1.h;
        rect2.x = W - rect2.w;
        char q[100] = "Health left: \0";
        std::stringstream out2;
        out2 << _health;
        strcpy(c, out2.str().c_str());
        strcat(q, c);
        Image image3(font.render(q, WHITE));
        Rect rect3 = image3.getRect();
        rect3.y = rect2.y + rect2.h;
        rect3.x = W - rect3.w;
        _surface->put_image(image1, rect1);
        _surface->put_image(image2, rect2);
        _surface->put_image(image3, rect3);
        return;
    }
};

Surface * Enemy::_surface = NULL;
Surface * Tower::_surface = NULL;
Surface * Bullet::_surface = NULL;
Surface * Path::_surface = NULL;
Surface * Player::_surface = NULL;


void draw_main_door(Surface & surface)
{
    surface.put_rect(0, H/2 - 30, 10, 60, 150, 150, 0);
    return;
}

void draw_time_left_for_placing_tower(int last_time, Surface & surface)
{
    int current_time = getTicks();
    double proportion = (current_time-last_time) / 10000.0;
    if (proportion > 1)
    {
        Font font("fonts/FreeMonoBold.ttf", 16);
        Image image1(font.render("CLICK TO PLACE THE TOWER!", WHITE));
        Rect rect1 = image1.getRect();
        rect1.x = W/2 - rect1.w / 2;
        rect1.y = 10;
        surface.put_image(image1, rect1);
    }
    else
    {
        surface.put_rect(W/4, 10, W/2 - proportion * (W/2), 20, 0, 255, 0);
        Font font("fonts/FreeMonoBold.ttf", 16);
        Image image1(font.render("Timer: ", WHITE));
        Rect rect1 = image1.getRect();
        rect1.x = W/4 - (rect1.w + 10);
        rect1.y = 10;
        surface.put_image(image1, rect1);
    }
    return;
}

int main()
{
    //initialization
    //=============================
    srand(time(NULL));
    Surface surface(W, H);
    SDL_WM_SetCaption("Tower Of Defense", NULL);
    Event event;
    Vector main_door(0, H/2);
    Enemy::_surface = &surface;
    Tower::_surface = &surface;
    Bullet::_surface = &surface;
    Path::_surface = &surface;
    Player::_surface = &surface;
    //=============================
    
    //test object
    std::vector< Tower > towers;
    std::vector< Path > paths;
    std::vector< Bullet > bullets;
    Player player;
    //Paths
    Path path1;
    path1.add_point(0, H/2);
    path1.add_point(W, H/2);
    paths.push_back(path1);
    Path path4;
    path4.add_point(0, H/2);
    path4.add_point(W/2, H/2);
    path4.add_point(W/2, H);
    paths.push_back(path4);

    int last_tower_placed_time = -10000;
    int last_enemy_placed_time = -2500;
    // Enemy
    std::vector< Enemy > enemies;
    while (1 && player._health > 0)
    {
        int current_time = getTicks();
        if (event.poll() && event.type() == QUIT) break;
        if (event.type() == MOUSEBUTTONDOWN && player._nooftowersleft != 0)
        {
            if (current_time - last_tower_placed_time > 10000)
            {
                towers.push_back(Tower(mouse_x(), mouse_y()));
                player._nooftowersleft--;
                last_tower_placed_time = current_time;
            }
        }
        if (current_time - last_enemy_placed_time > 2500)
        {
            enemies.push_back(Enemy(main_door, paths));
            last_enemy_placed_time = current_time;
        }
        surface.lock();
        surface.fill(BLACK);
        for (int i = 0; i < towers.size(); ++i){ towers[i].draw(); towers[i].attack(enemies, bullets); }
        for (int i = 0; i < paths.size(); ++i) { paths[i].draw(); }
        for (int i = 0; i < enemies.size(); ++i){
            if (enemies[i]._isalive) enemies[i].draw();
            else
            {
                if (!enemies[i]._playerscoresubtracted)
                {
                    player._health -= 10;
                    enemies[i]._playerscoresubtracted = true;
                }
            }
        }
        for (int i = 0; i < bullets.size(); ++i){ bullets[i].draw(); }
        player.draw();
        draw_main_door(surface);
        draw_time_left_for_placing_tower(last_tower_placed_time, surface);
        surface.unlock();
        surface.flip();
        delay(10);
    }
    return 0;
}
