/** @file
 * Przechowuje stan gry gamma
 * Udostępnia podstawowe operacje na polach planszy.
 */


#ifndef BASIC_MANIPULATIONS_H
#define BASIC_MANIPULATIONS_H

#include <stdint.h>
#include <stdbool.h>

/** @brief Struktura przechowująca stan gry gamma
 * @dsu               - dla każdego pola trzyma jego następnik w
 *                      implementacji struktury zbiorów rozłącznych,
 * @board             - dla każdego pola trzyma numer gracza do
 *                      którego to pole należy lub 0 jeśli jest to puste pole,
 * @player_fields     - dla każdego gracza trzyma liczbę zajętych przez niego pól,
 * @player_areas      - dla każdego gracza trzyma liczbę jego obszarów na planszy,
 * @player_adjacent   - dla każdego gracza trzyma liczbę pól niczyich sąsiadujących
 *                    - z polami tego gracza,
 * @players           - liczba graczy,
 * @width             - szerokość planszy,
 * @height            - wysokość planszy,
 * @max_areas         - maksymalna liczba obszarów,
 * @empty_fields      - liczba pustych pól na planszy,
 * @legthOfString     - długość łańcucha znaków, w którym można wypisać stan planszy,
 * @player_golden_used- dla każdego gracza trzyma infromację czy wykorzystał już
 *                      swój złoty ruch.
 */
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


/** Sprawdza czy @x i @y to poprawne współrzędne pola na planszy @g */
bool is_addr_correct(gamma_t *g, uint32_t x, uint32_t y);


/** @brief Zwraca pozycję w tablicy pola zadanego przez wspolrzedne.
 * Zwraca liczbę za pomocą której można odwoływać się do pola (@x, @y)
 * z tablicach trzymających informacje o poszczególnych polach w strukturze gamma.
 * Zakłada, że dane wejściowe są poprawne.
 * @param[in,out] g   – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new.
 */
uint64_t get_position(gamma_t *g, uint32_t x, uint32_t y);


/** Zwraca reprezentatnta pola w strukturze dsu pól planszy
 */
uint64_t find(gamma_t *g, uint64_t position);



/** Zwraca numer gracza do którego należy @n-ty sąsied pola (@x, @y)
 * Zwraca 0 jeśli sąsiad jest pustym polem. Zakłada, że dane są poprawne
 */
uint32_t nth_neighbours_val(gamma_t *g, uint16_t n, uint32_t x, uint32_t y);

/** Zwraca pozycję w tablicy @n-tego sąsiada pola (@x, @y). Zakłada, że dane są poprawne
 */
uint32_t nth_neighbours_pos(gamma_t *g, uint16_t n, uint32_t x, uint32_t y);

/** Sprawdza, czy pole (@x, @y) ma @n-tego sąsiada. Zakłada, że dane są poprawne
 */
bool has_nth_neighbour(gamma_t *g, uint16_t n, uint32_t x, uint32_t y);

/** @brief Sprawdza, czy pole sąsiaduje z polem gracza.
 * @return true jesli pole (@x, @y) sasiaduje z polem gracza @player, false
 * w przeciwnym przypadku.
 */
bool is_adjacent(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Sprawdza, czy pole ma sąsiada i czy należy on do gracza @n
 * @return true jesli @n-ty sąsiad pola (@x, @y) istnieje i należy do
 * gracza @value, false w przeciwnym przypadku.
 */
bool has_nth_neighbour_is_eq(gamma_t *g, uint64_t value, uint16_t n, uint32_t x, uint32_t y);

/**
 * Sprawdza, czy przynajmniej jeden z sąsiadów pola (@x, @y) o numerach
 * 0, ..., @n - 1 należy do gracza @value
 */
bool scan_neighbours (gamma_t *g, uint16_t n, uint64_t value, uint32_t x, uint32_t y);

/**
 * Sprawdza, czy @n-ty sąsiad pola (@x, @y) sąsiaduje z polem należącym do gracza @value
 */
bool is_nth_neighbour_adjacent(gamma_t *g, uint16_t n, uint32_t value, uint32_t x, uint32_t y);

/**
 * Zwraca liczbę cyfr w dziesiętnym zapisie liczby @number
 */
uint16_t how_many_digits (uint32_t number);

/**
 * Wpisuje liczbę @number jako łańcuch znaków do stringa, którego @where
 * jest wskaźnikiem na koniec
 */
void write_number(uint32_t number, char *where);

#endif
