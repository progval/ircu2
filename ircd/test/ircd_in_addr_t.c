/* ircd_in_addr_t.c - Test file for IP address manipulation */

#include "ircd_log.h"
#include "ircd_string.h"
#include "numnicks.h"
#include "res.h"
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>

/** Structure to describe a test for IP address parsing and unparsing. */
struct address_test {
    const char *text; /**< Textual address to parse. */
    const char *canonical; /**< Canonical form of address. */
    struct irc_in_addr expected; /**< Parsed address. */
    const char *base64_v4; /**< v4-only compatible base64 encoding. */
    const char *base64_v6; /**< v6-compatible base64 encoding. */
    unsigned int is_valid : 1; /**< is address valid? */
    unsigned int is_ipv4 : 1; /**< is address ipv4? */
    unsigned int is_loopback : 1; /**< is address loopback? */
};

/** Array of addresses to test with. */
static struct address_test test_addrs[] = {
    { "::", "0::",
      {{ 0, 0, 0, 0, 0, 0, 0, 0 }},
      "AAAAAA", "_", 0, 0, 0 },
    { "::1", "0::1",
      {{ 0, 0, 0, 0, 0, 0, 0, 1 }},
      "AAAAAA", "_AAB", 1, 0, 1 },
    { "127.0.0.1", "127.0.0.1",
      {{ 0, 0, 0, 0, 0, 0, 0x7f00, 1 }},
      "B]AAAB", "B]AAAB", 1, 1, 1 },
    { "::ffff:127.0.0.3", "127.0.0.3",
      {{ 0, 0, 0, 0, 0, 0xffff, 0x7f00, 3 }},
      "B]AAAD", "B]AAAD", 1, 1, 1 },
    { "2002:7f00:3::1", "2002:7f00:3::1",
      {{ 0x2002, 0x7f00, 3, 0, 0, 0, 0, 1 }},
      "B]AAAD", "CACH8AAAD_AAB", 1, 0, 0 },
    { "8352:0344:0:0:0:0:2001:1204", "8352:344::2001:1204",
      {{ 0x8352, 0x344, 0, 0, 0, 0, 0x2001, 0x1204 }},
      "AAAAAA", "INSANE_CABBIE", 1, 0, 0 },
    { 0 },
};

/** Perform tests for a single IP address.
 * @param[in] addr Address test structure.
 */
static void
test_address(struct address_test *addr)
{
    struct irc_in_addr parsed;
    unsigned int ii, len, val;
    char unparsed[64], base64_v4[64], base64_v6[64];

    /* Convert expected address to network order. */
    for (ii = 0; ii < 8; ++ii)
        addr->expected.in6_16[ii] = htons(addr->expected.in6_16[ii]);
    /* Make sure the text form is parsed as expected. */
    len = ircd_aton(&parsed, addr->text);
    assert(len == strlen(addr->text));
    assert(!irc_in_addr_cmp(&parsed, &addr->expected));
    /* Make sure it converts back to ASCII. */
    ircd_ntoa_r(unparsed, &parsed);
    assert(!strcmp(unparsed, addr->canonical));
    /* Check IP-to-base64 conversion. */
    iptobase64(base64_v4, &parsed, sizeof(base64_v4), 0);
    iptobase64(base64_v6, &parsed, sizeof(base64_v6), 1);
    if (addr->base64_v4)
        assert(!strcmp(base64_v4, addr->base64_v4));
    if (addr->base64_v6)
        assert(!strcmp(base64_v6, addr->base64_v6));
    /* Check testable attributes. */
    val = irc_in_addr_valid(&parsed);
    assert(!!val == addr->is_valid);
    val = irc_in_addr_is_ipv4(&parsed);
    assert(!!val == addr->is_ipv4);
    val = irc_in_addr_is_loopback(&parsed);
    assert(!!val == addr->is_loopback);
    printf("Passed: %s (%s/%s)\n", addr->text, base64_v4, base64_v6);
}

int
main(int argc, char *argv[])
{
    unsigned int ii;

    for (ii = 0; test_addrs[ii].text; ++ii)
        test_address(&test_addrs[ii]);

    return 0;
}
