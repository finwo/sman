#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "commands.h"
#include "common.h"

int commandUpdate(int argc, const char **argv, struct configuration *config) {
  char *tmpstr = malloc(8192);
  /* printf("UPDATE\n"); */
  struct repository *repo = config->repo;
  while(repo) {
    sprintf(tmpstr, "%s/%s.tar.gz", config->repodir, repo->name);

    spawn("mkdir", (char*[]){
      "mkdir",
      "-p",
      config->repodir,
      NULL,
    });

    spawn("curl", (char*[]){
      "curl",
      /* "-v", */
      "-z",
      tmpstr,
      "-#L",
      "-o",
      tmpstr,
      repo->location,
      NULL,
    });

    spawn("tar", (char*[]){
      "tar",
      "xf",
      tmpstr,
      "--overwrite",
      "--directory",
      config->repodir,
      "--strip-components=1",
      NULL,
    });

    repo = repo->next;
  }

  free(tmpstr);
  return 0;
}
