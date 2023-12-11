#ifndef CMARKDOWN_H_
#define CMARKDOWN_H_

/*doc is the markup that needs to be converted
 *fsize is the size of doc
 *outpath is where we want to save the html
 */

typedef struct element {
  int typeId;
  int isAllocated;
  char *html;
} element_t;

int convertMarkdownToHtml(char *doc, int fsize, char *outpath);

void freeMap(element_t *map, int size);

int getIntOfNeigbours(char *doc, char check, int offset);
int getIntToNext(char *doc, char check, int offset);
void mapToken(element_t *map, int startFrom, int numberOfSkips, int tokenType);
int isBeginningOfLine(char *doc, int pos);

// maps
void mapAsteriks(char *doc, element_t *map, int pos);
void mapHeader(char *doc, element_t *map, int pos);
void mapCode(char *doc, element_t *map, int pos);
void mapLink(char *doc, element_t *map, int pos);
void mapImage(char *doc, element_t *map, int pos);
void mapParagraph(char *doc, element_t *map, int pos);
void mapLineSymbol(char *doc, element_t *map, int pos);

#endif // !CMARKDOWN_H_
