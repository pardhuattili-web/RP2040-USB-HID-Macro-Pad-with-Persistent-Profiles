#ifndef MACRO_PAD_CONFIG_STORE_H
#define MACRO_PAD_CONFIG_STORE_H

#include <stdbool.h>
#include "macro_engine.h"

typedef enum {
    CONFIG_STORE_OK = 0,
    CONFIG_STORE_INVALID,
    CONFIG_STORE_UNAVAILABLE,
    CONFIG_STORE_IO_ERROR
} config_store_status_t;

/* Default implementation supplies defaults. Persistent flash programming is intentionally
 * not enabled until a board-specific reserved flash region is defined and reviewed. */
config_store_status_t config_store_load(macro_config_t *config);
config_store_status_t config_store_save(const macro_config_t *config);

#endif
