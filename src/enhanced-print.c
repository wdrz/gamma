#include "enhanced-print.h"
#include "basic_manipulations.h"
#include <stdlib.h>
#include <stdio.h>

#define GREEN "\033[32m"
#define BRIGHT "\033[0;31m"
#define SKY "\033[0;36m"
#define RESET "\033[0m"

char player_character(uint32_t player) {
  if (player <= 9) return '0' + player;
  if (player <= 35) return 'A' + (player - 10);
  else return '?';
}

uint16_t crumbles_to(gamma_t *g, uint32_t owner, uint32_t x, uint32_t y, uint64_t pos) {
  uint16_t quant = g->num_of_bridges[pos];
  if (quant % 10 == 0) {
    if (quant > 9) return 2;
    else if (num_of_adjacent(g, owner, x, y) != 0) return 1;
    else return 0;
  } else {
    if (quant % 10 == num_of_adjacent(g, owner, x, y)) return quant % 10;
    else return (quant % 10) + 1;
  }
}

// zaklada ze pola x, y należy do gracza innego niż player.
static bool golden_possible_to_field_aux(gamma_t *g, uint32_t player, uint32_t x, uint32_t y, bool adjacent, bool max_areas_reached) {
  if (!old_golden_possible(g, player)) {
    return false;
  } else {
    uint64_t pos = get_position(g, x, y);
    uint32_t owner = g->board[pos];
    bool can_be_removed = g->player_areas[owner - 1] +
      crumbles_to(g, owner, x, y, pos) - 1 <= g->max_areas;
    return (!max_areas_reached || adjacent) && can_be_removed;
  }
}

// zakłada, że parametry są poprawne
bool golden_possible_to_field(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  if (g->board[get_position(g, x, y)] == 0 || g->board[get_position(g, x, y)] == player) {
    return false;
  } else {
    bool max_areas_reached = (g->player_areas[player - 1] == g->max_areas);
    bool adjacent = is_adjacent(g, player, x, y);
    return golden_possible_to_field_aux(g, player, x, y, adjacent, max_areas_reached);
  }
}

static void print_enhanced_field(gamma_t *g, uint32_t x, uint32_t y, uint32_t player) {
  bool max_areas_reached = g->player_areas[player - 1] == g->max_areas, adjacent;
  uint64_t pos = get_position(g, x, y);
  uint32_t owner = g->board[pos];

  if (owner == player) {
    printf(BRIGHT);
    printf("%c", player_character(player));
    printf(RESET);
    return;
  }

  adjacent = is_adjacent(g, player, x, y);

  if (owner == 0) {

    if (max_areas_reached && !adjacent) {
      printf(".");
    } else {
      printf(GREEN);
      printf(".");
      printf(RESET);
    }

  } else if (golden_possible_to_field_aux(g, player, x, y, adjacent, max_areas_reached)) {
    printf(SKY);
    printf("%c", player_character(owner));
    printf(RESET);
  } else {
    printf("%c", player_character(owner));
  }
}

void print_enhanced_board(gamma_t *g, uint32_t player) {
  uint32_t i, j;
  printf("\e[?25l"); // ukryj kursor
  for (j = g->height; j > 0; j--) {
    for (i = 0; i < g->width; i++) {
      print_enhanced_field(g, i, j - 1, player);
    }
    printf("\n");
  }
  printf("\x1b%d", 7); // zapisz pozycje kursora
  printf("\e[?25h"); // pokaż kursor
}
