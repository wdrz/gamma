#include <stdlib.h>
#include "gamma-move-aux.h"

void update_dsu (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  uint64_t prev_areas[4];
  uint16_t i, j = 0, k;
  bool new_area;
  uint64_t curr_area, curr_pos;
  for (i = 0; i < 4; i++) {
    if (is_addr_correct(g, x + X[i], y + Y[i])) {
      curr_pos = get_position (g, x + X[i], y + Y[i]);
      if (g -> board[curr_pos] == player) {
        new_area = true;
        curr_area = find(g, curr_pos);
        for (k = 0; k < j; k++) {
          if (prev_areas[k] == curr_area) {
            new_area = false;
            break;
          }
        }
        if (new_area) {
          prev_areas[j] = curr_area;
          if (j > 0)
            g -> dsu[curr_area] = prev_areas[0];
          j++;
        }
      }
    }
  }
  if (j > 0)
    g -> dsu[ get_position(g, x, y) ] = prev_areas[0];
  else
    g -> dsu[ get_position(g, x, y) ] = get_position(g, x, y);

  g -> player_areas[player - 1] -= j - 1;
}


void update_adjacency (gamma_t *g,  uint32_t player, uint32_t x, uint32_t y) {
  uint16_t i, j;
  if (is_adjacent(g, player, x, y))
    g -> player_adjacent[player - 1] --;

  for (i = 0; i < 4; i++) {
    if (! is_addr_correct(g, x + X[i], y + Y[i])) continue;
    if (nth_neighbours_val(g, i, x, y) == 0) {
      if (! is_adjacent (g, player, x + X[i], y + Y[i]))
        g -> player_adjacent[player - 1] ++;
    } else if (nth_neighbours_val(g, i, x, y) != player) {
      bool b = true;
      for (j = 0 ; j < i; j++)
        if (nth_neighbours_val(g, j, x, y) == nth_neighbours_val(g, i, x, y))
          b = false;
      if (b)
        g -> player_adjacent[nth_neighbours_val(g, i, x, y) - 1] --;
    }
  }
}


bool gm_input_incorrect (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  if (g == NULL || player == 0 || player > (g -> players))
    return true;
  if (! is_addr_correct(g, x, y))
    return true;
  if (g -> board[get_position(g, x, y)] != 0)
    return true;
  return false;
}

bool too_many_areas (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  if ((g -> player_areas[player - 1]) == (g -> max_areas) && ! is_adjacent(g, player, x, y))
    return true;
  return false;
}
