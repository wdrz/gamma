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

/** Sprawdza czy można wykonać złoty ruch na pole
* @param[in] g       – poprawny wskaźnik na strukturę przechowującą stan gry,
* @param[in] player  – poprawny numer gracza,
* @param[in] x       – poprawny numer kolumny,
* @param[in] y       – poprawny numer wiersza,
* @return Zwraca true jeśli gracz @p player możw wykonać ruch na pole @p x, @p y, false wpp.
*/
extern bool golden_possible_to_field(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** Liczy na ile obszarów rozpadnie się obszar
* @param[in] g       – poprawny wskaźnik na strukturę przechowującą stan gry,
* @param[in] owner   – poprawny numer gracza - właściciela pola @p x, @p y.
* @param[in] x       – poprawny numer kolumny,
* @param[in] y       – poprawny numer wiersza,
* @param[in] pos     – indeks pola @p x, @p y.
* @return liczba obszarów które powstaną z obszaru do którego należy pole @p x, @p y
* po usunięciu pola @p x, @p y.
*/
extern uint16_t crumbles_to(gamma_t *g, uint32_t owner, uint32_t x, uint32_t y, uint64_t pos);


#endif
