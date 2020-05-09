#ifndef READ_INPUT_H
#define READ_INPUT_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_NUMBER_OF_PARAMETERS 4

/* Typ mozliwych sygnalow wyslanych przez procedure wczytujaca linie. */
enum Flag {IGN, ERR, OK, END, ERR_END};

typedef struct line {
  enum Flag flag;
  char command;
  uint64_t param[MAX_NUMBER_OF_PARAMETERS];
  uint16_t number_of_params;
  uint32_t line_number;
} Line;

extern bool init_read_input();

extern Line* get_line();

extern void display_line(Line* line);

// extern void print_if_err();

#endif
