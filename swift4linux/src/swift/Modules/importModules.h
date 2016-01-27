#ifndef IMPORTMODULE_H
#define IMPORTMODULE_H

#include "module.h"
#include <dlfcn.h>
#include "../parseCodeLine.h"

#include "log.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */


void importModules(ExecuteContext * context);

#endif /* IMPORTMODULE_H */

