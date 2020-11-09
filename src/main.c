#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argparse.h"
#include "ini.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

/* #ifdef __cplusplus */
/* extern "C" { */
/* #endif */

struct repository {
  void *next;
  const char *name;
  const char *location;
};

struct configuration {
  const char *worldfile;
  const char *repodir;
  struct repository *repo;
};

// Which configs to load
const char *configs[] = {
  "/etc/sman/config",
  "/etc/smanrc",
  "~/.smanrc",
  ".smanrc",
  NULL,
};

// Baisc usage
const char *usage[] = {
  "sman [options] [[--] args]",
  NULL,
};

// Handle config entries
static int configHandler(void *user, const char *section, const char *name, const char *value) {
  struct repository    *repo;
  struct configuration *config = user;

  // Change worldfile
  if (strcmp(section, "main") == 0) {
    if (strcmp(name, "worldfile") == 0) {
      config->worldfile = strdup(value);
    }
  }

  // Add repositories
  if (strcmp(section, "repository") == 0) {
    repo           = malloc(sizeof(struct repository));
    repo->next     = config->repo;
    repo->name     = strdup(name);
    repo->location = strdup(value);
    config->repo   = repo;
    return 0;
  }

  return 1;
}

int main(int argc, const char **argv) {
  int i;
  struct configuration config;
  config.repo      = NULL;
  config.worldfile = "/etc/sman/world";
  config.repodir   = "/etc/sman/repo";

  // Define arguments
  char *configfile = NULL;
  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('c', "config", &configfile, "Select a specific config file"),
    OPT_END(),
  };

  // Parse arguments
  struct argparse argparse;
  argparse_init(&argparse, options, usage, 0);
  argc = argparse_parse(&argparse, argc, argv);

  // Load configuration file(s)
  if (configfile) {
    if (ini_parse(configfile, configHandler, &config) < 0) {
      fprintf(stderr, "Could not read config file: %s\n", configfile);
      return 1;
    }
  } else {
    for(i=0; configs[i]; i++) {
      if (ini_parse(configs[i], configHandler, &config) < 0) {
        // Intentionally blank
      }
    }
  }

  return 0;
}

/* #ifdef __cplusplus */
/* } // extern "C" */
/* #endif */
