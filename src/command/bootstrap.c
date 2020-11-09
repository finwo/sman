#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "commands.h"



char *defaultRepository = "https://github.com/finwo/sman-repository/archive/master.tar.gz";

int commandBootstrap(int argc, const char **argv, struct configuration *config) {

  char *tmp = malloc(strlen(config->destdir) + strlen("/etc/sman/config") + 1);
  sprintf(tmp, "%s/etc/sman/config", config->destdir);

  spawn("mkdir", (char*[]){
      "mkdir",
      "-p",
      config->destdir,
      config->repodir,
      NULL,
  });

  spawn("touch", (char*[]){
      "touch",
      config->worldfile,
      config->installedfile,
      NULL,
  });

  // Start writing config
  FILE *fd = fopen(tmp, "w+");
  if (!fd) {
    fprintf(stderr, "Could not open '%s'", tmp);
    return 1;
  }

  fwrite("[main]\n", 1, 7, fd);

  fwrite("destdir = ", 1, 10, fd);
  fwrite(config->destdir, 1, strlen(config->destdir), fd);
  fwrite("\n", 1, 1, fd);

  fwrite("repodir = ", 1, 10, fd);
  fwrite(config->repodir + strlen(config->destdir), 1, strlen(config->repodir) - strlen(config->destdir), fd);
  fwrite("\n", 1, 1, fd);

  fwrite("worldfile = ", 1, 12, fd);
  fwrite(config->worldfile + strlen(config->destdir), 1, strlen(config->worldfile) - strlen(config->destdir), fd);
  fwrite("\n", 1, 1, fd);

  fwrite("installedfile = ", 1, 16, fd);
  fwrite(config->installedfile + strlen(config->destdir), 1, strlen(config->installedfile) - strlen(config->destdir), fd);
  fwrite("\n", 1, 1, fd);

  fwrite("\n", 1, 1, fd);

  fwrite("[repository]\n", 1, 13, fd);
  fwrite("default = ", 1, 10, fd);
  fwrite(defaultRepository, 1, strlen(defaultRepository), fd);
  fwrite("\n", 1, 1, fd);

  // TODO: install extras, like libc/cc?

  fclose(fd);
  return 0;
}
