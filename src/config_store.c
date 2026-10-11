#include "config_store.h"

#include <stddef.h>

config_store_status_t config_store_load(macro_config_t *config)
{
    if (config == NULL) {
        return CONFIG_STORE_INVALID;
    }

    /* Safe default: never read/write an unreserved flash address. */
    macro_config_defaults(config);
    return CONFIG_STORE_UNAVAILABLE;
}

config_store_status_t config_store_save(const macro_config_t *config)
{
    if (config == NULL || !macro_config_validate(config)) {
        return CONFIG_STORE_INVALID;
    }
    /* Deliberately disabled; add a reviewed, sector-aligned backend for the selected build. */
    return CONFIG_STORE_UNAVAILABLE;
}
