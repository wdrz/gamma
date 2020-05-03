#include "gamma.h"
#include "interactive-mode.h"
#include "basic_manipulations.h"
#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>

uint16_t FOOTER_WINDOW_LENGTH = 20;

uint32_t CURRENT_X = 0;
uint32_t CURRENT_Y = 0;

gamma_t *BOARD;

WINDOW *GAME_WINDOW;
WINDOW *FOOTER_WINDOW;

static uint32_t PLAYER = 1;

/// Tabela różnic pierwszych wpółrzędnych pomiędzy dowolnym polem, a jego i-tym sąsiadem
static const int X[4] = {-1, 0, 1,  0};

/// Tabela różnic drugich wpółrzędnych pomiędzy dowolnym polem, a jego i-tym sąsiadem
static const int Y[4] = { 0, 1, 0, -1};

void move_to_right_position() {
  wmove(GAME_WINDOW, BOARD->height - 1 - CURRENT_Y, CURRENT_X);
}

void if_has_neighbour_move(uint16_t n) {
  if ((CURRENT_X == 0 && X[n] == -1) || (CURRENT_Y == 0 && Y[n] == -1) ||
      CURRENT_X + X[n] >= BOARD->width || CURRENT_Y + Y[n] >= BOARD->height) {
        return;
  }
  CURRENT_X += X[n];
  CURRENT_Y += Y[n];
  move_to_right_position();
  wrefresh(GAME_WINDOW);
}

void init_ncurses() {
  initscr();
  cbreak(); // One-character-a-time: disable the buffering of typed characters
  noecho(); // No echo: suppress the automatic echoing of typed characters
  clear();
  keypad(stdscr, TRUE); // Special keys: capture special keystrokes
  refresh();
}

void draw_board() {
  uint16_t i, j;
  uint32_t board_field;
  for (j = 0; j < BOARD->height; j++)
    for (i = 0; i < BOARD->width; i++) {
      board_field = BOARD->board[get_position(BOARD, i, j)];
      if (board_field == 0) {
        mvwaddch(GAME_WINDOW, BOARD->height - 1 - j, i, '.');
      } else {
        mvwaddch(GAME_WINDOW, BOARD->height - 1 - j, i, '0' + board_field);
      }
    }
  move_to_right_position();
}


void draw_initial_board() {
  GAME_WINDOW = newwin(BOARD->height, BOARD->width, 0, 0);
  wmove(GAME_WINDOW, 0, 0);
  wrefresh(GAME_WINDOW);
  draw_board();
  wrefresh(GAME_WINDOW);
}

void draw_footer() {
  uint16_t i, length;
  char str[FOOTER_WINDOW_LENGTH];
  length = sprintf(str, "PLAYER %u %lu %lu %c", PLAYER,
    gamma_busy_fields(BOARD, PLAYER), gamma_free_fields(BOARD, PLAYER),
    gamma_golden_possible(BOARD, PLAYER) ? 'G' : ' ');
  wmove(FOOTER_WINDOW, 0, 0);
  for (i = 0; i < FOOTER_WINDOW_LENGTH; i++) {
    if (i < length)
      waddch(FOOTER_WINDOW, str[i]);
    else
      wdelch(FOOTER_WINDOW);
  }

  wrefresh(FOOTER_WINDOW);

  move_to_right_position();
  wrefresh(GAME_WINDOW);

}

void draw_initial_footer() {
  FOOTER_WINDOW = newwin(1, FOOTER_WINDOW_LENGTH, BOARD->height, 0);
  draw_footer();
}

void next_player() {
  PLAYER = (PLAYER % BOARD->players) + 1;
  draw_footer();
}

// TODO
bool is_it_the_end() {
  return false;
}

void finish() {
  char *result = gamma_board(BOARD);
  endwin();
  printf("%s", result);
  free(result);
  // jeszcze jakies podsumowania !!!
}

int run_interactive_mode(gamma_t *g) {
  BOARD = g;
  int ch;

  init_ncurses();

  draw_initial_board();
  draw_initial_footer();
  refresh();


  CURRENT_X = BOARD->width / 2;
  CURRENT_Y = BOARD->height / 2;
  move_to_right_position();
  wrefresh(GAME_WINDOW);


  for (;;) {
    if (is_it_the_end()) {
      finish();
      return 0;
    }
    ch = getch();
    switch (ch) {
      case KEY_UP:
        if_has_neighbour_move(1);
        break;
      case KEY_DOWN:
        if_has_neighbour_move(3);
        break;
      case KEY_LEFT:
        if_has_neighbour_move(0);
        break;
      case KEY_RIGHT:
        if_has_neighbour_move(2);
        break;
      case ' ':
        if (gamma_move(BOARD, PLAYER, CURRENT_X, CURRENT_Y)) {
          draw_board();
          wrefresh(GAME_WINDOW);
          next_player();
        }
        break;
      case 4:
        finish();
        return 0;
        break;
      case 'c':
      case 'C':
        next_player();
        break;
      case 'g':
      case 'G':
        if (gamma_golden_move(BOARD, PLAYER, CURRENT_X, CURRENT_Y)) {
          draw_board();
          wrefresh(GAME_WINDOW);
          next_player();
        }
        break;
    }
  }
  endwin();
  return 0;
}
