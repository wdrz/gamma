#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

#include "read-input.h"

Line *CURRENT_LINE;

// ignoruje resztę linii, ale patrzy czy nie natrafi na EOF. W takim wypadku zwraca true.
// wpp. zwraca false.
bool ignore_rest_of_line() {
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

// static ???
void ignore_rest_on_error() {
  if (ignore_rest_of_line()) CURRENT_LINE->flag = ERR_END;
  else CURRENT_LINE->flag = ERR;
}

// zwraca true jesli nalezy kontunuowac wczytywanie linii, false jesli wczytano
// znak konca linii lub wejscia
bool look_at_first_char() {
  char first_char = getchar();
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

// zakłada, że flaga CURRENT_LINE to OK
// zwraca true jesli nalezy kontunuowac wczytywanie linii, false jesli wczytano
// znak konca linii lub wejscia
bool look_at_second_char() {
  char second_char = getchar();
  if (second_char == EOF) {
    CURRENT_LINE->flag = ERR_END;
    return false;
  } else if (second_char == '\n') {
    CURRENT_LINE->number_of_parameters = 0;
    return false;
  } else if (isspace(second_char)) {
    return true;
  } else {
    ignore_rest_on_error();
    return false;
  }
}

// moze inline ???
// zwraca true jesli operacja sie powiodla, false jesli nie
// pozwalamy na zera wiodące
bool add_digit_to_number(uint32_t *number, char digit) {
  uint32_t temp_number;
  if (digit > '9' || digit < '0')
    return false;

  // patrzymy czy dodanie cyfry nie obróci inta - czy liczba w zakresie
  temp_number = *number * 10 + (digit - '0');
  if (temp_number < *number)
    return false;

  *number = temp_number;
  return true;
}

void look_at_the_rest () {
  // if (ignore_rest_of_line()) CURRENT_LINE->flag = ERR_END;

  uint16_t which_number = -1;
  uint32_t currently_considered_number = 0;
  bool new_block_started = false;

  char character_on_input = getchar();
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
      CURRENT_LINE->parameters[which_number] = currently_considered_number;
    }

    character_on_input = getchar();
  }

  if (character_on_input == EOF) {
    CURRENT_LINE->flag = ERR_END;
  }

  CURRENT_LINE->number_of_parameters = which_number + 1;

  // err jesli nie prawda ze juz same liczby i spacje       ok (chyba)
  // err jesli konczy sie EOFem zamiast \n-em               ok (chyba)
  // err jesli za duzo liczb na wejsciu                     ok (chyba)
  // err jesli za dlugie liczby                             !!!!
  // err jesli stringi cyfr sie nie konweruja do uint64_t   !!!!
}

// zwraca true jesli alokacja sie powiodla
// i false wpp.
bool init_read_input() {
  CURRENT_LINE = malloc(sizeof(struct line));
  return CURRENT_LINE != NULL;
}

// alkouje pamięć na linie,
// w pierwszym wywolaniu alokuje pamiec do lini ktora zwraca.
// potem juz alokuje w tym samym miejscu, wiec nie nalezy tego za szybko zwalniac!
// za zwolnienie pamieci odpowiada zewnetrzny moduł.
Line* get_line() {
  static uint32_t line_number = 0;

  line_number++;

  CURRENT_LINE->line_number = line_number;

  if (look_at_first_char() && look_at_second_char()) {
    look_at_the_rest ();
  }

  return CURRENT_LINE;
}
/*
void print_if_err(Line* line) {
  if (line->flag == ERR || line->flag == ERR_END)
    printf("ERROR %u\n", line->line_number);
}
*/

void display_line(Line* line) {
  uint16_t i;

  if (line->flag == END || line->flag == ERR_END) printf("\n");

  printf("line %u - ", line->line_number);

  if (line->flag == OK) printf("OK: ");
  else if (line->flag == ERR) printf("ERR");
  else if (line->flag == IGN) printf("IGN");
  else if (line->flag == END) printf("END");
  else if (line->flag == ERR_END) printf("ERR_END");
  else printf("???");

  if (line->flag == OK) {
    printf("command: %c, ", line->command);
    printf("num of params: %u,  params: [", line->number_of_parameters);
    for (i = 0;i < line->number_of_parameters; i++){
        printf("%lu, ", line->parameters[i]);
    }
    if (line->number_of_parameters == 0) printf("]");
    else printf("\b\b]");
  }

  printf("\n");
}
