/** @file
 * Przechowuje stan gry gamma
 * Udostępnia podstawowe operacje na polach planszy.
 */


#ifndef BASIC_MANIPULATIONS_H
#define BASIC_MANIPULATIONS_H

#include <stdint.h>
#include <stdbool.h>

extern int X[], Y[];

struct gamma {
  uint64_t *dsu;
  uint32_t *board;
  uint32_t *player_fields, *player_areas, *player_adjacent;
  uint32_t players, width, height, max_areas;
  uint64_t empty_fields;
  uint64_t legthOfString; // jeszcze nie obslugiwane !!!
  bool *player_golden_used;
};

typedef struct gamma gamma_t;

bool is_addr_correct (gamma_t *g, uint32_t x, uint32_t y);


/** @brief Zwraca numer pola zadanego przez wspolrzedne.
 * Zwraca liczbę za pomocą której można odwoływać się do pola (@x, @y)
 * z strukturze gamma.
 * Zakłada, że dane wejściowe są poprawne.
 * @param[in,out] g   – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new.
 */
uint64_t get_position (gamma_t *g, uint32_t x, uint32_t y);


/** @brief Zwraca reprezentatnta pola.
 * Zwraca reprezentatnta pola.
 */
uint64_t find (gamma_t *g, uint64_t position);

/** @brief Sprawdza, czy pole sąsiaduje z polem gracza.
 * Jesli pole (x, y) nie sasiaduje z polem gracza zwraca 0.
 * W przeciwnym przypadku zwraca reprezentanta jednego z nich */
bool is_adjacent (gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Zwraca numer gracza do którego należy @n-ty sąsied pola.
 * n = 0, 1, 2, 3.
 */
uint32_t nth_neighbours_val (gamma_t *g, uint16_t n, uint32_t x, uint32_t y);

bool if_corr_is_eq (gamma_t *g, uint32_t player, uint32_t x, uint32_t y);
#endif
