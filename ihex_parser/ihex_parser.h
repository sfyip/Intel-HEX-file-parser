#ifndef _IHEX_PARSER_H_
#define _IHEX_PARSER_H_

#include <stdint.h>
#include <stdbool.h>

#define CONFIG_IHEX_HOOK        1u          // Output parse status

bool ihex_parser(const uint8_t *steambuf, uint32_t size);

#endif
