#ifndef BATCH_MODE_H
#define BATCH_MODE_H

#include <stdio.h>
#include <stdlib.h>
#include "basic_manipulations.h"
#include "read-input.h"

inline void print_error(Line *line) {
  fprintf(stderr, "ERROR %u\n", line->line_number);
}

extern void run_batch_mode(gamma_t *g);

#endif
