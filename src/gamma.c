# include <stdio.h>
# include <stdbool.h>
# include <stdint.h>
# include <stdlib.h>

# include "gamma.h"
# include "basic_manipulations.h"
# include "gamma-move-aux.h"
# include "basic_manipulations.h"

/// musi dealokować gamma_t!!!!!, musi zwracać NULL a nie się wywalać !!!
gamma_t* gamma_new (uint32_t width, uint32_t height, uint32_t players, uint32_t areas) {
    if (width == 0 || height == 0 || players == 0)
      return NULL;
    gamma_t *g = malloc (sizeof(struct gamma));
    if (g == NULL) return NULL;

    g -> players = players;
    g -> width = width;
    g -> height = height;
    g -> max_areas = areas;
    g -> empty_fields = width * height;
    g -> legthOfString = (width + 1) * height;

    g -> dsu = calloc (width * height, sizeof (uint64_t)); // byc moze tu wystarczy malloc
    g -> board = calloc (width * height, sizeof (uint32_t));
    g -> player_fields = calloc (players, sizeof (uint32_t));
    g -> player_areas = calloc (players, sizeof (uint32_t));
    g -> player_adjacent = calloc (players, sizeof (uint32_t));
    g -> player_golden_used = calloc (players, sizeof (bool));
    if (g -> dsu == NULL || g -> board == NULL || g -> player_areas == NULL
    || g -> player_adjacent == NULL || g -> player_golden_used == NULL)
      return NULL;
    return g;
}

void gamma_delete(gamma_t *g) {
  if (g == NULL) return;
  free (g -> board);
  free (g -> dsu);
  free (g -> player_fields);
  free (g -> player_areas);
  free (g -> player_adjacent);
  free (g -> player_golden_used);
  free (g);
}

void gamma_debug(gamma_t *g) {
  uint32_t i, j;
  for (i = 0; i < g -> width; i++) {
    for (j = 0; j < g -> height; j++) {
      if (g -> board[get_position(g, i, j)] == 0) {
        printf (".");
      } else {
        printf("%d", g -> board[get_position(g, i, j)]);
      }
    }
    printf("\n");
  }
  printf("-------------\n");
  printf("Player areas:\n");
  for (i = 0; i < g -> players; i++) {
    printf("%d ", g -> player_areas[i]);
  }
  printf("\n-------------\n");
  printf("Player fields:\n");
  for (i = 0; i < g -> players; i++) {
    printf("%d ", g -> player_fields[i]);
  }
  printf("\n-------------\n");
  printf("Player adjacent:\n");
  for (i = 0; i < g -> players; i++) {
    printf("%d ", g -> player_adjacent[i]);
  }
  printf("\n-------------\n");
}


bool gamma_move (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  if (gm_input_incorrect(g, player, x, y))
    return false;
  if (too_many_areas(g, player, x, y))
    return false;

  update_adjacency(g, player, x, y);

  update_dsu(g, player, x, y);

  g -> board[ get_position(g, x, y) ] = player;
  g -> player_fields[player - 1]++;
  g -> empty_fields--;

  gamma_debug(g);

  return true;
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
  return g -> player_fields[player - 1];
}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
  if (g -> player_areas[player - 1] < g -> max_areas)
    return g -> empty_fields;
  else
    return g -> player_adjacent[player - 1];
}

bool gamma_golden_possible(gamma_t *g, uint32_t player) {
  return (! (g -> player_golden_used[player - 1])) &&
    ((g -> width * g -> height) - (g -> empty_fields) -
    (g -> player_fields[player - 1]) > 0);
}

/*char* gamma_board(gamma_t *g) {
  uint32_t i, j;
  char *board = malloc ((g -> width + 1) * g -> height + 1);
  if (board == NULL) return NULL;
  for (i = 0; i < g -> width; i++) {
    for (j = 0; j < g -> height; j++) {
      board[(g -> width + 1) * j + i] = (char) '0' - g -> board[find(g, g -> board[get_field(g, i, j)])];
    }
    board[(g -> width + 1) * j + g -> width] = '\n';
  }
  board[(g -> width + 1) * g-> height] = '\0';
  return board;
}*/
