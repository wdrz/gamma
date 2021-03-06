#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "gamma.h"
#include "basic_manipulations.h"
#include "gamma-move-aux.h"
#include "enhanced-print.h"
#include "low-module.h"


void gamma_delete(gamma_t *g) {
  if (g == NULL) return;
  free(g->dsu);
  free(g->board);
  free(g->player_fields);
  free(g->player_areas);
  free(g->player_adjacent);
  free(g->player_golden_used);
  free(g->num_of_bridges);
  free(g->preorder);
  free(g);
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
    g->empty_fields = (uint64_t) width * (uint64_t) height;
    g->length_of_string = (width + 1) * height + 1;
    g->low_updated = false;

    g->dsu = calloc((uint64_t) width * (uint64_t) height, sizeof(uint64_t));
    g->board = calloc((uint64_t) width * (uint64_t) height, sizeof(uint32_t));
    g->player_fields = calloc(players, sizeof(uint32_t));
    g->player_areas = calloc(players, sizeof(uint32_t));
    g->player_adjacent = calloc(players, sizeof(uint32_t));
    g->player_golden_used = calloc(players, sizeof(bool));
    g->num_of_bridges = calloc((uint64_t) width * (uint64_t) height, sizeof(uint16_t));
    g->preorder = calloc((uint64_t) width * (uint64_t) height, sizeof(uint64_t));
    if (g->dsu == NULL || g->board == NULL || g->player_areas == NULL ||
        g->player_adjacent == NULL || g->player_golden_used == NULL ||
        g->preorder == NULL || g->num_of_bridges == NULL) {
          gamma_delete(g);
          return NULL;
    }
    return g;
}


uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
  if (g == NULL || player > g->players || player == 0)
    return 0;
  else
    return g->player_fields[player - 1];
}


uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
  if (g == NULL || player > g->players || player == 0)
    return 0;
  else if (g->player_areas[player - 1] < g->max_areas)
    return g->empty_fields;
  else
    return g->player_adjacent[player - 1];
}

static bool golden_possible_iteration(gamma_t *g, uint32_t player) {
  uint16_t i, j;
  for (j = 0; j < g->height; j++)
    for (i = 0; i < g->width; i++)
      if (golden_possible_to_field(g, player, i, j))
        return true;

  return false;
}

bool gamma_golden_possible(gamma_t *g, uint32_t player) {
  if (!old_golden_possible(g, player))
    return false;

  if (!g->low_updated)
    actualise_low_all(g);

  return golden_possible_iteration(g, player);
}


char* gamma_board(gamma_t *g) {
  uint32_t i, j;
  if (g == NULL) return NULL;
  char *board = malloc(g->length_of_string);
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
