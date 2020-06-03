/** @file
 * Udostępnia funkcje wypisującą kolorową planszę
 */

#ifndef ENHANCED_PRINT
#define ENHANCED_PRINT

#include <stdlib.h>
#include "basic_manipulations.h"

/** @brief Wypisuje planszę do gry gamma z autorsko pokolorowanymi polami
* Od początku oblicza tablicę wyników funkcji low i wpisuje wynik do pola low stanu gry @p g
* @param[in,out] g  – poprawny wskaźnik na strukturę przechowującą stan gry,
* @param[in] x      – poprawny numer gracza, którego ruch jest teraz oczekiwany.
*/
extern void print_enhanced_board(gamma_t *g, uint32_t player);

//extern void print_enhanced_field(gamma_t *g, uint32_t x, uint32_t y, uint32_t player);

#endif