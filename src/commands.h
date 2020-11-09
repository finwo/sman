#ifndef _SMAN_COMMANDS_H_
#define _SMAN_COMMANDS_H_

#include "common.h"

int commandAdd(int argc, const char **argv, struct configuration *config);
int commandBootstrap(int argc, const char **argv, struct configuration *config);
int commandInstall(int argc, const char **argv, struct configuration *config);
int commandUpdate(int argc, const char **argv, struct configuration *config);

#endif // _SMAN_COMMANDS_H_
