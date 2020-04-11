#include "basic_manipulations.h"

//int X[4] = {1, -1, 0, 0};
//int Y[4] = {0, 0, 1, -1};

int X[5] = {-1, 0, 1,  0, -1};
int Y[5] = { 0, 1, 0, -1,  0};

bool is_addr_correct (gamma_t *g, uint32_t x, uint32_t y) {
  return x < (g -> width) && y < (g -> height);
}

uint64_t get_position (gamma_t *g, uint32_t x, uint32_t y) {
  return (g -> height * y) + x;
}

uint64_t find (gamma_t *g, uint64_t position) {
  uint64_t* zawartosc = g -> dsu + position;
  if (*zawartosc == position) {
    return position;
  } else {
    *zawartosc = find(g, *zawartosc);
    return *zawartosc;
  }
}

bool is_adjacent (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  uint32_t i;
  for (i = 0; i < 4; i++)
    if (is_addr_correct(g, x + X[i], y + Y[i]))
      if ((g -> board)[ get_position (g, x + X[i], y + Y[i]) ] == player)
        return true;

  return false;
}

uint32_t nth_neighbours_val (gamma_t *g, uint16_t n, uint32_t x, uint32_t y) {
  return g -> board[get_position(g, x + X[n], y + Y[n])];
}

bool if_corr_is_eq (gamma_t *g, uint32_t value, uint32_t x, uint32_t y){
  return is_addr_correct(g, x, y) && g -> board[get_position(g, x, y)] == value;
}
