#include "gamma.h"
#include "interactive-mode.h"
#include "batch-mode.h"
#include "read-input.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline bool starts_a_mode(Line *line) {
  return line->flag == OK && line->number_of_params == 4 &&
     (line->command == 'I' || line->command == 'B');
}

// zwraca false jesli nie udalo sie rozpoczac jakiegos trybu i true jesli udalo się
bool try_to_start_mode(Line *line) {
  if (line->flag == IGN || line->flag == END)
    return false;

  if (!starts_a_mode(line)) {
    print_error(line);
    return false;
  }

  gamma_t *g;
  g = gamma_new(line->param[0], line->param[1], line->param[2], line->param[3]);

  if (g == NULL) {
    print_error(line);
    return false;
  }
  if (line->command == 'I') {
    run_interactive_mode(g);
  } else {
    printf("OK %d\n", line->line_number);
    run_batch_mode(g);
  }
  gamma_delete(g);
  return true;
}

int main() {
  Line* line;

  if (!init_read_input()) {
    return 1;
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
