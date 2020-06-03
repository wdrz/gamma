#include "gamma.h"
#include "interactive-mode.h"
#include "basic_manipulations.h"
#include "enhanced-print.h"
#include "low-module.h"
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>

/* Pierwsza współrzędna obecnie zaznaczonego pola planszy do gry gamma */
static uint32_t CURRENT_X = 0;

/* Druga współrzędna obecnie zaznaczonego pola planszy do gry gamma */
static uint32_t CURRENT_Y = 0;

/* Stan gry gamma */
static gamma_t *BOARD;

/* Numer gracza, o którym oczekujemy że wykona ruch */
static uint32_t PLAYER = 1;

/* Ustawienia konsoli przed i po zainicjowaniu trybu interaktywnego */
static struct termios OLD_SETTING, NEW_SETTING;

/* Przesuwa kursor w miejsce gdzie na konsoli wyświetla się pole CURRENT_X, CURRENT_Y */
static inline void move_to_right_position(void) {
  printf("\x1b[%d;%df", BOARD->height - CURRENT_Y, CURRENT_X + 1);
}

/* Przesuwa kursor w lewy górny róg konsoli */
static inline void goto_start_position(void) {
  printf("\x1b[%d;%df", 1, 1);
}

/* Ustawia działanie konsoli */
static void init_console(void) {
  tcgetattr(0, &OLD_SETTING);          /* Pobierz stare ustawienia i/o terminala */
  NEW_SETTING = OLD_SETTING;
  NEW_SETTING.c_lflag &= ~ICANON;      /* wyłącz buforowanie znaków w linię */
  NEW_SETTING.c_lflag &= ~ECHO;        /* wyłącz echo terminala */
  tcsetattr(0, TCSANOW, &NEW_SETTING); /* użyj nowych ustawień terminala */
  printf("\033[?1049h\033[H");         /* nowy bufor */
}

/* Zwraca symbol, który będzie reprezentował gracza player w trybie interaktywnym */
static char player_character(uint32_t player) {
  if (player <= 9) return '0' + player;
  if (player <= 35) return 'A' + (player - 10);
  else return '?';
}

/* Przesuwa rozpatrywane pole w lewo, prawo, górę lub dół w zależności od
 * podanego jako parametr numeru sąsiada. Jeśli przesunięcie nie jest możliwe,
 * nie robi nic. */
static void if_has_neighbour_move(uint16_t n) {
  if ((CURRENT_X == 0 && X_DELTA[n] == -1) || (CURRENT_Y == 0 && Y_DELTA[n] == -1) ||
      CURRENT_X + X_DELTA[n] >= BOARD->width || CURRENT_Y + Y_DELTA[n] >= BOARD->height) {
        return;
  }
  CURRENT_X += X_DELTA[n];
  CURRENT_Y += Y_DELTA[n];
  move_to_right_position();
}

/* Poczynając od lewego górnego rogu konsoli, rysuje planszę do gry gamma.
 * zapisuje pozycję kursora, gdzie skończył */
static void draw_board(void) {
  printf("\033[2J"); // czyszczenie ekranu
  goto_start_position();
  print_enhanced_board(BOARD, PLAYER);
  move_to_right_position();
}

/* W zapisanym przez funkcję draw_board miejscu wypisuje numer i informacje o graczu */
static void draw_footer(void) {
  printf("\x1b%d", 8);  /* Idź do zapisanej pozycji kursora */
  printf("\x1b[%dK", 2); /* Wyczysc linie w ktorej znajduje się kursor */
  printf("PLAYER %c %lu %lu %s", player_character(PLAYER),
    gamma_busy_fields(BOARD, PLAYER), gamma_free_fields(BOARD, PLAYER),
    gamma_golden_possible(BOARD, PLAYER) ? "G" : "");
  move_to_right_position();
}

/* Kończy tryb interaktywny: wychodzi z bufora, wypisuje podsumowanie gry gamma */
static void finish(void) {
  uint32_t i;
  printf("\033[?1049l"); /* wyjdz z bufora */
  char *result = gamma_board(BOARD);

  tcsetattr(0, TCSANOW, &OLD_SETTING);

  printf("%s", result);
  free(result);
  for (i = 1; i <= BOARD->players; i++) {
    printf("PLAYER %u %lu\n", i, gamma_busy_fields(BOARD, i));
  }
}

/* Przeskakuje do następnego gracza, który nadal może wykonać ruch i zwraca true.
 * Jeśli takiego gracza nie ma, zwraca false */
static bool next_player(void) {
  uint16_t i, player;
  for (i = 0; i < BOARD->players; i++) {
    player = (PLAYER + i) % BOARD->players + 1;
    if (gamma_free_fields(BOARD, player) != 0 || gamma_golden_possible(BOARD, player)) {
      PLAYER = player;
      draw_board();
      draw_footer();
      return true;
    }
  }
  return false;
}

/* Sprawdza, czy wczytano obslugiwany ascii escape code (true) i na tej podstawie określa
 * wywołuje odpowienią funkcję. Zwraca false jeśli nie wczytał strzałki. */
static bool handle_arrows(int ch, int prev_ch, int prevprev_ch) {
  if (prevprev_ch != '\033' || prev_ch != '[' ) // escape + [
    return false;
  switch (ch) {
    case 'A': // GÓRA
      if_has_neighbour_move(1);
      break;
    case 'B': // DÓŁ
      if_has_neighbour_move(3);
      break;
    case 'C': // PRAWA
      if_has_neighbour_move(2);
      break;
    case 'D': // LEWA
      if_has_neighbour_move(0);
      break;
    default:
      return false;
  }
  return true;
}

/* zwraca true jeśli gra powinna się zakończyć, false w przeciwnym przypadku */
static bool handle_moves(int ch) {
  uint32_t x = CURRENT_X, y = CURRENT_Y;
  switch (ch) {
    case ' ':
      if (gamma_move(BOARD, PLAYER, x, y)) {
        actualise_low_after_move(BOARD, x, y);
        if (!next_player()) {
          finish();
          return true;
        }
      }
      break;
    case 4:
      finish();
      return true;
      break;
    case 'c':
    case 'C':
      next_player();
      break;
    case 'g':
    case 'G':
      if (gamma_golden_move(BOARD, PLAYER, CURRENT_X, CURRENT_Y)) {
        actualise_low_all(BOARD);
        if (!next_player()) {
          finish();
          return true;
        }
      }
      break;
  }
  return false;
}

void get_character(int *ch, int *prev_ch, int *prevprev_ch) {
  *prevprev_ch = *prev_ch;
  *prev_ch = *ch;
  *ch = getchar();
}

void run_interactive_mode(gamma_t *g) {
  BOARD = g;

  // wczytywany znak; poprzedni wczytany znak; znak dwa znaki temu
  int ch = 4, prev_ch = 4, prevprev_ch;

  init_console();
  draw_board();
  draw_footer();

  CURRENT_X = BOARD->width / 2;
  CURRENT_Y = BOARD->height / 2;
  move_to_right_position();

  for (;;) {
    get_character(&ch, &prev_ch, &prevprev_ch);
    if (!handle_arrows(ch, prev_ch, prevprev_ch) && handle_moves(ch)) return;
  }
}
