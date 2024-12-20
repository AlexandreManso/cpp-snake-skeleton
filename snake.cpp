#include <iostream>
#include <vector>
#include <utility>
#include <array>
#include <thread>
#include <cstring>
#include <string>
#include "sys/ioctl.h"
#include "termios.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"

namespace internal
{
  // 💀💀💀💀💀💀💀💀 DON'T TOUCH THIS BLOCK 💀💀💀💀💀💀💀💀💀 ///
  int keyEvent();
  void frameSleep(const int &ms);
  const char *cmd_clear = "clear";
  void backgroundClear();
  void printFrame(const int &nx, const int &ny, const std::vector<int> &bg);
  void createFood(std::vector<int> &bg, std::array<int, 2> &food, const int &nx, const int &ny);
  bool eatFood(std::array<int, 2> &food, std::vector<std::pair<int, int>> &snake);
  // 💀💀💀💀💀💀💀💀 DON'T TOUCH THIS BLOCK 💀💀💀💀💀💀💀💀💀 ///
}

// 😇 Je suis gentil je vous ai fait le tracé du terrain 😇
std::vector<int> backgroundSetup(const int &nx, const int &ny)
{
  std::vector<int> bg(nx * ny, 0);
  int i;
  for (i = 0; i < nx; i++)
  {
    bg[i] = 1;
  }
  for (i = 0; i < nx; i++)
  {
    bg[(ny - 1) * nx + i] = 1;
  }
  for (i = 0; i < ny; i++)
  {
    bg[i * nx] = 1;
  }
  for (i = 0; i < ny; i++)
  {
    bg[i * nx + nx - 1] = 1;
  }

  return bg;
}

///////////////////////////////////////////////////////////////////////////////
/////// 🧑‍💻 FROM HERE you must complete 🧑‍💻 ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void add_snake(const std::vector<std::pair<int, int>> &snake, std::vector<int> &bg, int nx, int ny)
{
  // 👉️ Your code here 👈️
  for (auto& corps : snake){
    bg[corps.second *nx + corps.first] = 3;
  }
}

void remove_snake(const std::vector<std::pair<int, int>> &snake, std::vector<int> &bg, int nx, int ny)
{
  // 👉️ Your code here 👈️
    for (auto& corps : snake){
    bg[corps.second *nx + corps.first] = 0;
}
}

std::array<int, 2> snake_movement(char key)
{
  // 👉️ Your code here 👈️
  std::array<int, 2> dxdy = {0, 0};
    const char* cz = "z";
    const char* cq = "q";
    const char* cs = "s";
    const char* cd = "d";

    if (key == cz[0]) {
        dxdy = {0, 1};  // Déplacement vers le haut
    } 
    else if (key == cq[0]) {
        dxdy = {-1, 0};  // Déplacement vers la gauche
    } 
    else if (key == cs[0]) {
        dxdy = {0, -1};  // Déplacement vers le bas
    } 
    else if (key == cd[0]) {
        dxdy = {1, 0};   // Déplacement vers la droite
    }

    return dxdy;  // Retourne la valeur de dxdy
}

bool verifyBorder(const std::vector<std::pair<int, int>> &snake, int nx, int ny)
{
  std::pair <int,int> tete;
  tete = snake[0];
  if((tete.first == nx) || (tete.first == 0) || (tete.second == ny) ||(tete.second == 0)){
    return(false);
  }
  return(true);
}

std::vector<std::pair<int, int>> setupSnake(int snake_len)
{
  std::vector<std::pair<int, int>> snake(snake_len);
  int i;
  for (i = 0; i < snake_len; i++)
  {
    snake[i].first = 10 + i;
    snake[i].second = 10;
  }
  return snake;
}

void update_snake_coordinates(std::vector<std::pair<int, int>> &snake, bool eat, std::array<int, 2> dxdy)
{
  if(eat == true){
    std::pair<int,int> ajout;
    std::pair<int, int>& dernier = snake.back();
    ajout.first = dernier.first ;
    ajout.second = dernier.second -1;
    snake.push_back(ajout);
  }
  
  for(int i =0; snake.size()-1;i++){
      snake[snake.size()-i] = snake[snake.size()-i-1];
  }

  std::pair <int,int> tete;
  tete.first = snake[0].first + dxdy[0];
  tete.second = snake[0].second +dxdy[1];
  snake[0] = tete;
}

void startGame(const int &lap, const int &nx, const int &ny, std::vector<std::pair<int, int>> &snake, std::vector<int> &bg)
{
  char key;
  std::array<int, 2> dxdy = {1, 0};
  std::array<int, 2> food = {0, 0};
  internal::createFood(bg, food, nx, ny);
  while (true);
  {
    internal::frameSleep(lap);
    if (internal::keyEvent())
    {
      std::cin >> key;
      dxdy = snake_movement(key);
    }
    internal::backgroundClear();
    add_snake(snake, bg, nx, ny);
    internal::printFrame(nx, ny, bg);
    remove_snake(snake, bg, nx, ny);
    bool out = verifyBorder(snake, nx, ny);
    if (out == false)
    {
      std::cerr << "" << std::endl;
      exit(1);
    }
    bool eat = internal::eatFood(food, snake);
    if (eat)
    {
      internal::createFood(bg, food, nx, ny);
    }
    update_snake_coordinates(snake, eat, dxdy);
  }
}

int main()
{
  const int nx = 50;
  const int ny = 25;
  const int lap = 200;
  int snake_len = 3;

  std::vector<int> background = backgroundSetup(nx, ny);
  internal::printFrame(nx, ny, background);

  std::vector<std::pair<int, int>> snake = setupSnake(snake_len);

  startGame(lap, nx, ny, snake, background);
  return 0;
}

// 💀💀💀💀💀💀💀💀 DON'T TOUCH CODE AFTER THIS LINE 💀💀💀💀💀💀💀💀💀💀 ///

namespace internal
{

  static const int STDIN = 0;
  static bool initialized = false;

  // Comment ca va

  int keyEvent()
  {
    if (!initialized)
    {
      termios term;
      tcgetattr(STDIN, &term);
      term.c_lflag &= ~ICANON;
      tcsetattr(STDIN, TCSANOW, &term);
      setbuf(stdin, NULL);
      initialized = true;
    }
    int bytesWaiting;
    // int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
  }

  void frameSleep(const int &ms)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  }

  /////////////

  void backgroundClear()
  {
    int out = system(cmd_clear);
    if (out != 0)
    {
      std::cerr << "clear command failed" << std::endl;
      exit(1);
    }
  }

  void printFrame(const int &nx, const int &ny, const std::vector<int> &bg)
  {
    for (int j = 0; j < ny; j++)
    {
      for (int i = 0; i < nx; i++)
      {
        if (bg[i + j * nx] == 1)
        {
          std::cout << "#";
        }
        else if (bg[i + j * nx] == 2)
        {
          std::cout << "@";
        }
        else if (bg[i + j * nx] == 3)
        {
          std::cout << "o";
        }
        else
        {
          std::cout << " ";
        }
      }
      std::cout << std::endl;
    }
  }

  void createFood(std::vector<int> &bg, std::array<int, 2> &food, const int &nx, const int &ny)
  {
    if (food[0] == 0)
    {
      food[0] = rand() % (nx - 2) + 2;
      food[1] = rand() % (ny - 2) + 2;
      bg[food[1] * nx + food[0]] = 2;
    }
  }

  bool eatFood(std::array<int, 2> &food, std::vector<std::pair<int, int>> &snake)
  {
    if ((food[0] == std::get<0>(snake[0])) && (food[1] == std::get<1>(snake[0])))
    {
      food[0] = 0;
      food[1] = 0;
      return true;
    }
    return false;
  }

}
