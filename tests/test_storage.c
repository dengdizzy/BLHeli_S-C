#include <assert.h>
#include <string.h>

#include "storage/blheli_s_storage.h"

void test_storage(void)
{
    struct blheli_s_config original;
    struct blheli_s_config decoded;
    uint8_t record[BLHELI_S_PARAMETER_RECORD_SIZE];

    blheli_s_config_defaults(&original);
    assert(blheli_s_storage_encode(&original, record, sizeof(record)));
    assert(blheli_s_storage_header_valid(record, sizeof(record)));
    assert(blheli_s_storage_decode(record, sizeof(record), &decoded));
    assert(memcmp(&original, &decoded, sizeof(original)) == 0);
    record[13] = 0u;
    assert(!blheli_s_storage_header_valid(record, sizeof(record)));
    assert(!blheli_s_storage_decode(record, sizeof(record), &decoded));
}
