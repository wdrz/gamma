#include "gamma.h"
#include "interactive-mode.h"
#include "read-input.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inline bool starts_a_mode(Line *line) {
  return line->flag == OK && line->number_of_parameters == 4 &&
     (line->command == 'I' || line->command == 'B');
}

// zwraca false jesli nie udalo sie rozpoczac jakiegos trybu i true jesli udalo się
bool try_to_start_mode(Line *line) {
  if (line->flag == IGN || line->flag == END)
    return false;

  if (!starts_a_mode(line)) {
    printf("ERROR %u\n", line->line_number);
    return false;
  }

  gamma_t *g;
  g = gamma_new(line->parameters[0], line->parameters[1],
    line->parameters[2], line->parameters[3]);

  if (g == NULL) {
    printf("ERROR %u\n", line->line_number);
    return false;
  }
  if (line->command == 'I') {
    run_interactive_mode(g);
  } else {
    printf("RUN BATCH MODE\n");
  }
  gamma_delete(g);
  return true;
}

int main() {
  Line* line;

  if (!init_read_input()) {
    // blad alokacja sie nie powiodla.
  }

  do {
    line = get_line();
    // display_line(line);

    if (try_to_start_mode(line))
      break;

  } while (line->flag != END && line->flag != ERR_END);

  free(line);
  return 0;
}
