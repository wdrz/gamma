/** @file
 * Implementacja prostych funkcji
 */

#include "basic_manipulations.h"

/// Tabela różnic pierwszych wpółrzędnych pomiędzy dowolnym polem, a jego i-tym sąsiadem
static int X[8] = {-1, 0, 1,  0, -1, 1,  1, -1};

/// Tabela różnic drugich wpółrzędnych pomiędzy dowolnym polem, a jego i-tym sąsiadem
static int Y[8] = { 0, 1, 0, -1,  1, 1, -1, -1};

bool is_addr_correct(gamma_t *g, uint32_t x, uint32_t y) {
  return x < (g->width) && y < (g->height);
}

uint64_t get_position(gamma_t *g, uint32_t x, uint32_t y) {
  return (g->width * y) + x;
}

uint64_t find (gamma_t *g, uint64_t position) {
  uint64_t* zawartosc = g -> dsu + position;
  if (*zawartosc == position) {
    return position;
  } else {
    // optymalizacja drzewa dsu
    *zawartosc = find(g, *zawartosc);
    return *zawartosc;
  }
}

uint32_t nth_neighbours_val (gamma_t *g, uint16_t n, uint32_t x, uint32_t y) {
  return g -> board[get_position(g, x + X[n], y + Y[n])];
}

uint32_t nth_neighbours_pos (gamma_t *g, uint16_t n, uint32_t x, uint32_t y) {
  return get_position(g, x + X[n], y + Y[n]);
}

bool has_nth_neighbour(gamma_t *g, uint16_t n, uint32_t x, uint32_t y) {
  return is_addr_correct(g, x + X[n], y + Y[n]);
}

bool is_adjacent (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  uint32_t i;
  for (i = 0; i < 4; i++)
    if (has_nth_neighbour(g, i, x, y))
      if (nth_neighbours_val(g, i, x, y) == player)
        return true;

  return false;
}

bool has_nth_neighbour_is_eq(gamma_t *g, uint64_t value, uint16_t n, uint32_t x, uint32_t y) {
  return has_nth_neighbour(g, n, x, y) && nth_neighbours_val(g, n, x, y) == value;
}

bool scan_neighbours(gamma_t *g, uint16_t n, uint64_t value, uint32_t x, uint32_t y) {
  uint16_t i;
  for (i = 0 ; i < n; i++)
    if (has_nth_neighbour(g, i, x, y))
      if (nth_neighbours_val(g, i, x, y) == value)
        return true;
  return false;
}

bool is_nth_neighbour_adjacent(gamma_t *g, uint16_t n, uint32_t value, uint32_t x, uint32_t y) {
  return is_adjacent (g, value, x + X[n], y + Y[n]);
}

uint16_t how_many_digits (uint32_t number) {
  if (number <= 9) return 1;
  else return how_many_digits(number / 10) + 1;
}

void write_number(uint32_t number, char *where) {
  *where = '0' + (number % 10);
  if (number >= 10)
    write_number(number / 10, where - 1);
}
