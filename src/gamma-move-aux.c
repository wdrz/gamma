#include <stdlib.h>
#include "gamma-move-aux.h"

/* @brief Sprawdza czy tablica zawiera liczbę
 * Sprawdza, czy w tablicy @p arr o długości @p arr_len występuje wartość @p val.
 * @param[in] arr     – tablica z danymi,
 * @param[in] val     – wartość której szukamy w tablicy,
 * @param[in] arr_len – długość tablicy @p arr.
 * @return true jeśli występuje, false jeśli nie
 */
static bool table_contains(uint64_t arr[], uint64_t val, uint16_t arr_len) {
  uint16_t i;
  for (i = 0; i < arr_len; i++)
    if (arr[i] == val)
      return false;

  return true;
}

void update_length_of_gamma_board(gamma_t *g, uint32_t old, uint32_t new) {
  uint16_t old_len = how_many_digits(old), new_len = how_many_digits(new);
  g->length_of_string += new_len - old_len;

  if (old_len > 1 && new_len == 1) {
    g->length_of_string -= 2;

  } else if (old_len == 1 && new_len > 1) {
    g->length_of_string += 2;
  }
}

/* @brief Aktualizuje stan struktury zbiorów rozłącznych.
 * Gdy puste pole (@p x, @p y) przechodzi na własność gracza @p player, aktualizuje
 * strukturę zbiorów rozłącznych pól planszy, tak aby pola w tym samym obszarze miały
 * tego samego reprezentanta. Aktualizuje liczbę obszarów gracza, jeśli ulegnie ona
 * zmianie w wyniku tego ruchu.
 * @param[in,out] g   – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player   – poprawny numer gracza lub 0 (puste pole),
 * @param[in] x        – poprawny numer kolumny,
 * @param[in] y        – poprawny numer wiersza.
 */
static void update_dsu_and_areas(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  uint64_t prev_areas[4], curr_area;
  uint16_t i, prev_areas_len = 0;

  for (i = 0; i < 4; i++)
    if (has_nth_neighbour(g, i, x, y) &&
        nth_neighbours_val(g, i, x, y) == player) {

          curr_area = find(g, nth_neighbours_pos(g, i, x, y));
          if (table_contains(prev_areas, curr_area, prev_areas_len)) {
            prev_areas[prev_areas_len] = curr_area;

            if (prev_areas_len > 0)
              g->dsu[curr_area] = prev_areas[0];
            prev_areas_len++;
          }
    }

  if (prev_areas_len > 0)
    g->dsu[get_position(g, x, y)] = prev_areas[0];
  else
    g->dsu[get_position(g, x, y)] = get_position(g, x, y);

  g->player_areas[player - 1] -= prev_areas_len - 1;
}


/* @brief Aktualizuje liczby pól sąsiadujących z polami graczy.
 * Gdy pole (@p x, @p y) przechodzi na własność gracza @p player, aktualizuje
 * liczby pól sąsiadujących z polami graczy tak żeby były one prawdą po wykonaniu ruchu.
 * UWAGA: zakłada, że pole (@p x, @p y) ustawione jest jako puste
 * @param[in,out] g   – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player   – poprawny numer gracza lub 0 (puste pole),
 * @param[in] x        – poprawny numer kolumny,
 * @param[in] y        – poprawny numer wiersza.
 */
static void update_adjacency(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  uint16_t i;
  for (i = 0; i < 4; i++) {

    if (!has_nth_neighbour(g, i, x, y)) continue;

    if (nth_neighbours_val(g, i, x, y) == 0) {
      if (!is_nth_neighbour_adjacent(g, i, player, x, y))
        g->player_adjacent[player - 1]++;

    } else if (!scan_neighbours(g, i, nth_neighbours_val(g, i, x, y), x, y)) {
      g->player_adjacent[nth_neighbours_val(g, i, x, y) - 1]--;
    }
  }
}

/* @brief Sprawdza czy wejscie funkcji gamma_move jest niepoprawne.
 * Sprawdza czy można wykonać gamma_move na planszy i na pole przez gracza
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – poprawny numer gracza,
 * @param[in] x       – poprawny numer kolumny,
 * @param[in] y       – poprawny numer wiersza.
 * @return true jeśli jest niepoprawny, false jeśli jest poprawny
 */
static inline bool gm_input_incorrect(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  return g == NULL || player == 0 || player > (g->players) ||
         (! is_addr_correct(g, x, y)) ||
         g->board[get_position(g, x, y)] != 0;
}

/* @brief Sprawdza czy wykonanie ruchu nie sprawi że gracz będzie miał za dużo obszarów
 * Sprawdza czy wykonanie ruchu na puste pole (@p x, @p y) przez gracza @p player
 * sprawi, że liczba obszarów tego gracza przekroczy areas z funkcji @ref gamma_new
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – poprawny numer gracza,
 * @param[in] x       – poprawny numer kolumny,
 * @param[in] y       – poprawny numer wiersza.
 * @return true jeśli przekroczy, false jeśli nie
 */
static inline bool too_many_areas(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  return g->player_areas[player - 1] == g->max_areas && !is_adjacent(g, player, x, y);
}

bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  if (gm_input_incorrect(g, player, x, y))
    return false;
  if (too_many_areas(g, player, x, y))
    return false;

  update_adjacency(g, player, x, y);
  update_dsu_and_areas(g, player, x, y);

  g->board[get_position(g, x, y)] = player;
  g->player_fields[player - 1]++;
  g->empty_fields--;
  update_length_of_gamma_board(g, 0, player);

  g->low_updated = false;

  return true;
}
