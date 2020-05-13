/** @file
 * Udostępnia funkcję obsługującą tryb interaktywny
 */

#ifndef INTERACTIVE_MODE_H
#define INTERACTIVE_MODE_H

# include <stdlib.h>
# include "basic_manipulations.h"

/** @brief Uruchamia tryb interaktywny
* Przeprowadza rozgrywkę gry gamma w trybie interaktywnym.
* @param[in,out] g   - poprawny wskaźnik na strukturę przechowującą (początkowy) stan gry,
*/
extern void run_interactive_mode(gamma_t *g);

#endif
