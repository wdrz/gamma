#include <stdbool.h>
#include <stdint.h>
#include "basic_manipulations.h"
#include "batch-mode.h"
#include "read-input.h"
#include "gamma.h"

static inline void bool_print(bool b) {
  printf("%d\n", b);
}

static inline bool line_matches(Line *line, char command, uint16_t num_of_params) {
    return line->command == command && line->number_of_params == num_of_params;
}

/* Interpretuje linię której flaga to OK */
static void interpret_ok(gamma_t *g, Line *line) {
  char *board;
  if (line_matches(line, 'm', 3)) {
    bool_print(gamma_move(g, line->param[0], line->param[1], line->param[2]));

  } else if (line_matches(line, 'g', 3)) {
    bool_print(gamma_golden_move(g, line->param[0], line->param[1], line->param[2]));

  } else if (line_matches(line, 'b', 1)) {
    printf("%lu\n", gamma_busy_fields(g, line->param[0]));

  } else if (line_matches(line, 'f', 1)) {
    printf("%lu\n", gamma_free_fields(g, line->param[0]));

  } else if (line_matches(line, 'q', 1)) {
    bool_print(gamma_golden_possible(g, line->param[0]));

  } else if (line_matches(line, 'p', 0)) {
    board = gamma_board(g);
    if (board != NULL) {
      printf("%s", board);
      free(board);
    } else {
      print_error(line);
    }

  } else {
    print_error(line);
  }
}


void run_batch_mode(gamma_t *g) {
  Line* line;
  do {
    line = get_line();
    switch (line->flag) {
      case OK:
        interpret_ok(g, line);
        break;
      case ERR:
      case ERR_END:
        print_error(line);
        break;
      default:
        break;
    }
  } while (line->flag != END && line->flag != ERR_END);
}
