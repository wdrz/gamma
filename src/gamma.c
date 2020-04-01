# include <stdio.h>
# include <stdbool.h>
# include <stdint.h>
# include "gamma.h"
# include <stdlib.h>

int X[4] = {1, -1, 0, 0};
int Y[4] = {0, 0, 1, -1};

struct gamma {
  uint64_t *board;
  uint32_t *player_fields, *player_areas, *player_adjacent;
  uint32_t players, width, height, max_areas;
  uint64_t empty_fields;
  bool *player_golden_used;
};

gamma_t* gamma_new(uint32_t width, uint32_t height,
  uint32_t players, uint32_t areas) {
    // TODO: powinien sie wywalac jak niepoprawne argumenty
    gamma_t *g = malloc (sizeof(struct gamma));
    if (g == NULL) exit (1);
    g -> players = players;
    g -> width = width;
    g -> height = height;
    g -> max_areas = areas;
    g -> empty_fields = width * height;
    g -> board = calloc (width * height, sizeof (uint32_t) + 1);
    g -> player_fields = calloc (players, sizeof (uint32_t));
    g -> player_areas = calloc (players, sizeof (uint32_t));
    g -> player_adjacent = calloc (players, sizeof (uint32_t));
    g -> player_golden_used = calloc (players, sizeof (bool));
    if (g -> board == NULL || g -> player_areas == NULL
    || g -> player_adjacent == NULL || g -> player_golden_used == NULL)
      exit (1);
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

/* Zwraca pozycje pola zadanego przez wspolrzedne. Pozycja jest to adres
   w pamieci pola, pomniejszony o adres poczatku tablicy z polami. */
uint64_t get_field (gamma_t *g, uint32_t x, uint32_t y) {
  if (x < g -> width && y < g -> height)
    return (g -> height * y) + x + 1;
  else
    return 0;
}

/* Zwraca pozycje reprezentatnta pola. Pozycja pola zostala zdefiniowana wyzej. */
uint64_t find (gamma_t *g, uint64_t position) {
  uint64_t* zawartosc = g -> board + position;
  if ( *zawartosc >=  (- g -> players) || *zawartosc == 0) return position;
  *zawartosc = find(g, *zawartosc);
  return *zawartosc;
}

/* Laczy dwa obszary o ile naleza one do tego samego gracza.
   Oczekuje, ze podane zostana pozycje reprezentantow dwoch obszarow.
   Moga byc to te same obszary */
void union_g (gamma_t *g, uint64_t pos_rep1, uint64_t pos_rep2) {
  if (pos_rep1 == 0 || pos_rep2 == 0) return;
  if (g -> board[pos_rep1] != g -> board[pos_rep2]) return;
  g -> board[pos_rep1] = pos_rep2;
}

/* Jesli pole (x, y) nie sasiaduje z polem gracza zwraca 0.
   W przeciwnym przypadku zwraca reprezentanta jednego z nich */
uint64_t is_adjacent (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  uint32_t i;
  for (i = 0; i < 4; i++)
    if (g -> board[find(g, get_field(g, x + X[i], y + Y[i]))] == player)
      return find(g, get_field(g, x + X[i], y + Y[i]));
  return false;
}

bool gamma_move (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  uint64_t pos = get_field(g, x, y);
  uint32_t i;
  // Czy dane wejsciowe sa poprawne? (TODO: sprawdzic playera, plansze)
  // Czy wskazano puste pole wewnatrz planszy?
  if (pos == 0 || g -> board[pos] != 0) return false;

  // Ruch nie moze zwiekszyc liczby obszarow gracza powyzej [max_areas]
  uint64_t adj = is_adjacent(g, player, x, y);
  if (g -> player_areas[player - 1] == g -> max_areas && adj == 0)
    return false;

  // Zbadajmy jacy sa reprezentanci sasiadow pola (x, y)
  uint64_t hood[4]; // pozycje reprezentatow pol sasiadujacych z (x, y)
  uint32_t wolne; // liczba pustych pol sasiadujacych z (x, y) ktore nie sasiaduja z polami gracza player
  for (i = 0; i < 4; i++) {
    hood[i] = find(g, get_field(g, x + X[i], y + Y[i]));
  }
  // Postawinie pionka byc moze zmienilo troszke liczbe pol na ktorych gracz
  // moze sie ruszyc bez zwiekszania liczby swoich obszarow.
  for (i = 0; i < 4; i++)
    if (! is_adjacent(g, player, x + X[i], y + Y[i]))
      g -> player_adjacent[player - 1]++;
  if (adj != 0) g -> player_adjacent[player - 1]--;

  // Byc moze nowe pole jest mostem laczacym 2, 3, a moze nawet 4 obszary gracza.
  // Sprawdzmy, czy tak jest i polaczmy je ze soba jesli trzeba!
  if (adj != 0)
    for (i = 0; i < 4; i++)
      union_g (g, adj, hood[i]);

  // Miejsca na ktorych inni gracze moga postawic pionka bez zwiekszania liczby
  // swoich obszarow tez wlasnie sie zmienila. Uwzglednijmy to w wyliczniach!
  for (i = 0; i < 4; i++)
    if (g -> board[hood[i]] != 0 && g -> board[hood[i]] != player)
      g -> player_adjacent[ g -> board[hood[i]] ]--;

  // Zmienila sie liczba obszarow gracza? Liczba pol na pewno. Tak samo liczba
  // pustych pol na plaszy. Nadpiszmy tez w koncu pole o ktore sie rozchodzi
  if (adj == 0) {
    g -> player_areas[player - 1]++;
    g -> board[pos] = player;
  } else {
    g -> board[pos] = adj;
  }
  g -> player_fields[player - 1]++;
  g -> empty_fields--;
  return true;
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
  // TODO: Sprawdzic poprawnosc danych

  // Sprawdzamy, czy pole nalezy do innego gracza
  uint64_t pos = get_field(g, x, y);
  if (find(g, pos)g -> board[])

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
}

char* gamma_board(gamma_t *g) {
  char *board = malloc ((g -> width + 1) * g -> height + 1);
  if (board == NULL) return false;
  uint32_t i, j;
  for (i = 0; i < g -> width; i++) {
    for (j = 0; j < g -> height; j++) {
      board[(g -> width + 1) * j + i] = (char) '0' - 1 + g -> board[get_field(g, i, j)];
    }
    board[(g -> width + 1) * j + g -> width] = '\n';
  }
  board[(g -> width + 1) * g-> height] = '\0';
  return board;
}
