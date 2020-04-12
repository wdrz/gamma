#include <stdlib.h>
//#include <stdio.h>
#include "golden-move-aux.h"
#include "gamma-move-aux.h"

//static uint64_t pos;
//static uint64_t owner;

/** @brief Sprawdza czy jest to srodek terytorium jakiegoś gracza.
 * Nic nie robi jeśli wszystie pola stykające się bokami lub rogami z polem
 * [x, y] należą do tego gracza do którego należy pole [x, y]. Wpp ustawia *adr na adres
 * jednego z takich pól -- stykającego się z [x, y] (być może tylko rogiem)
 */
bool in_the_middle_of_territory (gamma_t *g, uint32_t x, uint32_t y, uint64_t *adr) {
  uint16_t i;
  uint32_t owner = g -> board[get_position(g, x, y)];
  for (i = 0; i < 8; i++)
    if (has_nth_neighbour(g, i, x, y) && nth_neighbours_val(g, i, x, y) != owner) {
      *adr = get_position(g, x + X[i], y + Y[i]);
      return false;
    }
  return true;
}


/** @brief Sprawdza, czy argumenty są semantycznie poprawne.
 * Co dokładnie sprawdza?
 */
bool gold_input_incorrect (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  return g == NULL || player == 0 || player > (g -> players) ||
         g -> player_golden_used[player - 1] || (! is_addr_correct(g, x, y)) ||
         g -> board[get_position(g, x, y)] == 0 ||
         g -> board[get_position(g, x, y)] == player;
}

/** @brief Obsługuje zdegenerowany przypadek.
 * Jeśli plansza ma przynajmniej jeden z wymiarów równy 1, funkcja obsługuje
 * golden_move i zwraca true.
 * W przeciwnym przypadku zwraca false i nie robi nic poza tym.
 */
//bool handle_degenerated (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  //uint16_t areas = 0;
  /*if (! is_addr_correct(g, x + X[0], y + Y[0]) && ! is_addr_correct(g, x + X[2], y + Y[2])) {
    if (is_addr_correct(g, x + X[1], y + Y[1])) {

    }
  } else if (! is_addr_correct(g, x + X[1], y + Y[1]) && ! is_addr_correct(g, x + X[3], y + Y[3])) {

  } else {
    return false
  }*/

  // ....

  //return false;
//}

void search (gamma_t *g, uint64_t flag, uint32_t player, uint32_t x, uint32_t y) {
  uint16_t i;
  g -> dsu[get_position(g, x, y)] = flag;
  for (i = 0; i < 4; i++)
    if (if_corr_is_eq(g, player, x + X[i], y + Y[i]) &&
        g -> dsu[get_position(g, x + X[i], y + Y[i])] != flag)
          search (g, flag, player, x + X[i], y + Y[i]);
}

bool can_be_divided (gamma_t *g, uint32_t player, uint16_t pieces) {
  return g -> player_areas[player - 1] + pieces - 1 <= g -> max_areas;
}

bool crumble (gamma_t *g, uint32_t x, uint32_t y, uint64_t visited) {
  uint16_t i, res = 0;
  uint32_t owner = g -> board[get_position(g, x, y)];

  g -> board[get_position(g, x, y)] = 0;
  for (i = 0; i < 4; i++)
    if (if_corr_is_eq(g, owner, x + X[i], y + Y[i]) &&
        g -> dsu[get_position(g, x + X[i], y + Y[i])] != visited) {
      res++;
      search (g, visited, owner, x + X[i], y + Y[i]);
    }

  if (can_be_divided(g, owner, res)) {
    g -> player_areas[owner - 1] += res - 1;
    for (i = 0; i < 4; i++)
      if (if_corr_is_eq(g, owner, x + X[i], y + Y[i])) {
        if (g -> dsu[get_position(g, x + X[i], y + Y[i])] == visited)
          search (g, get_position(g, x + X[i], y + Y[i]), owner, x + X[i], y + Y[i]);
      }
    return true;
  } else {
    g -> board[get_position(g, x, y)] = owner;
    g -> dsu[get_position(g, x, y)] = visited;
    search (g, get_position(g, x, y), owner, x, y);
    g -> dsu[get_position(g, x, y)] = get_position(g, x, y);
    return false;
  }
}

// zakłada, że pole [x, y] nie należy do żadnego gracza
void change_adjacency (gamma_t *g, uint32_t player, uint32_t owner, uint32_t x, uint32_t y) {
  uint16_t i;
  for (i = 0; i < 4; i++) {
    if (if_corr_is_eq(g, 0, x + X[i], y + Y[i])) {
      if (! is_adjacent(g, player, x + X[i], y + Y[i])) {
        g -> player_adjacent[player - 1] ++;
      }
      if (! is_adjacent(g, owner, x + X[i], y + Y[i])) {
        g -> player_adjacent[owner - 1] --;
      }
    }
  }
}

void mod_fields (gamma_t *g, uint32_t player, uint32_t owner, uint64_t pos) {
  g -> player_fields[owner - 1] --;
  g -> player_fields[player - 1] ++;
  g -> board[pos] = player;
  g -> player_golden_used[player - 1] = true;
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  uint64_t visited, pos;
  uint32_t owner;
  if (gold_input_incorrect(g, player, x, y))
    return false;
  if (! is_adjacent(g, player, x, y) && g -> player_areas[player - 1] == g -> max_areas)
    return false;
  pos = get_position(g, x, y);
  owner = g -> board[pos];
  /*if (handle_degenerated(g, player, x, y))
    return true;/ return false  */

  if (in_the_middle_of_territory(g, x, y, &visited) == true) {
    mod_fields(g, player, owner, pos);
    g -> dsu[pos] = pos;

    g -> player_areas[player - 1] ++;
    return true;
  }
  if (crumble(g, x, y, visited)) {
    change_adjacency(g, player, owner, x, y);
    update_dsu(g, player, x, y);
    mod_fields(g, player, owner, pos);
    return true;
  }

  return false;
}
