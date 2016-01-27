#include "../lib/LibSha1.h"
#include <stdlib.h>
#include "fcntl.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include "../data_core/JSON.h"
#include "../lib/JSLog.h"


JSON * resolveImage(JSON * image_info);
