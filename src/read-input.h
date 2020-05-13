/** @file
 * Udostępnia funkcje obsługujące konwertowanie ciągu znaków na wejściu w
 * linie zrozumiałe dla programu.
 */

#ifndef READ_INPUT_H
#define READ_INPUT_H

#include <stdint.h>
#include <stdbool.h>

/* Maksymalna liczba parametrów które mogą pojawić się po komendzie w linii wejścia  */
#define MAX_NUMBER_OF_PARAMETERS 4

/** Typ mozliwych sygnalow wyslanych przez procedure wczytujaca linie. */
enum Flag {IGN, ERR, OK, END, ERR_END};

/**
 * Struktura przechowująca zawartość linii wczytanej z wejścia
 */
typedef struct line {
  enum Flag flag;  /**< Flaga wyrażająca semantyczną poprawność linii.
                        Pozostałe parametry posiadają znaczenie o ile flaga to OK */
  char command;    ///< pierwszy znak linii, komenda dla silnika
  uint64_t param[MAX_NUMBER_OF_PARAMETERS]; ///< tablica parametrów
  uint16_t number_of_params; ///< liczba parametrów poodzielanych białymi znakami
  uint32_t line_number; ///< numer linii
} Line;

/** @brief Inicjuje moduł.
 * Alokuje pamięć potrzebną modułowi do działania.
 * @return Zwraca true jeśli alokacja sie powiodla i false wpp.
 */
extern bool init_read_input(void);


/** @brief Czyta linię z wejścia
 * Wczytuję następną linię znaków ze standardowego wejścia i wstępnie sprawdza jej popraność
 * Nie należy zwalniać pamięci pod adresem który zwraca dopóki pozostają na wejściu
 * niewczytane dane. Za zwolnienie pamięci odpowiada zewnętrzny moduł.
 * @return Zwraca adres struktury przechowującej informacje o wczytanej linii.
 */
extern Line* get_line(void);

#endif
