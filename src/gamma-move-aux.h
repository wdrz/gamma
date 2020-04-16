/** @file
 * Udostępnie funkcje pomocnicze dla gamma_move, dwie funkcje są pomocnicze dla golden_move
 */

#ifndef GAMMA_MOVE_AUX
#define GAMMA_MOVE_AUX

#include <stdint.h>
#include <stdbool.h>
#include "basic_manipulations.h"

/** @brief Aktualizuje długość planszy jako łańcucha znaków
 * Jeśli pewne pole przechodzi na własność gracza @p new poprzednio należąc
 * do gracza @p old (lub 0 jeśli puste), aktualizuje odpowiednio długość łańcucha
 * znaków kodującego planszę.
 * @param[in,out] g   – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] old      – stary właściciel pola (lub 0 jeśli puste),
 * @param[in] new      – nowy właściciel pola.
 */
void update_length_of_gamma_board(gamma_t *g, uint32_t old, uint32_t new);

/** @brief Aktualizuje stan struktury zbiorów rozłącznych.
 * Gdy puste pole (@p x, @p y) przechodzi na własność gracza @p player, aktualizuje
 * strukturę zbiorów rozłącznych pól planszy, tak aby pola w tym samym obszarze miały
 * tego samego reprezentanta. Aktualizuje liczbę obszarów gracza, jeśli ulegnie ona
 * zmianie w wyniku tego ruchu.
 * @param[in,out] g   – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player   – poprawny numer gracza lub 0 (puste pole),
 * @param[in] x        – poprawny numer kolumny,
 * @param[in] y        – poprawny numer wiersza.
 */
void update_dsu_and_areas (gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Aktualizuje liczby pól sąsiadujących z polami graczy.
 * Gdy pole (@p x, @p y) przechodzi na własność gracza @p player, aktualizuje
 * liczby pól sąsiadujących z polami graczy tak żeby były one prawdą po wykonaniu ruchu.
 * UWAGA: zakłada, że pole (@p x, @p y) ustawione jest jako puste
 * @param[in,out] g   – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player   – poprawny numer gracza lub 0 (puste pole),
 * @param[in] x        – poprawny numer kolumny,
 * @param[in] y        – poprawny numer wiersza.
 */
void update_adjacency (gamma_t *g,  uint32_t player, uint32_t x, uint32_t y);


 /** @brief Sprawdza czy wejscie funkcji gamma_move jest niepoprawne.
  * Sprawdza czy można wykonać gamma_move na planszy i na pole przez gracza
  * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
  * @param[in] player  – poprawny numer gracza,
  * @param[in] x       – poprawny numer kolumny,
  * @param[in] y       – poprawny numer wiersza.
  * @return true jeśli jest niepoprawny, false jeśli jest poprawny
  */
bool gm_input_incorrect (gamma_t *g, uint32_t player, uint32_t x, uint32_t y);


/** @brief Sprawdza czy wykonanie ruchu nie sprawi że gracz będzie miał za dużo obszarów
 * Sprawdza czy wykonanie ruchu na puste pole (@p x, @p y) przez gracza @p player
 * sprawi, że liczba obszarów tego gracza przekroczy areas z funkcji @ref gamma_new
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – poprawny numer gracza,
 * @param[in] x       – poprawny numer kolumny,
 * @param[in] y       – poprawny numer wiersza.
 * @return true jeśli przekroczy, false jeśli nie
 */
bool too_many_areas (gamma_t *g, uint32_t player, uint32_t x, uint32_t y);
#endif
