#ifndef LIBWALLY_BASE58_H
#define LIBWALLY_BASE58_H

#include <include/wally_core.h>

#include <stddef.h>
#include <stdint.h>

/**
 * Calculate the base58 checksum of a block of binary data.
 *
 * @bytes_in: Binary data to calculate the checksum for.
 * @len: The length of @bytes_in in bytes.
 */
uint32_t base58_get_checksum(
    const unsigned char *bytes_in,
    size_t len);

/* Returns non-zero on error. If 0 is returned then:
 * *len <= input value - OK, bytes_out contains data.
 * *len > input value - Failed and bytes_out untouched.
 */
WALLY_CORE_API int base58_decode(const char *base58, size_t base58_len,
                         unsigned char *bytes_out, size_t *len);

#endif /* LIBWALLY_BASE58_H */
