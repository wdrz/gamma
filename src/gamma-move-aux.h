/** @file
 * Udostępnie funkcje pomocnicze dla gamma_move, dwie funkcje są pomocnicze dla golden_move
 */

#ifndef GAMMA_MOVE_AUX
#define GAMMA_MOVE_AUX

#include <stdint.h>
#include <stdbool.h>
#include "basic_manipulations.h"

/** @brief Aktualizuje długość planszy jako łańcucha znaków.
 * Jeśli pewne pole przechodzi na własność gracza @p new poprzednio należąc
 * do gracza @p old (lub 0 jeśli puste), aktualizuje odpowiednio długość łańcucha
 * znaków kodującego planszę.
 * @param[in,out] g   – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] old      – stary właściciel pola (lub 0 jeśli puste),
 * @param[in] new      – nowy właściciel pola.
 */
extern void update_length_of_gamma_board(gamma_t *g, uint32_t old, uint32_t new);


/** @brief Wykonuje ruch.
 * Ustawia pionek gracza @p player na polu (@p x, @p y).
 * @param[in,out] g   – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia niewiększa od wartości
 *                      @p players z funkcji @ref gamma_new,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new.
 * @return Wartość @p true, jeśli ruch został wykonany, a @p false,
 * gdy ruch jest nielegalny lub któryś z parametrów jest niepoprawny.
 */
extern bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);
#endif
