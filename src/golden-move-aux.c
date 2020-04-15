/** @file
 * Implementacja funkcji gamma_golden_move i wszystkich potrzebnych funkcji pomocniczych
 */

#include <stdlib.h>
#include "golden-move-aux.h"
#include "gamma-move-aux.h"

/// Pozycja w tablicy pola, które pojawiło się w zapytaniu o złoty ruch (dekl. w golden_move)
static uint64_t POSITION;

/// Gracz, który wykonuje złoty ruch, zmienna inicjowana w funkcji golden_move
static uint32_t PLAYER;

/// Właściciel pola na które @PLAYER chce wykonać złoty ruch (dekl. w golden_move)
static uint32_t OWNER;

/// Zmienna globalna inicjowana w funkcji golden_move, przechowuje planszę do gry gamma
static gamma_t *BOARD;

/// Liczby, które należy dodać do współrzędnych, aby przejść do sąsiada
static int X[8] = {-1, 0, 1,  0, -1, 1,  1, -1};
static int Y[8] = { 0, 1, 0, -1,  1, 1, -1, -1};

/** @brief Zmienia parametry pojedynczego pola przy wykonaniu złotego ruchu
 * Aktualizuje liczności pól graczy oraz ustawia pole na należące do
 * odpowiedniego gracza. Zapamiętuje, że gracz wykorzystał swój złoty ruch oraz
 * aktualizuje długość łańcucha znaków, który jest potrzeby aby wypisać planszę.
 */
void mod_field_count () {
  BOARD->player_fields[OWNER - 1]--;
  BOARD->player_fields[PLAYER - 1]++;
  BOARD->board[POSITION] = PLAYER;
  BOARD->player_golden_used[PLAYER - 1] = true;
  update_length_of_gamma_board(BOARD, OWNER, PLAYER);
}

/** @brief Sprawdza czy pole jest otoczone przez pola gracza @OWNER.
 * Sprawdza czy pole (@x, @y) jest otoczone przez pola gracza @OWNER.
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new.
 * @return true jeśli wszystie pola stykające się bokami lub rogami z polem
 * (@x, @y) należą do gracza @PLAYER, false w przeciwnym przypadku
 */
uint64_t in_the_middle_of_territory(uint32_t x, uint32_t y) {
  uint16_t i;
  for (i = 0; i < 8; i++)
    if (has_nth_neighbour(BOARD, i, x, y) && nth_neighbours_val(BOARD, i, x, y) != OWNER)
      return nth_neighbours_pos(BOARD, i, x, y);

  mod_field_count();
  BOARD->dsu[POSITION] = POSITION;
  BOARD->player_areas[PLAYER - 1]++;
  return POSITION;
}


/** @brief Sprawdza, czy argumenty golden_move są NIEpoprawne.
 * Wstępnie waliduje wejście funkcji golden_move.
 * @param[in] x       – numer kolumny,
 * @param[in] y       – numer wiersza.
 * @return false jeśli gracz wskazał pole leżące na planszy należące do innego gracza,
 * a true jeśli nie
 */
bool gold_input_incorrect(uint32_t x, uint32_t y) {
  return BOARD == NULL || PLAYER == 0 || PLAYER > (BOARD->players) ||
         BOARD->player_golden_used[PLAYER - 1] || (! is_addr_correct(BOARD, x, y)) ||
         BOARD->board[get_position(BOARD, x, y)] == 0 ||
         BOARD->board[get_position(BOARD, x, y)] == PLAYER;
}

/** @brief Sprawdza czy plansza jest zdegenerowana.
 * @return true jeśli któryś z wymiarów planszy @BOARD jest równy 1, false jeśli nie
 */
bool is_degenerated() {
  return BOARD->width == 1 || BOARD->height == 1;
}

/** @brief Aktualizuje liczby pól sąsiadujacych z obszarami graczy
 * Zabranie pola (@x, @y) graczowi @OWNER, przez gracza @PLAYER
 * sprawia, że część pól sąsiadujących z (@x, @y) może zacząć graniczyć
 * z obszarem gracza @PLAYER i przestać z gracza @OWNER. Funkcja odpowiednio
 * modyfikuje te wartości dla obu graczy. UWAGA! zakłada, że właściciel pola
 * (@x, @y) jest tymczasowo ustawiony na 0.
 * @param[in] x       – poprawny numer kolumny,
 * @param[in] y       – poprawny numer wiersza.
 */
void change_adjacency(uint32_t x, uint32_t y) {
  uint16_t i;
  for (i = 0; i < 4; i++)
    if (has_nth_neighbour_is_eq(BOARD, 0, i, x, y)) {
      if (! is_nth_neighbour_adjacent(BOARD, i, PLAYER, x, y))
        BOARD->player_adjacent[PLAYER - 1]++;
      if (! is_nth_neighbour_adjacent(BOARD, i, OWNER, x, y))
        BOARD->player_adjacent[OWNER - 1]--;
    }
}


/** @brief Przechodzi rekurencyjnie planszę do gry gamma
 * Przechodzi po obszarze do którego należy pole (@x, @y) gracza @value.
 * Następnik w dsu każdego odwiedzonego wierzchołka ustawia na @flag.
 * @param[in] flag    – flaga służąca do oznaczania odwiedzonych pól. Przy
 *                      jej pomocy oznaczamy następnika w dsu odwiedzonych pól.
 *                      Ważne jest, aby używać takiej flagi, która nie występuje
 *                      jako regularna wartość w polach, które chcemy przejść,
 * @param[in] value   – numer gracza po polach którego przechodzimy,
 * @param[in] x       – poprawny numer kolumny,
 * @param[in] y       – poprawny numer wiersza.
 */
void search(uint64_t flag, uint32_t value, uint32_t x, uint32_t y) {
  uint16_t i;
  BOARD->dsu[get_position(BOARD, x, y)] = flag;
  for (i = 0; i < 4; i++)
    if (has_nth_neighbour_is_eq(BOARD, value, i, x, y) &&
        BOARD->dsu[nth_neighbours_pos(BOARD, i, x, y)] != flag)
          search(flag, value, x + X[i], y + Y[i]);
}

/** @brief Zwraca liczbę obszarów na ile rozpada się obszar po zabraniu pola
 * Zwraca liczbę obszarów na ile rozpadłby się ten obszar gracza @OWNER do
 * w którym znajduje się pole (@x, @y) gdyby pole (@x, @y) przestało należeć
 * do tego gracza. Funkcja modyfikuje stan gry gamma: ustawia następnik wszystkich
 * pól leżących w tym samym obszarze gracza @PLAYER na @flag
 * @param[in] flag    – liczba taka, że nie istnieje pole należące do tego samego
 *                      obszaru gracza @OWNER co (@x, @y) takie, że jego następnik
 *                      w strukturze dsu jest ustwiony na flag,
 * @param[in] x       – poprawny numer kolumny,
 * @param[in] y       – poprawny numer wiersza.
 * @return
 */
uint16_t crumbles_to_how_many_parts(uint32_t x, uint32_t y, uint64_t flag) {
  uint16_t i, result = 0;
  BOARD->board[get_position(BOARD, x, y)] = 0;
  for (i = 0; i < 4; i++)
    if (has_nth_neighbour_is_eq(BOARD, OWNER, i, x, y) &&
        BOARD->dsu[nth_neighbours_pos(BOARD, i, x, y)] != flag) {
      result++;
      search(flag, OWNER, x + X[i], y + Y[i]);
    }
  return result;
}

/** @brief Sprawdza, czy obszar nie rozpada sie na nielegalnie wiele części
 * Sprawdza, czy gracz @whose_area może mieć @pieces obszarów więcej i
 * czy nie przekroczy w ten sposób maksymalnej liczby obszarów.
 * @return true jeśli nie przekroczy, false jeśli przekroczy
 */
bool can_be_divided(uint32_t whose_area, uint16_t pieces) {
  return BOARD->player_areas[whose_area - 1] + pieces - 1 <= BOARD->max_areas;
}

/** @brief Modyfikuje dsu tak aby zapamiętać rozpad obszaru.
 * Przechodzi po oflagowanych (@flag) polach gracza @OWNER i ustawia następniki
 * tych pól w dsu tak, aby oddawały zachodzący właśnie podział obszaru
 * @param[in] flag    – flaga
 * @param[in] x       – poprawny numer kolumny,
 * @param[in] y       – poprawny numer wiersza.
 */
void fix_after_search_when_divides(uint32_t x, uint32_t y, uint64_t flag) {
  uint16_t i;
  for (i = 0; i < 4; i++)
    if (has_nth_neighbour_is_eq(BOARD, OWNER, i, x, y) &&
        BOARD->dsu[nth_neighbours_pos(BOARD, i, x, y)] == flag)
          search(nth_neighbours_pos(BOARD, i, x, y), OWNER, x + X[i], y + Y[i]);
}

/** @brief Modyfikuje dsu tak, aby wycofać się rozkładania obszaru na części
 * Przechodzi po tym obszarze gracza @OWNER do którego należy pole (@x, @y)
 * i ustawia następniki tych pól w dsu na pole (@x, @y).
 * Zakłada, że każde z pól tego obszaru jest oflagowane (@flag) oraz że @flag nie
 * jest pozycją pola (@x, @y).
 * @param[in] flag    – flaga
 * @param[in] x       – poprawny numer kolumny,
 * @param[in] y       – poprawny numer wiersza.
 */
void fix_when_too_many_parts(uint32_t x, uint32_t y, uint64_t flag) {
  BOARD->board[get_position(BOARD, x, y)] = OWNER;
  BOARD->dsu[get_position(BOARD, x, y)] = flag;
  search(get_position(BOARD, x, y), OWNER, x, y);
  BOARD->dsu[get_position(BOARD, x, y)] = get_position(BOARD, x, y);
}

/** @brief Decyduje czy można podzielić obszar i go dzieli
 * Przeprowadza lub nie przeprowadza złotego ruchu
 * @param[in] flag    – pewna liczba, o której wiadomo, że żadne pole gracza
 *                      @OWNER nie ma ustawionego następnika w dsu na tą liczbę
 * @param[in] x       – poprawny numer kolumny,
 * @param[in] y       – poprawny numer wiersza.
 * @return true jeśli wykonano złoty ruch, false jeśli wykonanie złotego
 * ruchu sprawiłoby że obszary jakiegoś gracza przekroczyłyby maksimum
 */
bool crumble(uint32_t x, uint32_t y, uint64_t flag) {
  uint16_t parts = crumbles_to_how_many_parts(x, y, flag);

  if (can_be_divided(OWNER, parts)) {
    BOARD->player_areas[OWNER - 1] += parts - 1;
    fix_after_search_when_divides(x, y, flag);
    change_adjacency(x, y);
    update_dsu_and_areas(BOARD, PLAYER, x, y);
    mod_field_count();
    return true;
  } else {
    fix_when_too_many_parts(x, y, flag);
    return false;
  }
}


bool gamma_golden_move(gamma_t *g_temp, uint32_t player_temp, uint32_t x, uint32_t y) {
  PLAYER = player_temp;
  BOARD = g_temp;
  if (gold_input_incorrect(x, y) || (! is_adjacent(BOARD, PLAYER, x, y)
      && BOARD->player_areas[PLAYER - 1] == BOARD->max_areas)) {
        return false;
  } else {
    uint64_t flag;
    POSITION = get_position(BOARD, x, y);
    OWNER = BOARD->board[POSITION];
    if (is_degenerated()) {
      flag = -1;
    } else {
      flag = in_the_middle_of_territory(x, y);
    }
    return flag == POSITION || crumble(x, y, flag);
  }
}
