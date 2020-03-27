# include <stdio.h>
# include <stdbool.h>
# include <stdint.h>
# include "gamma.h"
# include <stdlib.h>

int X[4] = {1, -1, 0, 0};
int Y[4] = {0, 0, 1, -1};

struct gamma {
  uint32_t *board, *player_fields, *player_areas, *player_adjacent;
  uint32_t players, width, height, areas;
  uint64_t empty_fields;
  bool *player_golden_used;
};

gamma_t* gamma_new(uint32_t width, uint32_t height,
  uint32_t players, uint32_t areas) {
    // TODO: powinien sie wywalac jak niepoprawne argumenty albo nie da sie zaalokowac
    gamma_t *g = malloc (sizeof(struct gamma));
    g -> players = players;
    g -> width = width;
    g -> height = height;
    g -> areas = areas;
    g -> empty_fields = width * height;
    g -> board = calloc (0, sizeof (uint32_t) * width * height);
    g -> player_fields = calloc (0, sizeof (uint32_t) * players);
    g -> player_areas = calloc (0, sizeof (uint32_t) * players);
    g -> player_adjacent = calloc (0, sizeof (uint32_t) * players);
    g -> player_golden_used = calloc (false, sizeof (bool) * players);
    return g;
}

void gamma_delete(gamma_t *g) {
  free (g -> board);
  free (g -> player_fields);
  free (g -> player_areas);
  free (g -> player_adjacent);
  free (g -> player_golden_used);
  free (g);
}

/* Zwraca wskaznik na fragment pamieci przechowujacej dane dla pola x, y
   jesli nie ma takiego pola, zwraca NULL */
uint32_t* get_field (gamma_t *g, uint32_t x, uint32_t y) {
  if (x < g -> width && y < g -> height)
    return (g -> board + (g -> height * y) + x);
  else
    return NULL;
}

bool field_eq (uint32_t* f, uint32_t value) {
  if (f == NULL) return false;
  return (*f) == value;
}

/* Sprawdza, czy pole sasiaduje z polem gracza */
bool is_field_adjacent (gamma_t *g, uint32_t x, uint32_t y, uint32_t player) {
  if (field_eq(get_field(g, x + 1, y), player) ||
      field_eq(get_field(g, x - 1, y), player) ||
      field_eq(get_field(g, x, y + 1), player) ||
      field_eq(get_field(g, x, y - 1), player) )
    return true;
  else
    return false;
}

bool gamma_move (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  uint32_t i;
  if (player == 0 || g -> players < player)
    return false;
  uint32_t *f = get_field(g, x, y);
  if (f == NULL) return false;
  if (*f != 0) return false;

  if (is_field_adjacent(g, x, y, player)) {
    g -> player_adjacent[player - 1] --;
  } else {
    if (g -> player_areas[player - 1] >= g -> areas) {
      return false;
    }
    g -> player_areas[player - 1] ++;
  }
  for (i = 0; i < 4; i++) {
    if (! is_field_adjacent(g, x + X[i], y + Y[i], player) &&
          field_eq(get_field(g, x + X[i], y + Y[i]), 0)) {
      g -> player_adjacent[player - 1] ++;
    }
  }
  g -> empty_fields --;
  g -> player_fields[player - 1] ++;
  *f = player;
  return true;
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  // TODO !!!!!!!!!
  return false;
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
  return g -> player_fields[player - 1];
}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
  if (g -> player_areas[player - 1] < g -> areas)
    return g -> empty_fields;
  else
    return g -> player_adjacent[player - 1];
}

bool gamma_golden_possible(gamma_t *g, uint32_t player) {
  return (! (g -> player_golden_used[player - 1])) &&
    ((g -> width * g -> height) - (g -> empty_fields) -
    (g -> player_fields[player - 1]) > 0);
}

char* gamma_board(gamma_t *g) {
  char *board = malloc ((g -> width + 1) * g -> height + 1);
  if (board == NULL) return false;
  uint32_t i, j;
  for (i = 0; i < g -> width; i++) {
    for (j = 0; j < g -> height; j++) {
      board[(g -> width + 1) * j + i] = (char) '0' - 1 + *get_field(g, i, j);
    }
    board[(g -> width + 1) * j + g -> width] = '\n';
  }
  board[(g -> width + 1) * g-> height] = '\0';
  return board;
}

int main (void) {
  return 0;
}
