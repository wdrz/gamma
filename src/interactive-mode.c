#include "gamma.h"
#include "interactive-mode.h"
#include "basic_manipulations.h"
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
  uint16_t i, j;
  uint32_t owner;
  goto_start_position();
  for (j = BOARD->height; j > 0; j--) {
    for (i = 0; i < BOARD->width; i++) {
      owner = BOARD->board[get_position(BOARD, i, j - 1)];
      if (owner == 0) {
        printf(".");
      } else {
        printf("%c", player_character(owner));
      }
    }
    printf("\n");
  }
  printf("\x1b%d", 7); // zapisz pozycje kursora
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

/* Przeskakuje do następnego gracza, który nadal może wykonać ruch i zwraca true.
 * Jeśli takiego gracza nie ma, zwraca false */
static bool next_player(void) {
  uint16_t i, player;
  for (i = 0; i < BOARD->players; i++) {
    player = (PLAYER + i) % BOARD->players + 1;
    if (gamma_free_fields(BOARD, player) != 0 || gamma_golden_possible(BOARD, player)) {
      PLAYER = player;
      draw_footer();
      return true;
    }
  }
  return false;
}

/* Kończy tryb interaktywny: wychodzi z bufora, wypisuje podsumowanie gry gamma */
static void finish() {
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

/* Aktualizuje wyświetlaną tablicę o ruch gracza PLAYER na pole CURRENT_X,
 * CURRENT_Y. Zwraca true jeśli gra gamma powinna się zakończyć, false wpp */
static bool display_move(void) {
  printf("%c", player_character(PLAYER));
  move_to_right_position();
  if (!next_player()) {
    finish();
    return true;
  }
  return false;
}

/* Interpretuje drugi i trzeci znak ansi escape code'a: na ich podstawie określa
 * czy naciśnięta zastała strzałka i jeśli tak to wywołuje odpowienią funkcję */
static void handle_arrows() {
  char ch;
  ch = getchar();
  if (ch != '[') return;
  ch = getchar();
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
  }
}

void run_interactive_mode(gamma_t *g) {
  BOARD = g;
  int ch;

  init_console();
  draw_board();
  draw_footer();

  CURRENT_X = BOARD->width / 2;
  CURRENT_Y = BOARD->height / 2;
  move_to_right_position();

  for (;;) {
    ch = getchar();
    switch (ch) {
      case '\033': // escape
        handle_arrows();
        break;
      case ' ':
        if (gamma_move(BOARD, PLAYER, CURRENT_X, CURRENT_Y) && display_move())
          return;
        break;
      case 4:
        finish();
        return;
        break;
      case 'c':
      case 'C':
        next_player();
        break;
      case 'g':
      case 'G':
        if (gamma_golden_move(BOARD, PLAYER, CURRENT_X, CURRENT_Y) && display_move())
          return;
        break;
    }
  }
}
