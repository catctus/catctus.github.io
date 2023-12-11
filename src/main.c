#include "cmarkdown.h"
#include "filelist.h"
#include <dirent.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MARKDOWN_POST_FOLDER "./markdown/"
#define HTML_GEN_POST_FOLDER "./gen_posts/"
#define HTML_TEMPLATE_HEAD "./templates/post_head_template.html"
#define HTML_TEMPLATE_END "./templates/post_end_template.html"
#define HTML_TEMPLATE_INDEX_HEAD "./templates/index_head_template.html"
#define HTML_TEMPLATE_INDEX_END "./templates/index_end_template.html"
#define OUTPUT_POST_FOLDER "../pages/posts/"
#define OUTPUT_INDEX_FILE "../index.html"

void printHelp();
int addToFileFromFile(char *readPath, FILE *outputfile);
int fConvertMarkupToHml(char *sourcefile, char *outputfile);
int updateBlog();
int updateHomePageNavigation(ListNodePtr head);

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
    return updateBlog();
  }
  return EXIT_SUCCESS;
}
int string_ends_with(const char *str, const char *suffix) {
  int str_len = strlen(str);
  int suffix_len = strlen(suffix);

  return (str_len >= suffix_len) &&
         (0 == strcmp(str + (str_len - suffix_len), suffix));
}

/* just a mega method todo it all in one go
 * could have added some checking if things was new
 * but just felt like finishing this up for now..
 * */
int updateBlog() {
  ListNodePtr head = NULL;

  DIR *d;
  struct dirent *dir;
  d = opendir(MARKDOWN_POST_FOLDER);
  if (d) {
    int size;
    while ((dir = readdir(d)) != NULL)
      if (dir->d_type == 8)
        insert(&head, dir->d_name);
    closedir(d);
  }

  ListNodePtr headCpy = head;
  ListNodePtr previousPost = NULL;

  // now when we have everything in order we can build it
  while (headCpy) {
    char *fullpath = NULL;
    fullpath = (char *)malloc(
        (strlen(headCpy->data) + strlen(MARKDOWN_POST_FOLDER) + 1) *
        sizeof(char));

    strcpy(fullpath, MARKDOWN_POST_FOLDER);
    strcat(fullpath, headCpy->data);

    char *genpostpath = NULL;
    // just adding extra mem here to be safe when Im lazy..
    genpostpath = (char *)malloc(
        (strlen(headCpy->data) + strlen(HTML_GEN_POST_FOLDER) + 9) *
        sizeof(char));

    char *outputpath = NULL;
    outputpath = (char *)malloc(
        (strlen(headCpy->data) + strlen(OUTPUT_POST_FOLDER) + 9) *
        sizeof(char));

    // generate new path
    strcpy(genpostpath, HTML_GEN_POST_FOLDER);
    strcpy(outputpath, OUTPUT_POST_FOLDER);
    if (string_ends_with(headCpy->data, ".md")) {
      strncat(genpostpath, headCpy->data, strlen(headCpy->data) - 3);
      strncat(outputpath, headCpy->data, strlen(headCpy->data) - 3);
    } else {
      strncat(genpostpath, headCpy->data, strlen(headCpy->data));
      strncat(outputpath, headCpy->data, strlen(headCpy->data));
    }
    strcat(genpostpath, ".html");
    strcat(outputpath, ".html");

    printf("convert : %s -> %s\n", fullpath, genpostpath);
    int check = fConvertMarkupToHml(fullpath, genpostpath);

    // now we need to combine this stuff
    printf("publish: %s -> %s\n", genpostpath, outputpath);
    FILE *fOutPost;
    FILE *fHeadTemplate;
    FILE *fHeadEnd;

    fOutPost = fopen(outputpath, "w+");

    if (addToFileFromFile(HTML_TEMPLATE_HEAD, fOutPost) == -1)
      return EXIT_FAILURE;

    if (addToFileFromFile(genpostpath, fOutPost) == -1)
      return EXIT_FAILURE;

    // add some nice navigation between posts
    // This code is a mess, I just wanted to finish it..
    fputs("<div class=\"container post-linker\">", fOutPost);
    // if we are the end of the posts do this
    if (previousPost != NULL && headCpy->nxtPtr == NULL) {
      char *linkHtml = NULL;
      char *start = "<a href=\"./pages/posts/";
      char *end = "\"> <i class=\"bi bi-arrow-left-square\"> Previous </i></a>";
      linkHtml = (char *)malloc(strlen(start) + strlen(end) +
                                strlen(previousPost->data) + 10);
      strcpy(linkHtml, start);
      if (string_ends_with(previousPost->data, ".md")) {
        strncat(linkHtml, previousPost->data, strlen(previousPost->data) - 3);
        strcat(linkHtml, ".html");
      } else {
        strcat(linkHtml, previousPost->data);
      }
      strcat(linkHtml, end);
      fputs(linkHtml, fOutPost);
      free(linkHtml);
      linkHtml = NULL;
    } else if (previousPost == NULL && headCpy->nxtPtr != NULL) {
      char *linkHtml = NULL;
      char *start = "<a href=\"./pages/posts/";
      char *end = "\"> Next <i class=\"bi bi-arrow-right-square\"></i></a>";
      linkHtml = (char *)malloc(strlen(start) + strlen(end) +
                                strlen(headCpy->nxtPtr->data) + 10);
      strcpy(linkHtml, start);
      if (string_ends_with(headCpy->nxtPtr->data, ".md")) {
        strncat(linkHtml, headCpy->nxtPtr->data,
                strlen(headCpy->nxtPtr->data) - 3);
        strcat(linkHtml, ".html");
      } else {
        strcat(linkHtml, headCpy->nxtPtr->data);
      }
      strcat(linkHtml, end);
      fputs(linkHtml, fOutPost);
      free(linkHtml);
      linkHtml = NULL;
    } else if (previousPost != NULL && headCpy->nxtPtr != NULL) {
      char *linkHtml = NULL;
      char *start = "<a href=\"./pages/posts/";
      char *endPrev =
          "\"><i class=\"bi bi-arrow-left-square\"> Previous</i></a>";
      char *endNext =
          "\"> & Next <i class=\"bi bi-arrow-right-square\"></i></a>";
      linkHtml = (char *)malloc(
          strlen(start) + strlen(endPrev) + strlen(start) + strlen(endNext) +
          strlen(headCpy->nxtPtr->data) + strlen(previousPost->data) + 10);

      strcpy(linkHtml, start);
      if (string_ends_with(previousPost->data, ".md")) {
        strncat(linkHtml, previousPost->data, strlen(previousPost->data) - 3);
        strcat(linkHtml, ".html");
      } else {
        strcat(linkHtml, previousPost->data);
      }
      strcat(linkHtml, endPrev);

      strcpy(linkHtml, start);

      if (string_ends_with(headCpy->nxtPtr->data, ".md")) {
        strncat(linkHtml, headCpy->nxtPtr->data,
                strlen(headCpy->nxtPtr->data) - 3);
        strcat(linkHtml, ".html");
      } else {
        strcat(linkHtml, headCpy->nxtPtr->data);
      }
      strcat(linkHtml, endNext);
      fputs(linkHtml, fOutPost);
      free(linkHtml);
      linkHtml = NULL;
    }
    fputs("</div>\n", fOutPost);
    // Finish off the linking between posts

    if (addToFileFromFile(HTML_TEMPLATE_END, fOutPost) == -1)
      return EXIT_FAILURE;

    fclose(fOutPost);

    free(genpostpath);
    free(outputpath);
    free(fullpath);

    previousPost = headCpy;
    headCpy = headCpy->nxtPtr;
  }
  printf("\nupdate index.html..\n");
  if (updateHomePageNavigation(head) == -1) {
    printf("failed updating index.html");
    return EXIT_FAILURE;
  }
  freeList(head);

  printf("\nblog has been updated!\n");
  return EXIT_SUCCESS;
}

int updateHomePageNavigation(ListNodePtr head) {
  ListNodePtr currentPtr = head;
  FILE *fIndexOut;

  fIndexOut = fopen(OUTPUT_INDEX_FILE, "w+");
  if (fIndexOut == NULL)
    return EXIT_FAILURE;

  if (addToFileFromFile(HTML_TEMPLATE_INDEX_HEAD, fIndexOut) == -1)
    return EXIT_FAILURE;

  char *linkHtml = NULL;
  char *linkImg = NULL;

  while (currentPtr != NULL) {
    fputs("\n<div class=\"col\"><a href=\"/pages/posts", fIndexOut);

    linkHtml = (char *)malloc(strlen(currentPtr->data) + 10);
    linkImg = (char *)malloc(strlen(currentPtr->data) + 10);

    // this is just to make things easier not having to add nullz
    strcpy(linkHtml, "/");
    strcpy(linkImg, "/");

    if (string_ends_with(currentPtr->data, ".md")) {
      strncat(linkHtml, currentPtr->data, strlen(currentPtr->data) - 3);
      strcat(linkHtml, ".html");

      strncat(linkImg, currentPtr->data, strlen(currentPtr->data) - 3);
      strcat(linkImg, ".jpeg");

    } else {
      strcpy(linkHtml, currentPtr->data);
      strcpy(linkImg, currentPtr->data);
      strcat(linkImg, ".jpeg");
    }
    fputs(linkHtml, fIndexOut);
    fputs("\"><img src=\"/resource", fIndexOut);
    fputs(linkImg, fIndexOut);
    fputs("\" class=\"rounded img-thumbnail\"></a></div>", fIndexOut);

    free(linkHtml);
    linkHtml = NULL;
    free(linkImg);
    linkImg = NULL;

    currentPtr = currentPtr->nxtPtr;
  }

  if (addToFileFromFile(HTML_TEMPLATE_INDEX_END, fIndexOut) == -1)
    return EXIT_FAILURE;

  fclose(fIndexOut);

  return EXIT_SUCCESS;
}
int addToFileFromFile(char *readPath, FILE *outputfile) {
  FILE *readFile = NULL;
  readFile = fopen(readPath, "r");
  if (readFile == NULL) {
    printf("failed opening %s\n", readPath);
    return EXIT_FAILURE;
  }
  int c;
  while ((c = fgetc(readFile)) != EOF) {
    fputc(c, outputfile);
  }
  fclose(readFile);
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

  // do convert magic and dump to file
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
