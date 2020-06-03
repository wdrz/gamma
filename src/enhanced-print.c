#include "enhanced-print.h"
#include "basic_manipulations.h"
#include <stdlib.h>
#include <stdio.h>

#define GREEN "\x1b[32m"
#define BLUE "\x1b[44m"
#define RESET "\x1b[0m"

static char player_character(uint32_t player) {
  if (player <= 9) return '0' + player;
  if (player <= 35) return 'A' + (player - 10);
  else return '?';
}


static void print_enhanced_field(gamma_t *g, uint32_t x, uint32_t y, uint32_t player) {
  bool max_areas_reached = g->player_areas[player - 1] == g->max_areas, adjacent;
  uint64_t pos = get_position(g, x, y);

  if (g->board[pos] == player) {
    printf("%c", player_character(player));
    return;
  }

  adjacent = is_adjacent(g, player, x, y);

  if (g->board[pos] == 0) {

    if (max_areas_reached && !adjacent) {
      printf(".");
    } else {
      printf(GREEN);
      printf(".");
      printf(RESET);
    }

  } else { // pole przeciwnika
    uint32_t owner = g->board[pos];
    if (old_golden_possible(g, player)) {
      bool can_be_removed = g->player_areas[owner - 1] + g->num_of_bridges[pos] - 1 <= g->max_areas;
      if ((max_areas_reached && !adjacent) || !can_be_removed) {
        printf("%c", player_character(owner));
      } else {
        printf(BLUE);
        printf("%c", player_character(owner));
        printf(RESET);
      }
    } else {
      printf("%c", player_character(owner));
    }
  }
}

void print_enhanced_board(gamma_t *g, uint32_t player) {
  uint16_t i, j;
  for (j = g->height; j > 0; j--) {
    for (i = 0; i < g->width; i++) {
      print_enhanced_field(g, i, j - 1, player);
    }
    printf("\n");
  }
  printf("\x1b%d", 7); // zapisz pozycje kursora
}
