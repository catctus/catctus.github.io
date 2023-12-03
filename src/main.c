#include "cmarkdown.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void printHelp();

int fConvertMarkupToHml(char *sourcefile, char *outputfile);

int main(int argc, char *argv[]) {

  // cli

  // take care of arguments
  if (argc < 2) {
    printf("no arguments");
    return EXIT_FAILURE;
  }
  /* prints help */
  if (strcmp(argv[1], "-h") == 0)
    printHelp();
  /* convert markup to html */
  else if (strcmp(argv[1], "-c") == 0) {
    if (argc > 4) {
      printf("not enough arguments.");
      return EXIT_FAILURE;
    }
    return fConvertMarkupToHml(argv[2], argv[3]);
  } else if (strcmp(argv[1], "-u") == 0) {
    printf("Update the blog");
  }
  return EXIT_SUCCESS;
}

int fConvertMarkupToHml(char *sourceFilePath, char *outFilePath) {
  // read from file
  FILE *fp;
  fp = fopen(sourceFilePath, "r");

  if (!fp)
    return EXIT_FAILURE;

  // get file size
  fseek(fp, 0, SEEK_END);
  int fsize = ftell(fp);
  rewind(fp);

  // create a document and dump filedata
  char *doc = NULL;
  doc = (char *)malloc((fsize + 1) * sizeof(char));

  fread(doc, fsize, sizeof(char), fp);
  doc[fsize] = '\0';
  fclose(fp);

  convertMarkdownToHtml(doc, fsize, outFilePath);

  // free mem
  free(doc);
  doc = NULL;

  return EXIT_SUCCESS;
}
void printHelp() {
  printf("------------------- BLOGGEN --------------------\n"
         "-h : help\n"
         "-c : convert mark up file to html\n"
         "     first argument is the source, second is the output\n"
         "     example: bloggen -c post1.md post1.html\n"
         "-u : update site take the posts generated and\n"
         "     put them in the sites context.\n"
         "     argument after -u should be a folder\n"
         "-------------------------------------------------\n");
}
