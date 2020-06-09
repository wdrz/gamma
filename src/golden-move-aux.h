/** @file
 * Udostępnia funkcję gamma_golden_move.
 */

#ifndef GOLDEN_MOVE_AUX
#define GOLDEN_MOVE_AUX

#include <stdint.h>
#include <stdbool.h>
#include "basic_manipulations.h"

/** @brief Wykonuje złoty ruch.
 * Ustawia pionek gracza @p player na polu (@p x, @p y) zajętym przez innego
 * gracza, usuwając pionek innego gracza.
 * @param[in,out] g   – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new.
 * @return Wartość @p true, jeśli ruch został wykonany, a @p false,
 * gdy gracz wykorzystał już swój złoty ruch, ruch jest nielegalny
 * lub któryś z parametrów jest niepoprawny.
 */
extern bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

#endif
