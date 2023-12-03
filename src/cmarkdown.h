#ifndef CMARKDOWN_H_
#define CMARKDOWN_H_

/*doc is the markup that needs to be converted
 *fsize is the size of doc
 *outpath is where we want to save the html
 */
int convertMarkdownToHtml(char *doc, int fsize, char *outpath);

#endif // !CMARKDOWN_H_
