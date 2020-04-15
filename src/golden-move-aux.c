#include <stdlib.h>
#include "golden-move-aux.h"
#include "gamma-move-aux.h"

static uint64_t pos;
static uint32_t player;
static uint32_t owner;

static gamma_t *g;

/** @brief Zmienia parametry pojedynczego pola przy wykonaniu złotego ruchu
 * Aktualizuje liczności pól graczy oraz ustawia pole na należące do
 * odpowiedniego gracza. Zapamiętuje, że gracz wykorzystał swój złoty ruch oraz
 * aktualizuje długość łańcucha znaków, który jest potrzeby aby wypisać planszę.
 */
void mod_field_count () {
  g->player_fields[owner - 1]--;
  g->player_fields[player - 1]++;
  g->board[pos] = player;
  g->player_golden_used[player - 1] = true;
  update_length_of_gamma_board(g, owner, player);
}

/** @brief Sprawdza czy jest to srodek terytorium jakiegoś gracza.
 * Nic nie robi jeśli wszystie pola stykające się bokami lub rogami z polem
 * [x, y] należą do tego gracza do którego należy pole [x, y]. Wpp ustawia *adr na adres
 * jednego z takich pól -- stykającego się z [x, y] (być może tylko rogiem)
 */
uint64_t in_the_middle_of_territory(uint32_t x, uint32_t y) {
  uint16_t i;
  for (i = 0; i < 8; i++)
    if (has_nth_neighbour(g, i, x, y) && nth_neighbours_val(g, i, x, y) != owner)
      return get_position(g, x + X[i], y + Y[i]);

  mod_field_count();
  g->dsu[pos] = pos;
  g->player_areas[player - 1]++;
  return pos;
}


/** @brief Sprawdza, czy argumenty są semantycznie poprawne.
 * Co dokładnie sprawdza?
 */
bool gold_input_incorrect(uint32_t x, uint32_t y) {
  return g == NULL || player == 0 || player > (g->players) ||
         g->player_golden_used[player - 1] || (! is_addr_correct(g, x, y)) ||
         g->board[get_position(g, x, y)] == 0 ||
         g->board[get_position(g, x, y)] == player;
}

/** @brief Sprawdza czy plansza jest zdegenerowana.
 * Jeśli wysokość lub szerokość planszy jest równa 1, zwraca true.
 * W przeciwnym przypadku zwraca false.
 */
bool is_degenerated() {
  return g->width == 1 || g->height == 1;
}


// zakłada, że pole [x, y] nie należy do żadnego gracza
void change_adjacency(uint32_t x, uint32_t y) {
  uint16_t i;
  for (i = 0; i < 4; i++)
    if (if_corr_is_eq(g, 0, x + X[i], y + Y[i])) {
      if (! is_adjacent(g, player, x + X[i], y + Y[i]))
        g->player_adjacent[player - 1]++;
      if (! is_adjacent(g, owner, x + X[i], y + Y[i]))
        g->player_adjacent[owner - 1]--;
    }
}


/** @brief Przechodzi rekurencyjnie planszę.
 *
 */
void search(uint64_t flag, uint32_t value, uint32_t x, uint32_t y) {
  uint16_t i;
  g->dsu[get_position(g, x, y)] = flag;
  for (i = 0; i < 4; i++)
    if (if_corr_is_eq(g, value, x + X[i], y + Y[i]) &&
        g->dsu[get_position(g, x + X[i], y + Y[i])] != flag)
          search(flag, value, x + X[i], y + Y[i]);
}

/**
 * Zmienia g -> dsu i g -> board !!
 */
uint16_t crumbles_to_how_many_parts(uint32_t x, uint32_t y, uint64_t flag) {
  uint16_t i, result = 0;
  g->board[get_position(g, x, y)] = 0;
  for (i = 0; i < 4; i++)
    if (if_corr_is_eq(g, owner, x + X[i], y + Y[i]) &&
        g->dsu[nth_neighbours_pos(g, i, x, y)] != flag) {
      result++;
      search(flag, owner, x + X[i], y + Y[i]);
    }
  return result;
}

/** @brief
 *
 */
bool can_be_divided(uint32_t whose_area, uint16_t pieces) {
  return g->player_areas[whose_area - 1] + pieces - 1 <= g->max_areas;
}

/** @brief
 *
 */
void fix_after_search_when_divides(uint32_t x, uint32_t y, uint64_t flag) {
  uint16_t i;
  for (i = 0; i < 4; i++)
    if (if_corr_is_eq(g, owner, x + X[i], y + Y[i]) &&
        g->dsu[nth_neighbours_pos(g, i, x, y)] == flag)
          search(nth_neighbours_pos(g, i, x, y), owner, x + X[i], y + Y[i]);
}

/** @brief
 *
 */
void fix_when_too_many_parts(uint32_t x, uint32_t y, uint64_t flag) {
  g->board[get_position(g, x, y)] = owner;
  g->dsu[get_position(g, x, y)] = flag;
  search (get_position(g, x, y), owner, x, y);
  g->dsu[get_position(g, x, y)] = get_position(g, x, y);
}

bool crumble(uint32_t x, uint32_t y, uint64_t flag) {
  uint16_t parts = crumbles_to_how_many_parts(x, y, flag);

  if (can_be_divided(owner, parts)) {
    g->player_areas[owner - 1] += parts - 1;
    fix_after_search_when_divides(x, y, flag);
    change_adjacency(x, y);
    update_dsu_and_areas(g, player, x, y);
    mod_field_count();
    return true;
  } else {
    fix_when_too_many_parts(x, y, flag);
    return false;
  }
}


bool gamma_golden_move(gamma_t *g_temp, uint32_t player_temp, uint32_t x, uint32_t y) {
  player = player_temp;
  g = g_temp;
  if (gold_input_incorrect(x, y) || (! is_adjacent(g, player, x, y)
      && g->player_areas[player - 1] == g->max_areas)) {
        return false;
  } else {
    uint64_t flag = 0;
    pos = get_position(g, x, y);
    owner = g->board[pos];
    if (! is_degenerated())
      flag = in_the_middle_of_territory(x, y);

    return flag == pos || crumble(x, y, flag);
  }
}
