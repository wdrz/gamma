# include <stdio.h>
# include <stdbool.h>
# include <stdint.h>
# include "gamma.h"
# include <stdlib.h>

int X[4] = {1, -1, 0, 0};
int Y[4] = {0, 0, 1, -1};

struct gamma {
  uint64_t *dsu;
  uint32_t *board;
  uint32_t *player_fields, *player_areas, *player_adjacent;
  uint32_t players, width, height, max_areas;
  uint64_t empty_fields;
  uint64_t legthOfString;
  bool *player_golden_used;
};

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


bool is_addr_correct (gamma_t *g, uint32_t x, uint32_t y) {
  return x < (g -> width) && y < (g -> height);
}

/* Zwraca adres w pamieci pola zadanego przez wspolrzedne.*/
uint64_t get_position (gamma_t *g, uint32_t x, uint32_t y) {
  return (g -> height * y) + x;
}

/* Zwraca reprezentatnta pola. */
uint64_t find (gamma_t *g, uint64_t position) {
  uint64_t* zawartosc = g -> dsu + position;
  if (*zawartosc == position) {
    return position;
  } else {
    *zawartosc = find(g, *zawartosc);
    return *zawartosc;
  }
}

/* Jesli pole (x, y) nie sasiaduje z polem gracza zwraca 0.
   W przeciwnym przypadku zwraca reprezentanta jednego z nich */
bool is_adjacent (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  uint32_t i;
  for (i = 0; i < 4; i++)
    if (is_addr_correct(g, x + X[i], y + Y[i]))
      if ((g -> board)[ get_position (g, x + X[i], y + Y[i]) ] == player)
        return true;

  return false;
}

/* 1) Zwraca liczbę różnych obszarów
   2) Łączy różne obszary w jeden
   3) Dołącza pole [x, y] do tego obszaru (jeśli brak obszarów to tworzy nowy)
*/
uint32_t areas_adj (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  uint64_t prev_areas[4];
  uint16_t i, j = 0, k;
  bool new_area;
  uint64_t curr_area, curr_pos;
  for (i = 0; i < 4; i++) {
    if (is_addr_correct(g, x + X[i], y + Y[i])) {
      curr_pos = get_position (g, x + X[i], y + Y[i]);
      if (g -> board[curr_pos] == player) {
        new_area = true;
        curr_area = find(g, curr_pos);
        for (k = 0; k < j; k++) {
          if (prev_areas[k] == curr_area) {
            new_area = false;
            break;
          }
        }
        if (new_area) {
          prev_areas[j] = curr_area;
          if (j > 0) {
            g -> dsu[curr_area] = prev_areas[0];
          }
          j++;
        }
      }
    }
  }
  if (j > 0)
    g -> dsu[ get_position(g, x, y) ] = prev_areas[0];
  else
    g -> dsu[ get_position(g, x, y) ] = get_position(g, x, y);

  return j;
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

// zakłada, że pole [x, y] jest puste.
void update_adjacency (gamma_t *g,  uint32_t player, uint32_t x, uint32_t y, bool is_adj) {
  uint16_t i;
  if (is_adj)
    g -> player_adjacent[player - 1] --;

  for (i = 0; i < 4; i++) {
    if (! is_addr_correct(g, x + X[i], y + Y[i])) continue;
    if (g -> board[get_position(g, x + X[i], y + Y[i])] == 0) {
      if (! is_adjacent (g, player, x + X[i], y + Y[i]))
        g -> player_adjacent[player - 1] ++;
    } else if (g -> board[get_position(g, x + X[i], y + Y[i])] != player) {
        //
    }
  }
}

bool gamma_move (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  uint64_t pos;
  // sprawdzenie danych wejsciowych
  // Ruch nie moze zwiekszyc liczby obszarow gracza powyzej [max_areas]
  if (g == NULL || player == 0 || player >= (g -> players))
    return false;
  if (! is_addr_correct(g, x, y))
    return false;
  pos = get_position(g, x, y);
  if (g -> board[pos] != 0)
    return false;
  bool is_adj = is_adjacent(g, player, x, y);

  if ((g -> player_areas[player - 1]) == (g -> max_areas) && ! is_adj)
    return false;

  uint32_t areas_connected = areas_adj(g, player, x, y); // ile roznych obszrow sasiaduje z (x, y)
  printf("Areas connected: %d\n", areas_connected);

  // Postawinie pionka byc moze zmienilo troszke liczbe pol na ktorych gracz
  // moze sie ruszyc bez zwiekszania liczby swoich obszarow.

  update_adjacency(g, player, x, y, is_adj);

  // Miejsca na ktorych inni gracze moga postawic pionka bez zwiekszania liczby
  // swoich obszarow tez wlasnie sie zmienila. Uwzglednijmy to w wyliczniach!
  /*for (i = 0; i < 4; i++)
    if (g -> board[hood[i]] != 0 && g -> board[hood[i]] != player)
      g -> player_adjacent[ g -> board[hood[i]] ]--;*/

  // Zmienila sie liczba obszarow gracza? Liczba pol na pewno. Tak samo liczba
  // pustych pol na plaszy. Nadpiszmy tez w koncu pole o ktore sie rozchodzi
  //if (adj == 0) {
    //g -> player_areas[player - 1]++;
    //g -> board[pos] = player;
  /*} else {
    g -> board[pos] = adj;
  }*/

  g -> board[ pos ] = player;
  g -> player_areas[player - 1] -= areas_connected - 1;
  g -> player_fields[player - 1]++;
  g -> empty_fields--;
  gamma_debug(g);

  return true;
}

// implementacja kolejki


// implementacja BFSa


/*bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  // TODO: Sprawdzic poprawnosc danych

  // .......

  // Sprawdzamy, czy pole nalezy do innego gracza
  uint64_t pos = get_field(g, x, y);
  if (g -> board[find(g, pos)] == player || g -> board[find(g, pos)] == 0)
    return false;
  // Ok, nalezy. Teraz upewnijmy sie ze obszar gracza nie zostanie
  // nielegalnie rozspojniony. Policzmy z iloma polami gracza to pole graniczy.

  // .......

  // Ok. pole da sie usunac. Ale to zmienia troszke liczbe pol na ktorych
  // poszczegolni gracze moga postawic swoje pionki.

  // .......


  return false;
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
}*/

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
