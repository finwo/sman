#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "commands.h"

#include "ini.h"

struct symbol *provided = NULL;
struct symbol *depends  = NULL;

static int providesHandler(void *udata, const char *section, const char *name, const char *value) {
  if (strcmp(section, "package")) return 1;
  if (strcmp(name, "provides")) return 1;
  struct symbol *s = malloc(sizeof(struct symbol));
  s->next = provided;
  s->value = strdup(value);
  provided = s;
  return 0;
}

static int dependsHandler(void *udata, const char *section, const char *name, const char *value) {
  if (strcmp(section, "package")) return 1;
  if (strcmp(name, "depends")) return 1;
  struct symbol *s = malloc(sizeof(struct symbol));
  s->next = depends;
  s->value = strdup(value);
  depends = s;
  return 0;
}

int haveSymbol(const char *value) {
  struct symbol *sym = provided;
  while(sym) {
    if (!strcmp(sym->value, value)) return 1;
    sym = sym->next;
  }
  return 0;
}

int commandInstall(int argc, const char **argv, struct configuration *config) {
  FILE *wfd = fopen(config->worldfile, "r");
  FILE *ifd = fopen(config->installedfile, "r");
  char *line = malloc(4096);
  char *tmp  = malloc(8192);
  struct symbol *sym;

  // Fetch all dependencies
  sprintf(tmp, "%s/%s/config", config->repodir, argv[0]);
  if(ini_parse(tmp, dependsHandler, NULL) < 0) {
    fprintf(stderr, "Could not read package file: %s\n", tmp);
    return 1;
  }

  // Fetch all provided symbols
  for(;;) {
    fgets(line, 4096, ifd);
    if (feof(ifd)) break;
    if (strlen(line)<=1) continue;
    *(line + strlen(line) - 1) = 0;
    sprintf(tmp, "%s/%s/config", config->repodir, line);
    ini_parse(tmp, providesHandler, NULL);
  }

  // Check if all required symbols are there
  sym = depends;
  while(sym) {
    printf("Depends on: %s\n", sym->value);
    sym = sym->next;
  }

  sym = provided;
  while(sym) {
    printf("I haz: %s\n", sym->value);
    sym = sym->next;
  }

  /* if (!fd) { */
    /* fprintf(stderr, "Could not open '%s'\n", config->worldfile); */
    /* return 1; */
  /* } */



  /* printf("Install %d\n", (int)fd); */
  /* printf("Argc: %d\n", argc); */
  /* for(int i=0; i<argc; i++) { */
    /* printf("argument: %s\n", argv[i]); */
  /* } */

  /* fclose(fd); */
  fclose(wfd);
  fclose(ifd);
  return 0;
}
