#include <stdlib.h>
#include "gamma-move-aux.h"

// tej nie widac na zewnątrz
bool table_contains (uint64_t arr[], uint64_t val, uint16_t arr_len) {
  uint16_t i;
  for (i = 0; i < arr_len; i++)
    if (arr[i] == val)
      return false;
  return true;
}

void update_dsu (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  uint64_t pos = get_position(g, x, y);
  uint64_t prev_areas[4], curr_area;
  uint16_t i, prev_areas_len = 0;
  for (i = 0; i < 4; i++) {
    if (has_nth_neighbour(g, i, x, y) &&
        nth_neighbours_val(g, i, x, y) == player) {
      curr_area = find(g, nth_neighbours_pos(g, i, x, y));
      if (table_contains(prev_areas, curr_area, prev_areas_len)) {
        prev_areas[prev_areas_len] = curr_area;
        if (prev_areas_len > 0)
          g -> dsu[curr_area] = prev_areas[0];
        prev_areas_len++;
      }
    }
  }
  if (prev_areas_len > 0)
    g -> dsu[pos] = prev_areas[0];
  else
    g -> dsu[pos] = pos;

  g -> player_areas[player - 1] -= prev_areas_len - 1;
}


void update_adjacency (gamma_t *g,  uint32_t player, uint32_t x, uint32_t y) {
  uint16_t i;
  for (i = 0; i < 4; i++) {
    if (! has_nth_neighbour(g, i, x, y)) continue;
    if (nth_neighbours_val(g, i, x, y) == 0) {
      if (! is_adjacent (g, player, x + X[i], y + Y[i]))
        g -> player_adjacent[player - 1] ++;
    } else if (! scan_neighbours(g, i, nth_neighbours_val(g, i, x, y), x, y)) {
      g -> player_adjacent[nth_neighbours_val(g, i, x, y) - 1] --;
    }
  }
}


bool gm_input_incorrect (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  return g == NULL || player == 0 || player > (g -> players) ||
         (! is_addr_correct(g, x, y)) ||
         g -> board[get_position(g, x, y)] != 0;
}


bool too_many_areas (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  return g -> player_areas[player - 1] == g -> max_areas && ! is_adjacent(g, player, x, y);
}
