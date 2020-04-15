/** @file
 * Udostępnie funkcje pomocnicze dla gamma_move.
 */

#ifndef GAMMA_MOVE_AUX
#define GAMMA_MOVE_AUX

#include <stdint.h>
#include <stdbool.h>
#include "basic_manipulations.h"

void update_length_of_gamma_board(gamma_t *g, uint32_t old, uint32_t new);

/** @brief Aktualizuje stan struktury zbiorów rozłącznych.
 * 1) Łączy różne obszary w jeden
 * 2) Dołącza pole [x, y] do tego obszaru (jeśli brak obszarów to tworzy nowy)
 */
void update_dsu_and_areas (gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Aktualizuje liczby sąsiednich pól dla wszystkich graczy.
 * zakłada, że pole [x, y] jest puste.
 */
void update_adjacency (gamma_t *g,  uint32_t player, uint32_t x, uint32_t y);


 /** @brief Sprawdza czy wejscie funkcji gamma_move jest niepoprawne.
  * Sprawdza czy można wykonać gamma_move na planszy i na pole przez gracza
  * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
  *                      @p players z funkcji @ref gamma_new,
  * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
  *                      @p width z funkcji @ref gamma_new,
  * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
  *                      @p height z funkcji @ref gamma_new.
  * @return true jeśli da się, false jeśli nie
  */
bool gm_input_incorrect (gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

// NIE
/** @brief Sprawdza czy wejscie funkcji gamma_move jest niepoprawne.
 * Sprawdza czy można wykonać gamma_move na planszy i na pole przez gracza
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new.
 * @return true jeśli da się, false jeśli nie
 */
bool too_many_areas (gamma_t *g, uint32_t player, uint32_t x, uint32_t y);
#endif
