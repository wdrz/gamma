#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include "read-input.h"

/* maksymalna wartość uint32_t */
#define MAX32 4294967295

static Line *CURRENT_LINE;

/* Ignoruje resztę linii. Dodatkowo, jeśli linia zawiera EOF zwraca true. Wpp zwraca false */
static bool ignore_rest_of_line(void) {
  char character_on_input = getchar();
  while (character_on_input != '\n' && character_on_input != EOF) {
    character_on_input = getchar();
  }
  if (character_on_input == EOF) {
    return true;
  } else {
    return false;
  }
}

/* Ignoruje resztę linii. Jeśli linia zawiera EOF ustawia flagę obecnej linii na EER_END.
 * Wpp ustawia flagę obecnej linii na ERR */
static inline void ignore_rest_on_error(void) {
  if (ignore_rest_of_line()) CURRENT_LINE->flag = ERR_END;
  else CURRENT_LINE->flag = ERR;
}

/* Zwraca true jeśli po wczytaniu pierwszego znaku należy kontynuować
 * wczytywanie reszty linii, false wpp. Dodatkowo ustawia flagę linii
 * jeśli da się ją wywnisokować po pierwszym jej znaku. */
static bool look_at_first_char(void) {
  int first_char = getchar();
  switch (first_char) {
    case EOF:
      CURRENT_LINE->flag = END;
      return false;
    case '\n':
      CURRENT_LINE->flag = IGN;
      return false;
    case '#':
      CURRENT_LINE->flag = IGN;
      if (ignore_rest_of_line()) CURRENT_LINE->flag = END;
      else CURRENT_LINE->flag = IGN;
      return false;
    default:
      if (isspace(first_char)) {
        ignore_rest_on_error();
        return false;
      } else {
        CURRENT_LINE->flag = OK;
        CURRENT_LINE->command = first_char;
      }
  }
  return true;
}

/* Zwraca true jeśli po wczytaniu drugiego znaku należy kontynuować
 * wczytywanie reszty linii, false wpp. Dodatkowo ustawia flagę linii
 * jeśli da się ją wywnisokować po drugim jej znaku. Zakłada, że po wczytaniu
 * pierwszego znaku flaga linii jest ustawiona na OK. */
static bool look_at_second_char(void) {
  int second_char = getchar();
  if (second_char == EOF) {
    CURRENT_LINE->flag = ERR_END;
    return false;
  } else if (second_char == '\n') {
    CURRENT_LINE->number_of_params = 0;
    return false;
  } else if (isspace(second_char)) {
    return true;
  } else {
    ignore_rest_on_error();
    return false;
  }
}

/* Do liczby *number dopisuje cyfrę digit. Zwraca true jesli operacja sie
 * powiodla, false wpp. Pozwalamy na zera wiodące. */
static bool add_digit_to_number(uint32_t *number, char digit) {
  if (digit > '9' || digit < '0')
    return false;

  if (*number < MAX32 / 10 || (*number == MAX32 / 10 && digit - '0' <= MAX32 % 10)) {
     *number = *number * 10 + (digit - '0');
     return true;
  } else {
    return false;
  }
}

/* Spogląda na resztę (od trzeciego) znaków w linii. Zakłada, że po przejrzeniu
 * pierwszych dwóch znaków flaga linii to OK. Ustawia flagę linii na ERR jeśli
 * * nie prawda ze są same liczby i spacje
 * * kończy sie EOFem zamiast \n-em
 * * za dużo liczb na wejsciu
 * * za duże liczby
 */
static void look_at_the_rest(void) {
  uint16_t which_number = -1;
  uint32_t currently_considered_number = 0;
  bool new_block_started = false;

  int character_on_input = getchar();
  while (character_on_input != '\n' && character_on_input != EOF) {
    if (isspace(character_on_input)) {
      currently_considered_number = 0;
      new_block_started = false;
    } else {
      if (!new_block_started) {
        which_number++;
        new_block_started = true;
        if (which_number >= MAX_NUMBER_OF_PARAMETERS) {
          ignore_rest_on_error();
          return;
        }
      }
      if (!add_digit_to_number(&currently_considered_number, character_on_input)) {
        ignore_rest_on_error();
        return;
      }
      CURRENT_LINE->param[which_number] = currently_considered_number;
    }

    character_on_input = getchar();
  }

  if (character_on_input == EOF) {
    CURRENT_LINE->flag = ERR_END;
  }

  CURRENT_LINE->number_of_params = which_number + 1;
}

bool init_read_input(void) {
  CURRENT_LINE = malloc(sizeof(struct line));
  return CURRENT_LINE != NULL;
}

Line* get_line() {
  static uint32_t line_number = 0;

  line_number++;

  CURRENT_LINE->line_number = line_number;

  if (look_at_first_char() && look_at_second_char()) {
    look_at_the_rest ();
  }

  return CURRENT_LINE;
}
