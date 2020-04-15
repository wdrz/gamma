# include <stdio.h>
# include <stdbool.h>
# include <stdint.h>
# include <stdlib.h>

# include "gamma.h"
# include "basic_manipulations.h"
# include "gamma-move-aux.h"
# include "basic_manipulations.h"

void gamma_delete(gamma_t *g) {
  if (g == NULL) return;
  free (g->board);
  free (g->dsu);
  free (g->player_fields);
  free (g->player_areas);
  free (g->player_adjacent);
  free (g->player_golden_used);
  free (g);
}

gamma_t* gamma_new(uint32_t width, uint32_t height, uint32_t players, uint32_t areas) {
    if (width == 0 || height == 0 || players == 0 || areas == 0)
      return NULL;

    gamma_t *g = malloc(sizeof(struct gamma));
    if (g == NULL) return NULL;

    g->players = players;
    g->width = width;
    g->height = height;
    g->max_areas = areas;
    g->empty_fields = width * height;
    g->legthOfString = (width + 1) * height + 1;

    g->dsu = calloc(width * height, sizeof (uint64_t));
    g->board = calloc(width * height, sizeof (uint32_t));
    g->player_fields = calloc(players, sizeof (uint32_t));
    g->player_areas = calloc(players, sizeof (uint32_t));
    g->player_adjacent = calloc(players, sizeof (uint32_t));
    g->player_golden_used = calloc(players, sizeof (bool));
    if (g->dsu == NULL || g->board == NULL || g->player_areas == NULL ||
        g->player_adjacent == NULL || g->player_golden_used == NULL) {
          free(g);
          return NULL;
    }
    return g;
}

bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  if (gm_input_incorrect(g, player, x, y))
    return false;
  if (too_many_areas(g, player, x, y))
    return false;

  update_adjacency(g, player, x, y);
  update_dsu_and_areas(g, player, x, y);

  g->board[get_position(g, x, y)] = player;
  g->player_fields[player - 1]++;
  g->empty_fields--;
  update_length_of_gamma_board(g, 0, player);
  //gamma_debug(g);

  return true;
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
  return g->player_fields[player - 1];
}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
  if (g->player_areas[player - 1] < g -> max_areas)
    return g->empty_fields;
  else
    return g->player_adjacent[player - 1];
}

bool gamma_golden_possible(gamma_t *g, uint32_t player) {
  return (! g->player_golden_used[player - 1]) &&
    ((g->width * g->height) - (g->empty_fields) -
    (g->player_fields[player - 1]) > 0);
}

void write_number(uint32_t number, char *where) {
  *where = '0' + (number % 10);
  if (number >= 10)
    write_number(number / 10, where - 1);
}

char* gamma_board(gamma_t *g) {
  uint32_t i, j;
  if (g == NULL) return NULL;
  char *board = malloc(g->legthOfString);
  char *iterator = board;
  if (board == NULL) return NULL;
  for (j = g->height; j > 0; j--) {
    for (i = 0; i < g->width; i++) {
      uint32_t number = g->board[get_position(g, i, j - 1)];
      if (number == 0) {
        *iterator = '.';
      } else if (number <= 9) {
        *iterator = '0' + number;
      } else {
        *iterator = '(';
        iterator += how_many_digits(number);
        write_number(number, iterator);
        iterator++;
        *iterator = ')';
      }
      iterator++;
    }
    *iterator = '\n';
    iterator++;
  }
  *iterator = '\0';
  return board;
}
