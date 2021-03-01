#pragma once

#ifndef PURPL_APP_INFO_H
#define PURPL_APP_INFO_H 1

#include <stdlib.h>

#include "asset.h"
#include "log.h"
#include "types.h"
#include "util.h"

struct purpl_app_info {
	struct purpl_asset *json;
};

#endif /* !PURPL_APP_INFO_H */
