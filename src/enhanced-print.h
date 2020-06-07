/** @file
 * Udostępnia funkcje wypisującą kolorową planszę
 */

#ifndef ENHANCED_PRINT
#define ENHANCED_PRINT

#include <stdlib.h>
#include "basic_manipulations.h"

/** @brief Wypisuje planszę do gry gamma z autorsko pokolorowanymi polami
* Wypisuje planszę kolorując pola na które gracz @p player może wykonać złoty ruch,
* na inny kolor koloruje pola gracza @p player, a na jeszcze inny pola na które gracz
* @p player może się ruszyć zwykłym ruchem.
* @param[in,out] g  – poprawny wskaźnik na strukturę przechowującą stan gry,
* @param[in] player – poprawny numer gracza, którego ruch jest teraz oczekiwany.
*/
extern void print_enhanced_board(gamma_t *g, uint32_t player);

/** Zwaraca pionek gracza
* @param[in] player – poprawny numer gracza.
* @return Zwraca znak, którym oznaczane będą pola gracz @p player w trybie interaktywnym.
*/
extern char player_character(uint32_t player);

#endif
