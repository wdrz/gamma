#include "gamma.h"
#include "interactive-mode.h"
#include "batch-mode.h"
#include "read-input.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

static bool ERROR = false;

static inline bool starts_a_mode(Line *line) {
  return line->flag == OK && line->number_of_params == 4 &&
     (line->command == 'I' || line->command == 'B');
}

static inline bool console_large_enough(gamma_t *g) {
  struct winsize w;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) return false;
  return w.ws_row > g->width && w.ws_col >= 15 && w.ws_col >= g->height;
}

/* Zwraca false jeśli nie udało się rozpocząć żadnego trybu i true jeśli udało się. */
static bool try_to_start_mode(Line *line) {
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
    if (!console_large_enough(g) || !run_interactive_mode(g)) {
      print_error(line);
      ERROR = true;
      return false;
    }

  } else {
    printf("OK %d\n", line->line_number);
    run_batch_mode(g);
  }
  gamma_delete(g);
  return true;
}

int main(void) {
  Line* line;

  if (!init_read_input())
    exit(1);

  do {
    line = get_line();

    if (try_to_start_mode(line))
      break;

  } while (line->flag != END && line->flag != ERR_END);

  free(line);

  if (ERROR) exit(1);
  else return 0;
}
