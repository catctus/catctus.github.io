/*Markup converter
 *
 * we have two arrays, one with the raw markup and one
 * that is filled with information about the position.
 * The position array (struct*) will be filled by looking a head
 * where elements open and close.  So when we get there in the
 * char markup array we can make decisions if we need to add html,
 * skip the char or ignore mapping new values to the position/struct array.
 *
 * Should be relativly easy to add more advance and bespoke options to the
 * mapping. everything can be done in one go.
 *
 * */

#include "cmarkdown.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

// define some standard search things
#define ASTERISKS_SYMBOL '*'
#define HEADER_SYMBOL '#'
#define BRACKET_SYMBOL_START '['
#define BRACKET_SYMBOL_END ']'
#define PARENTHESES_SYMBOL_START '('
#define PARENTHESES_SYMBOL_END ')'
#define IMAGE_SYMBOL '!'
#define CODE_SYMBOL '`'
#define LINE_SYMBOL '-'
#define END_OF_LINE '\n'

// different tokens
#define SKIP_TOKEN -1   // removes char
#define IGNORE_TOKEN -2 // ignore search
#define KEEP_TOKEN -3   // keeps char for combo

// -main data structure used
// typeid is used for skips and to fill
// a space in the map array.
// - is allocted is used for clean up
// - html is the char array that should be used
typedef struct element {
  int typeId;
  int isAllocated;
  char *html;
} element_t;

/*  some utility functions */

void freeMap(element_t *map, int size) {

  for (int i = 0; i < size; i++) {
    if (map[i].isAllocated == 1) {
      free(map[i].html);
      map[i].html = NULL;
    }
  }
  free(map);
  map = NULL;
}
/* continue the search and check if we have more or
 * some specic char on a doc + offset position
 * can be used to find number of occurences
 * for example if there is 1 or 2 '*' in a row
 * */
int getIntOfNeigbours(char *doc, char check, int offset) {
  int count = 0;
  doc += offset;
  while (*doc == check && *doc != '\0') {
    doc++;
    count++;
  }
  return count;
}

/* Search string and count until we hit the next check */
int getIntToNext(char *doc, char check, int offset) {
  int count = 0;
  // WE NEED TO DO A CHECK HERE SO WE DON't RUN OUT!
  doc += offset;
  while (*doc != check && *doc != '\0') {
    doc++;
    count++;

    if (*doc == check)
      break;
  }

  // if we get to end something must have gone wrong. abort for now
  if (*doc == '\0') {

#ifdef DEBUG
    doc -= count;
    printf("Warning Missing Closure for:\n");
    while (*doc != '\n' && *doc != '\0') {
      printf("%c", *doc);
      doc++;
    }
    printf("\n");
#endif /* ifdef WARNING */
    return -1;
  }
  // assert(*doc == '\0');

  return count;
}

/* simple function to just fill rows of the map with skip or ignore etc. */
void mapToken(element_t *map, int startFrom, int numberOfSkips, int tokenType) {
  element_t token = {tokenType, 0, "\0"};
  for (int i = 0; i < (numberOfSkips); i++) {
    map[startFrom + i] = token;
  }
}

int isBeginningOfLine(char *doc, int pos) {

  // do a check so we actually are at the beginning of a line
  if (pos > 0) {
    // move back one
    doc--;
    char check = *doc;

    if (check != END_OF_LINE)
      return 0;
  }

  return 1;
}

/*  HERE GOES ALL THE MAPINGS!  */

// this is for '*', '**' and '***' to cursive and bold and both!
void mapAsteriks(char *doc, element_t *map, int pos) {

  // do search for number of occurences of symbol, length to next and end
  int openCount = getIntOfNeigbours(doc, ASTERISKS_SYMBOL, 0);
  int length = getIntToNext(doc, ASTERISKS_SYMBOL, openCount);
  if (length == -1)
    return;

  int closeCount = getIntOfNeigbours(doc, ASTERISKS_SYMBOL, openCount + length);

  // grab the least amount so ***text** would just be 2 signs.
  int numberOfSigns = openCount < closeCount ? openCount : closeCount;

  // map skips
  if (numberOfSigns > 1) {
    mapToken(map, pos + 1, numberOfSigns - 1, SKIP_TOKEN);
    mapToken(map, pos + length + openCount + 1, numberOfSigns - 1, SKIP_TOKEN);
  }

  // switch depending on occurences '*', '**', '***'
  switch (numberOfSigns) {
  case 1: {
    element_t cursiveOpen = {1, 0, "<em>"};
    element_t cursiveClose = {1, 0, "</em>"};
    map[pos] = cursiveOpen;
    map[pos + length + openCount] = cursiveClose;
  } break;
  case 2: {
    element_t cursiveOpen = {1, 0, "<strong>"};
    element_t cursiveClose = {1, 0, "</strong>"};
    map[pos] = cursiveOpen;
    map[pos + length + openCount] = cursiveClose;
  } break;
  default: {
    element_t cursiveOpen = {1, 0, "<strong><em>"};
    element_t cursiveClose = {1, 0, "</em></strong>"};
    map[pos] = cursiveOpen;
    map[pos + length + openCount] = cursiveClose;
  } break;
  }
}

// this is from # to headers, h1 h2 h3 h4
// we could add more so it goes up to 6. but 4 for now.
// Could also allocate and just use the number..
void mapHeader(char *doc, element_t *map, int pos) {

  // check so we are at beginning of line else pass..
  if (!isBeginningOfLine(doc, pos))
    return;

  // do search for number of occurences of symbol and length to next
  int openCount = getIntOfNeigbours(doc, HEADER_SYMBOL, 0);
  int length = getIntToNext(doc, END_OF_LINE, openCount);
  if (length == -1)
    return;

  // add skips!
  mapToken(map, pos + 1, openCount - 1, SKIP_TOKEN);
  // max of ? in <h?>
  int headerMax = 4;

  // generate header html based on amount of '#', 4 would be <h4> ec..
  int headerSize = openCount > headerMax ? headerMax : openCount;
  // just do a switch so we don't have to malloxx
  switch (headerSize) {
  case 1: {
    element_t htmlOpenHeader = {1, 0, "<h1>"};
    map[pos] = htmlOpenHeader;
    element_t htmlCloseHeader = {1, 0, "</h1>"};
    map[pos + length + openCount] = htmlCloseHeader;
  } break;
  case 2: {
    element_t htmlOpenHeader = {1, 0, "<h2>"};
    map[pos] = htmlOpenHeader;
    element_t htmlCloseHeader = {1, 0, "</h2>"};
    map[pos + length + openCount] = htmlCloseHeader;
  } break;
  case 3: {
    element_t htmlOpenHeader = {1, 0, "<h3>"};
    map[pos] = htmlOpenHeader;
    element_t htmlCloseHeader = {1, 0, "</h3>"};
    map[pos + length + openCount] = htmlCloseHeader;
  } break;
  default: {
    element_t htmlOpenHeader = {1, 0, "<h4>"};
    map[pos] = htmlOpenHeader;
    element_t htmlCloseHeader = {1, 0, "</h4>"};
    map[pos + length + openCount] = htmlCloseHeader;
  } break;
  }
}

void mapCode(char *doc, element_t *map, int pos) {
  // do search for number of occurences of symbol, length to next and end
  int openCount = getIntOfNeigbours(doc, CODE_SYMBOL, 0);
  int length = getIntToNext(doc, CODE_SYMBOL, openCount);
  if (length == -1)
    return;

  int closeCount = getIntOfNeigbours(doc, CODE_SYMBOL, openCount + length);

  // grab the least amount so ***text** would just be 2 signs.
  int numberOfSigns = openCount < closeCount ? openCount : closeCount;

  // map skips
  if (numberOfSigns > 1) {
    mapToken(map, pos + 1, numberOfSigns - 1, SKIP_TOKEN);
    mapToken(map, pos + length + openCount + 1, numberOfSigns - 1, SKIP_TOKEN);
  }

  // we want to ignore and not map anything inside code
  mapToken(map, pos + 1 + numberOfSigns, length, IGNORE_TOKEN);

  element_t codeOpen = {1, 0, "<code>"};
  element_t codeClose = {1, 0, "</code>"};
  map[pos] = codeOpen;
  map[pos + length + openCount] = codeClose;
}

void mapLink(char *doc, element_t *map, int pos) {

  // find end symbol bail if it's not found
  int linkTextLength = getIntToNext(doc, BRACKET_SYMBOL_END, 1);
  if (linkTextLength == -1)
    return;

  // now lets make sure we have a link after [?]<move ptr here>(?) bail if not
  // found
  int gotURL =
      getIntOfNeigbours(doc, PARENTHESES_SYMBOL_START, linkTextLength + 2);
  if (gotURL == 0)
    return;

  // find length to end ) , bail if not found..
  int URLTextLength =
      getIntToNext(doc, PARENTHESES_SYMBOL_END, linkTextLength + 3);
  if (URLTextLength == -1)
    return;

  // move text one byte and collect linktext and url text
  doc++;
  char *linkText = strndup(doc, linkTextLength);
  doc += linkTextLength + 2;
  char *URLText = strndup(doc, URLTextLength);
  char *linkHTML = (char *)malloc(linkTextLength + URLTextLength + 20);

  // create html string
  sprintf(linkHTML, "<a href=\"%s\">%s</a>", URLText, linkText);
  free(URLText);
  free(linkText);

  // --- MAP
  // create element and set to allocate laterz..
  element_t htmlElement = {1, 1, linkHTML};
  map[pos] = htmlElement;

  // add skips!
  mapToken(map, pos + 1, linkTextLength + URLTextLength + 3, SKIP_TOKEN);
}
/* Looks for Image tags and sets up the mapping  */
void mapImage(char *doc, element_t *map, int pos) {
  // TODO: we could reuse things in link here.. and remove code!

  // check so its a '[' after '!'
  int nextIsBracket = getIntToNext(doc, BRACKET_SYMBOL_START, 0);
  if (nextIsBracket != 1 || nextIsBracket == -1)
    return;

  // find end symbol bail if it's not found
  int linkTextLength = getIntToNext(doc, BRACKET_SYMBOL_END, 2);
  if (linkTextLength == -1)
    return;

  // now lets make sure we have a link after [?]<move ptr here>(?) bail if not
  // found
  int gotURL =
      getIntOfNeigbours(doc, PARENTHESES_SYMBOL_START, linkTextLength + 3);
  if (gotURL == 0)
    return;

  // find length to end ) , bail if not found..
  int URLTextLength =
      getIntToNext(doc, PARENTHESES_SYMBOL_END, linkTextLength + 4);
  if (URLTextLength == -1)
    return;

  // move text one byte and collect linktext and url text
  doc += 2;
  char *linkText = strndup(doc, linkTextLength);
  doc += linkTextLength + 2;
  char *URLText = strndup(doc, URLTextLength);
  char *linkHTML = (char *)malloc(linkTextLength + URLTextLength + 22);

  // create html string
  sprintf(linkHTML, "<img src=\"%s\" alt=\"%s\">", URLText, linkText);
  free(URLText);
  free(linkText);

  // --- MAP
  // create element and set to allocate laterz..
  element_t htmlElement = {1, 1, linkHTML};
  map[pos] = htmlElement;

  // add skips!
  mapToken(map, pos + 1, linkTextLength + URLTextLength + 4, SKIP_TOKEN);
}

/*This will map the paragraph signs, it will look for a double
 * new line as for ending. The inbetween spaces will be set to
 * ignore, so we don't start looking for more paragraph starts
 * still needs to fix if for multiple spaces. else it will create
 * empty paragraphs..
 * */
void mapParagraph(char *doc, element_t *map, int pos) {

  // search string until we hit 2
  int total = 0;
  int foundTwo = 0;
  while (foundTwo < 2) {
    int nextCount = getIntToNext(doc, END_OF_LINE, total + 1);
    if (nextCount == -1)
      return;

    foundTwo = getIntOfNeigbours(doc, END_OF_LINE, total);
    total += (nextCount + 1);
  }

  element_t paraOpen = {1, 0, "<p>"};
  element_t paraClose = {1, 0, "</p>"};
  map[pos] = paraOpen;
  map[pos + total - 1] = paraClose;

  // need to add skips for all the inbetweens!
  doc++;
  for (int i = 0; i < (total - 2); i++) {
    if (*doc == '\n') {
      element_t ignore = {IGNORE_TOKEN, 0, "\0"};
      map[pos + i + 1] = ignore;
    }
    doc++;
  }
}

void mapLineSymbol(char *doc, element_t *map, int pos) {}

int convertMarkdownToHtml(char *doc, int fsize, char *outpath) {
  // read from file
  FILE *htmlFileOut;
  htmlFileOut = fopen(outpath, "w");

  if (!htmlFileOut)
    return EXIT_FAILURE;

  // callox it and create an array with 0's
  element_t *map = NULL;
  map = calloc(fsize + 1, sizeof(element_t));

  int pos = 0;
  int id = 1;
  // do some magic
  while (*doc != '\0') {

    // check so map space is available
    if (map[pos].typeId == 0) {

      switch (*doc) {
      case ASTERISKS_SYMBOL:
        mapAsteriks(doc, map, pos);
        break;
      case HEADER_SYMBOL:
        mapHeader(doc, map, pos);
        break;
      case BRACKET_SYMBOL_START:
        mapLink(doc, map, pos);
        break;
      case IMAGE_SYMBOL:
        mapImage(doc, map, pos);
        break;
      case CODE_SYMBOL:
        mapCode(doc, map, pos);
        break;
      case LINE_SYMBOL:
        mapLineSymbol(doc, map, pos);
        break;
      case END_OF_LINE:
        mapParagraph(doc, map, pos);
        break;
      default:
        break;
      }
    }

    // write to file
    if (map[pos].typeId == 1) {
      fprintf(htmlFileOut, "%s", map[pos].html);
      if (*doc == '\n') {
        fprintf(htmlFileOut, "\n");
      }
    }

    if (map[pos].typeId == 0 || map[pos].typeId == IGNORE_TOKEN) {
      fprintf(htmlFileOut, "%c", *doc);
    }

    pos++;
    doc++;
  }
  // back it
  doc -= fsize;

  // free memoery and close file
  freeMap(map, fsize);
  fclose(htmlFileOut);

  return EXIT_SUCCESS;
}
