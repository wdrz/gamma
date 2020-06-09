#include <stdlib.h>
#include "golden-move-aux.h"
#include "gamma-move-aux.h"
#include "enhanced-print.h"
#include "low-module.h"

// wykorzystujemy tablice num_of_bridges jako tablice odwiedzonych - i tak przestaje
// ona być aktualna po wykonaniu złotego ruchu.
// Marker odwiedzenia to liczba 60 - większa od wszystkich wartości, które mogą
// naturalnie wystąpić w tej tablicy
#define VISITED 60

/* Aktualizuje liczby pól sąsiadujacych z obszarami graczy po zwolnieniu
 * pola (@p x, @p y) należącego poprzednio do gracza #OWNER
 * Zakłada, że właściciel pola (@p x, @p y) jest tymczasowo ustawiony na 0.
 */
static void change_adjacent(gamma_t *g, uint32_t owner, uint32_t x, uint32_t y) {
  uint16_t i;

  for (i = 0; i < 4; i++) {

    if (!has_nth_neighbour(g, i, x, y)) continue;

    if (nth_neighbours_val(g, i, x, y) == 0) {

      if (!is_nth_neighbour_adjacent(g, i, owner, x, y)) {
        g->player_adjacent[owner - 1]--;
      }

    } else if (!scan_neighbours(g, i, nth_neighbours_val(g, i, x, y), x, y)) {
      g->player_adjacent[nth_neighbours_val(g, i, x, y) - 1]++;
    }
  }
}

/* @brief Przechodzi rekurencyjnie planszę do gry gamma
 * Przechodzi po obszarze do którego należy pole (@p x, @p y) gracza @p owner.
 * @param[in,out] g   – poprawny wskaźnik na strukturę przechowującą stan gry,
 * @param[in] flag    – liczba którą wpisujemy w tablicę dsu odwiedzanych pól,
 * @param[in] owner   – numer gracza po polach którego przechodzimy,
 * @param[in] x       – poprawny numer kolumny,
 * @param[in] y       – poprawny numer wiersza.
 */
static void change_dsu(gamma_t *g, uint64_t flag, uint32_t owner, uint32_t x, uint32_t y) {
  uint16_t i;
  uint32_t pos = get_position(g, x, y);

  g->dsu[pos] = flag;
  g->num_of_bridges[pos] = VISITED;

  for (i = 0; i < 4; i++)
    if (has_nth_neighbour_is_eq(g, owner, i, x, y) &&
        g->num_of_bridges[nth_neighbours_pos(g, i, x, y)] != VISITED)
          change_dsu(g, flag, owner, x + X_DELTA[i], y + Y_DELTA[i]);
}

static void make_field_free(gamma_t *g, uint32_t x, uint32_t y, uint32_t owner) {
  uint16_t i;
  uint32_t pos = get_position(g, x, y);
  uint64_t flag;

  g->player_areas[owner - 1] += crumbles_to(g, owner, x, y, pos) - 1;
  update_length_of_gamma_board(g, owner, 0);
  g->player_fields[owner - 1]--;
  g->board[pos] = 0;
  g->num_of_bridges[pos] = 0;
  g->empty_fields++;

  change_adjacent(g, owner, x, y);

  for (i = 0; i < 4; i++) {
    if (has_nth_neighbour_is_eq(g, owner, i, x, y) &&
      g->num_of_bridges[nth_neighbours_pos(g, i, x, y)] != VISITED) {
        flag = get_position(g, x + X_DELTA[i], y + Y_DELTA[i]);
        change_dsu(g, flag, owner, x + X_DELTA[i], y + Y_DELTA[i]);
    }
  }
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {

  if (!is_data_correct(g, player, x, y))
    return false;

  if (!g->low_updated)
    actualise_low_all(g);

  if (!golden_possible_to_field(g, player, x, y))
    return false;

  g->low_updated = false;
  uint32_t owner = g->board[get_position(g, x, y)];

  make_field_free(g, x, y, owner);
  gamma_move(g, player, x, y);

  g->player_golden_used[player - 1] = true;
  return true;
}
