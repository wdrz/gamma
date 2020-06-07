#include "low-module.h"
#include "basic_manipulations.h"
#include <stdlib.h>

// zmienna wykorzystywana przez low_rec do numerowania wierzchołków grafu
uint64_t PREORDER;

static inline uint64_t min(uint64_t a, uint64_t b) {
  return a < b ? a : b;
}

static void clear_area_rec(gamma_t *g, uint32_t x, uint32_t y, uint32_t player) {
  uint16_t i;
  g->preorder[get_position(g, x, y)] = 0;
  g->num_of_bridges[get_position(g, x, y)] = 0;

  for (i = 0; i < 4; i++)
    if (has_nth_neighbour_is_eq(g, player, i, x, y) &&
      g->preorder[nth_neighbours_pos(g, i, x, y)] != 0)
        clear_area_rec(g, x + X_DELTA[i], y + Y_DELTA[i], player);
}

// zakłada, że preorder wszystkich pól należących do obszaru do którego należy x, y są ustawione na 0
// Mały trik: (x, y) jest korzeniem <=> pos = previous_pos
static uint64_t low_rec(gamma_t *g, uint32_t x, uint32_t y, uint32_t player, uint64_t previous_pos) {
  uint16_t i, num_of_children = 0;
  uint64_t pos = get_position(g, x, y), low = PREORDER, neighbour_pos, low_next;
  bool articulation_point_found = false;

  g->preorder[pos] = PREORDER++;

  for (i = 0; i < 4; i++) {
    if (has_nth_neighbour_is_eq(g, player, i, x, y)) {
      neighbour_pos = nth_neighbours_pos(g, i, x, y);
      if (neighbour_pos == previous_pos) continue; // sąsiad jest ojcem w drzewie dfs

      if (g->preorder[neighbour_pos] == 0) { // sąsiad nieodwiedzony
        num_of_children++;
        low_next = low_rec(g, x + X_DELTA[i], y + Y_DELTA[i], player, pos);
        low = min(low, low_next);
        if (low_next >= g->preorder[pos]) // znaleziono punkt artykulacji o ile to nie korzeń
          articulation_point_found = true;

      } else { // sąsiad odwiedzony
        low = min(low, g->preorder[neighbour_pos]);
      }
    }
  }

  if (previous_pos != pos && low == g->preorder[pos]) { // znaleziono most
    g->num_of_bridges[pos]++;
    g->num_of_bridges[previous_pos]++;
  }

  if ((previous_pos == pos && num_of_children > 1) ||
    (previous_pos != pos && articulation_point_found)) { // znaleziono punkt artykulacji
      g->num_of_bridges[pos] += 10;
  }

  return low;
}

// zakłada, że preorder wszystkich pól należących do obszaru do którego należy pole position są
// ustawione na 0. Zakłada, że pole position należy do jakiegoś gracza
static void evaluate_low(gamma_t *g, uint64_t position) {
  uint32_t pos_start = find(g, position);
  uint32_t x_start, y_start;

  PREORDER = 1;
  get_xy_by_position(g, pos_start, &x_start, &y_start);
  low_rec(g, x_start, y_start, g->board[pos_start], pos_start);
}

static void clear_all(gamma_t *g, uint64_t size) {
  uint64_t i;

  for (i = 0; i < size; i++) {
    g->preorder[i] = 0;
    g->num_of_bridges[i] = 0;
  }
}

// zakłada, że preorder wszystkich pól należących do obszaru do którego należy x, y są ustawione na 0
static void evaluate_low_all(gamma_t *g, uint64_t size) {
  uint64_t i;

  for (i = 0; i < size; i++)
    if (g->preorder[i] == 0 && g->board[i] != 0)
      evaluate_low(g, i);
}

void actualise_low_all(gamma_t *g) {
  uint64_t size = (uint64_t) g->width * (uint64_t) g->height;

  clear_all(g, size);
  evaluate_low_all(g, size);
}


void actualise_low_after_move(gamma_t *g, uint32_t x, uint32_t y) {
  uint64_t pos = get_position(g, x, y);

  clear_area_rec(g, x, y, g->board[pos]);
  evaluate_low(g, pos);
}
