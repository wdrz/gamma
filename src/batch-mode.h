/** @file
 * Udostępnia funkcję obsługującą tryb wsadowy
 */

#ifndef BATCH_MODE_H
#define BATCH_MODE_H

#include <stdio.h>
#include <stdlib.h>
#include "basic_manipulations.h"
#include "read-input.h"

/** @brief Wypisuje błąd.
 * Wypisuje "ERROR" oraz numer linii @p line.
 * @param[in,out] line   – wskaźnik na linię z wejścia, której numer wypisuje po
 *                         informacji o błędzie.
 */
static inline void print_error(Line *line) {
  fprintf(stderr, "ERROR %u\n", line->line_number);
}

/** @brief Uruchamia tryb wsadowy.
 * Przeprowadza rozgrywkę gry gamma w trybie wsadowym.
 * @param[in,out] g   - poprawny wskaźnik na strukturę przechowującą (początkowy) stan gry.
 */
extern void run_batch_mode(gamma_t *g);

#endif
