#include "basic_manipulations.h"

const int X_DELTA[8] = {-1, 0, 1,  0, -1, 1,  1, -1};

const int Y_DELTA[8] = { 0, 1, 0, -1,  1, 1, -1, -1};


bool old_golden_possible(gamma_t *g, uint32_t player) {
  if (g == NULL || player > g->players || player == 0)
    return false;
  else
    return (! g->player_golden_used[player - 1]) &&
      ((g->width * g->height) - (g->empty_fields) -
      (g->player_fields[player - 1]) > 0);
}


uint64_t find(gamma_t *g, uint64_t position) {
  uint64_t* content = g -> dsu + position;
  if (*content == position) {
    return position;
  } else {
    // optymalizacja drzewa dsu
    *content = find(g, *content);
    return *content;
  }
}


bool is_adjacent(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  uint32_t i;
  for (i = 0; i < 4; i++)
    if (has_nth_neighbour(g, i, x, y))
      if (nth_neighbours_val(g, i, x, y) == player)
        return true;

  return false;
}


bool scan_neighbours(gamma_t *g, uint16_t n, uint64_t value, uint32_t x, uint32_t y) {
  uint16_t i;
  for (i = 0 ; i < n; i++)
    if (has_nth_neighbour(g, i, x, y))
      if (nth_neighbours_val(g, i, x, y) == value)
        return true;
  return false;
}


uint16_t how_many_digits(uint32_t number) {
  if (number <= 9) return 1;
  else return how_many_digits(number / 10) + 1;
}


void write_number(uint32_t number, char *where) {
  *where = '0' + (number % 10);
  if (number >= 10)
    write_number(number / 10, where - 1);
}
