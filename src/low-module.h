/** @file
 * Udostępnia funkcje aktualizujące tablice preorder i num_of_bridges stanu gry gamma
 */

#ifndef LOW_MODULE_H
#define LOW_MODULE_H

#include <stdlib.h>
#include "basic_manipulations.h"

/** Czyści i od początku oblicza tablice preorder i num_of_bridges gry @p g
* @param[in,out] g  – poprawny wskaźnik na strukturę przechowującą stan gry.
*/
extern void actualise_low_all(gamma_t *g);

/** Czyści i od początku oblicza tablice preorder i num_of_bridges tego obszaru
* gry @p g do którego należy pole (@p x, @p y)
* Aktualizuje tablicę wyników funkcji low dla obszaru do którego należy pole (@p x, @p y)
* @param[in,out] g   – poprawny wskaźnik na strukturę przechowującą stan gry,
* @param[in] x       – poprawny numer kolumny,
* @param[in] y       – poprawny numer wiersza.
*/
extern void actualise_low_after_move(gamma_t *g, uint32_t x, uint32_t y);

#endif
