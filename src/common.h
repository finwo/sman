#ifndef _SMAN_COMMON_H_
#define _SMAN_COMMON_H_

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef MIN
#define MIN(a,b) ((a<b)?a:b)
#endif

#ifndef MAX
#define MAX(a,b) ((a>b)?a:b)
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

struct repository {
  void *next;
  const char *name;
  const char *location;
};

struct configuration {
  char *destdir;
  char *installedfile;
  char *worldfile;
  char *repodir;
  struct repository *repo;
};

struct symbol {
  void *next;
  const char *value;
};

int spawn(const char *command, char *argv[]);

#endif // _SMAN_COMMON_H_
