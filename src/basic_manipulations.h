/** @file
 * Udostępnia podstawowe operacje na polach planszy
 */


#ifndef BASIC_MANIPULATIONS_H
#define BASIC_MANIPULATIONS_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Struktura przechowująca stan gry gamma
 */
typedef struct gamma {
  uint64_t *dsu;            /**<dla każdego pola trzyma jego następnik w
                             * implementacji struktury zbiorów rozłącznych */
  uint32_t *board;          /**<dla każdego pola trzyma numer gracza do którego
                             * to pole należy lub 0 jeśli jest to puste pole */
  uint32_t *player_fields;  ///<dla każdego gracza trzyma liczbę zajętych przez niego pól
  uint32_t *player_areas;   ///<dla każdego gracza trzyma liczbę jego obszarów na planszy
  uint32_t *player_adjacent;///<dla każdego gracza trzyma liczbę pól niczyich sąsiadujących
  bool *player_golden_used; ///<dla każdego gracza trzyma infromację czy wykorzystał już swój złoty ruch
  uint32_t players;         ///<liczba graczy
  uint32_t width;           ///<szerokość planszy
  uint32_t height;          ///<wysokość planszy
  uint32_t max_areas;       ///<maksymalna liczba obszarów
  uint64_t empty_fields;    ///<liczba pustych pól na planszy
  uint64_t legthOfString;   ///<długość łańcucha znaków, opisującego stan planszy
} gamma_t;


/** Sprawdza czy @p x i @p y to poprawne współrzędne pola na planszy @p g
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] x       – poprawny numer kolumny,
 * @param[in] y       – poprawny numer wiersza,
 * @return true jeśli są poprawne i false jeśli nie są
 */
bool is_addr_correct(gamma_t *g, uint32_t x, uint32_t y);


/** @brief Zwraca pozycję w tablicy pola zadanego przez wspolrzedne.
 * Zwraca liczbę za pomocą której można odwoływać się do pola (@p x, @p y)
 * z tablicach trzymających informacje o poszczególnych polach w strukturze gamma.
 * Zakłada, że dane wejściowe są poprawne.
 * @param[in] g       – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] x       – poprawny numer kolumny,
 * @param[in] y       – poprawny numer wiersza.
 * @return index w tablicy @ref gamma ->board pola (@p x, @p y)
 */
uint64_t get_position(gamma_t *g, uint32_t x, uint32_t y);


/** Zwraca pozycję reprezentatnta pola w strukturze dsu pól planszy
 * @param[in] g        – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] position – pozycja pola (wynik funkcji @ref get_position dla współrzędnych pola)
 * @return wynik funkcji @ref get_position dla współrzędnych reprezentanta pola w dsu
 */
uint64_t find(gamma_t *g, uint64_t position);



/** Zwraca numer gracza do którego należy @p n -ty sąsied pola (@p x, @p y)
 * Zakłada, że pole (@p x, @p y) posiada @p n-tego sąsiada
 * @param[in] g        – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] n        – numer sąsiada (liczba 0, 1, ..., 7) oznaczająca
 *                       odpowiednio sąsaida lewego, górnego, ..., lewego-górnego, ...,
 * @param[in] x        – poprawny numer kolumny,
 * @param[in] y        – poprawny numer wiersza.
 * @return numer gracza do którego należy @p n -ty sąsied pola (@p x, @p y).
 * 0 jeśli sąsiad jest pustym polem.
 */
uint32_t nth_neighbours_val(gamma_t *g, uint16_t n, uint32_t x, uint32_t y);

/** Zwraca pozycję w tablicy @p n-tego sąsiada pola (@p x, @p y)
 * Zakłada, że pole (@p x, @p y) posiada @p n-tego sąsiada
 * @param[in] g        – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] n        – numer sąsiada (liczba 0, 1, ..., 7) oznaczające
 *                       odpowiednio sąsaida lewego, górnego, ..., lewego-górnego, ...,
 * @param[in] x        – poprawny numer kolumny,
 * @param[in] y        – poprawny numer wiersza.
 * @return wynik @ref get_position dla @p n -tego sąsiada pola (@p x, @p y)
 */
uint32_t nth_neighbours_pos(gamma_t *g, uint16_t n, uint32_t x, uint32_t y);

/** Sprawdza, czy pole (@p x, @p y) ma @p n -tego sąsiada
* @param[in] g        – wskaźnik na strukturę przechowującą stan gry,
* @param[in] n        – numer sąsiada (liczba 0, 1, ..., 7) oznaczająca
*                       odpowiednio sąsaida lewego, górnego, ..., lewego-górnego, ...,
* @param[in] x        – poprawny numer kolumny,
* @param[in] y        – poprawny numer wiersza.
* @return true jeśli posiada, false jeśli nie posiada
*/
bool has_nth_neighbour(gamma_t *g, uint16_t n, uint32_t x, uint32_t y);

/** @brief Sprawdza, czy pole (@p x, @p y) sąsiaduje z polem gracza @p player
 * @param[in] g        – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] player   – numer gracza lub 0 (puste pole),
 * @param[in] x        – poprawny numer kolumny,
 * @param[in] y        – poprawny numer wiersza.
 * @return true jesli pole (@p x, @p y) sasiaduje z polem gracza @p player, false
 * w przeciwnym przypadku.
 */
bool is_adjacent(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Sprawdza, czy pole ma sąsiada i czy należy on do gracza @p value
 * @param[in] g        – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] value    – numer gracza lub 0 (puste pole),
 * @param[in] n        – numer sąsiada (liczba 0, 1, ..., 7) oznaczająca
 *                       odpowiednio sąsaida lewego, górnego, ..., lewego-górnego, ...,
 * @param[in] x        – poprawny numer kolumny,
 * @param[in] y        – poprawny numer wiersza.
 * @return true jesli @p n -ty sąsiad pola (@p x, @p y) istnieje i należy do
 * gracza @p value, false w przeciwnym przypadku.
 */
bool has_nth_neighbour_is_eq(gamma_t *g, uint64_t value, uint16_t n, uint32_t x, uint32_t y);

/**
 * Sprawdza, czy przynajmniej jeden z sąsiadów pola (@p x, @p y) o numerach
 * 0, ..., @p n - 1 należy do gracza @p value
 * @param[in] g        – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] n        – numer sąsiada (liczba 0, 1, ..., 7) oznaczająca
 *                       odpowiednio sąsaida lewego, górnego, ..., lewego-górnego, ...,
 * @param[in] value    – numer gracza lub 0 (puste pole),
 * @param[in] x        – poprawny numer kolumny,
 * @param[in] y        – poprawny numer wiersza.
 * @return true jeśli co najmniej jeden sąsiad o numerze należącym do zbioru (0, ..., @p n - 1)
 * istnieje i należy ma przypisaną wartość @p value (należy do gracza @p value lub jest pusty gdy @p value = 0)
 */
bool scan_neighbours (gamma_t *g, uint16_t n, uint64_t value, uint32_t x, uint32_t y);

/**
 * Sprawdza, czy @p n-ty sąsiad pola (@p x, @p y) sąsiaduje z polem należącym do gracza @p value.
 * Zakłada że @p n-ty sąsiad pola (@p x, @p y) istnieje.
 * @param[in] g        – wskaźnik na strukturę przechowującą stan gry,
 * @param[in] n        – numer sąsiada (liczba 0, 1, ..., 7) oznaczająca
 *                       odpowiednio sąsaida lewego, górnego, ..., lewego-górnego, ...,
 * @param[in] value    – numer gracza lub 0 (puste pole),
 * @param[in] x        – poprawny numer kolumny,
 * @param[in] y        – poprawny numer wiersza.
 * @return true jeśli @p n-ty sąsiad pola (@p x, @p y) sąsiaduje z polem należącym do gracza @p value
 */
bool is_nth_neighbour_adjacent(gamma_t *g, uint16_t n, uint32_t value, uint32_t x, uint32_t y);

/** Zwraca liczbę cyfr w dziesiętnym zapisie liczby @p number
 * @param[in] number     – liczba.
 * @return liczba cyfr w dziesiętnym zapisie liczby @p number
 */
uint16_t how_many_digits (uint32_t number);

/** Wpisuje liczbę @p number jako łańcuch znaków do stringa, którego @p where
 * jest wskaźnikiem na koniec.
 * @param[in] number         – liczba do wpisania,
 * @param[in,out] where      – koniec fragmentu pamięci do którego funkcja wpisuje cyfry.
 */
void write_number(uint32_t number, char *where);

#endif
