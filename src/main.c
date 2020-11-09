#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argparse.h"
#include "ini.h"

#include "common.h"
#include "commands.h"

/* #ifdef __cplusplus */
/* extern "C" { */
/* #endif */

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
  "sman --help",
  "sman [options] command [args]",
  NULL,
};


// Which commands we have available
// fn(argc, argv, config)
struct {
  char *name;
  char *description;
  int (*fn)(int,const char**,struct configuration *);
} commands[] = {
  { "add"      , "Add a package to the world set and install it"       , commandAdd       },
  { "install"  , "Install a package without adding it to the world set", commandInstall   },
  { "update"   , "Fetch repository updates"                            , commandUpdate    },
  { "bootstrap", "Bootstrap directory for use with sman"               , commandBootstrap },
  NULL,
};

int spawn(const char *command, char *argv[]) {
  int cpid = fork();
  if (!cpid) {
    return execvp(command, argv);
  }
  if (cpid < 0) {
    return 1;
  }
  return waitpid(cpid, NULL, 0);
}

// Handle config entries
static int configHandler(void *user, const char *section, const char *name, const char *value) {
  struct repository    *repo;
  struct configuration *config = user;

  // Change worldfile
  if (strcmp(section, "main") == 0) {
    if (strcmp(name, "worldfile") == 0) {
      free(config->worldfile);
      config->worldfile = strdup(value);
    } else if (strcmp(name, "repodir") == 0) {
      free(config->repodir);
      config->repodir = strdup(value);
    } else if (strcmp(name, "installedfile") == 0) {
      free(config->installedfile);
      config->installedfile = strdup(value);
    } else if (strcmp(name, "destdir") == 0) {
      free(config->destdir);
      config->destdir = strdup(value);
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
  char *tmp;
  const char *command = NULL;
  struct configuration config;
  config.repo      = NULL;
  config.destdir       = strdup("");
  config.installedfile = strdup("/etc/sman/installed");
  config.worldfile     = strdup("/etc/sman/world");
  config.repodir       = strdup("/etc/sman/repo");

  // Define arguments
  char *configfile = NULL;
  struct argparse_option options[] = {
    OPT_HELP(),
    OPT_STRING('c', "config", &configfile , "Select a specific config file"),
    OPT_END(),
  };

  // Initialize argument parser
  struct argparse argparse;
  argparse_init(&argparse, options, usage, 0);

  // Fetch longest command
  int longestCommand = 0;
  for(i=0; commands[i].name; i++) {
    longestCommand = MAX(longestCommand, strlen(commands[i].name));
  }

  // Build command list
  char *tmpstr       = malloc(8192);
  char *description  = strdup("\nCommands:\n\n");
  for(i=0; commands[i].name; i++) {
    sprintf(tmpstr, "    %-*s    %s\n", longestCommand, commands[i].name, commands[i].description);
    description = realloc(description, strlen(description) + strlen(tmpstr) + 1);
    strcat(description, tmpstr);
  }
  free(tmpstr);

  // Add command list to help
  argparse_describe(&argparse, "\nOptions:", description);

  // Parse arguments
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

  // Handle installedfile destdir
  tmp = calloc(1, strlen(config.destdir) + strlen(config.installedfile) + 1);
  strcat(tmp, config.destdir);
  strcat(tmp, config.installedfile);
  free(config.installedfile);
  config.installedfile = tmp;

  // Handle worldfile destdir
  tmp = calloc(1, strlen(config.destdir) + strlen(config.worldfile) + 1);
  strcat(tmp, config.destdir);
  strcat(tmp, config.worldfile);
  free(config.worldfile);
  config.worldfile = tmp;

  // Handle repodir destdir
  tmp = calloc(1, strlen(config.destdir) + strlen(config.repodir) + 1);
  strcat(tmp, config.destdir);
  strcat(tmp, config.repodir);
  free(config.repodir);
  config.repodir = tmp;

  // No extra arguments = no command
  if (argc < 1) {
    fprintf(stderr, "No command given.\nRun 'sman --help' to see more information.\n");
    return 1;
  }

  // Run given command
  command = *argv;
  for(i=0; commands[i].name; i++) {
    if (strcmp(commands[i].name, command) == 0) {
      return (commands[i].fn)(argc-1, (const char**)(argv+1), &config);
    }
  }

  // Still alive = unknown command
  fprintf(stderr, "Unknown command: %s\nRun 'sman --help' to see more information.\n", command);
  return 1;
}

/* #ifdef __cplusplus */
/* } // extern "C" */
/* #endif */
