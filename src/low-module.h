/** @file
 * Udostępnia funkcje aktualizujące tablicę low stanu gry gamma
 */

#ifndef LOW_MODULE_H
#define LOW_MODULE_H

#include <stdlib.h>
#include "basic_manipulations.h"

/** @brief Od początku oblicza tablicę wyników funkcji low
* Od początku oblicza tablicę wyników funkcji low i wpisuje wynik do pola low stanu gry @p g
* @param[in,out] g  – poprawny wskaźnik na strukturę przechowującą stan gry,
*/
extern void actualise_low_all(gamma_t *g);

/** @brief Oblicza tablicę wyników funkcji low dla obszaru do którego należy pole (@p x, @p y)
* Aktualizuje tablicę wyników funkcji low dla obszaru do którego należy pole (@p x, @p y)
* @param[in,out] g   – poprawny wskaźnik na strukturę przechowującą stan gry,
* @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
*                      @p width z funkcji @ref gamma_new,
* @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
*                      @p height z funkcji @ref gamma_new.
*/
extern void actualise_low_after_move(gamma_t *g, uint32_t x, uint32_t y);

#endif
