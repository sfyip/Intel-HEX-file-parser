/*************************************************************************************
# Released under MIT License
Copyright (c) 2020 SF Yip (yipxxx@gmail.com)
Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the "Software"), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "ihex_parser.h"

#if (CONFIG_IHEX_HOOK > 0u)
    #include <stdio.h>
#endif


//IHEX file parser state machine
#define START_CODE_STATE        0
#define BYTE_COUNT_0_STATE      1
#define BYTE_COUNT_1_STATE      2
#define ADDR_0_STATE            3
#define ADDR_1_STATE            4
#define ADDR_2_STATE            5
#define ADDR_3_STATE            6
#define RECORD_TYPE_0_STATE     7
#define RECORD_TYPE_1_STATE     8
#define DATA_STATE              9
#define CHECKSUM_0_STATE        10
#define CHECKSUM_1_STATE        11

#define INVALID_HEX_CHAR        'x'
#define IHEX_DATA_SIZE          32

static uint8_t HexToDec(uint8_t h)
{
    if (h >= '0' && h <= '9')
        return h - '0';
    else if (h >= 'A' && h <= 'F')
        return h - 'A' + 0xA;
    else if (h >= 'a' && h <= 'z')
        return h - 'a' + 0xA;
    else
        return INVALID_HEX_CHAR;
}

static uint8_t state;
static uint8_t byte_count;
static uint32_t address;
static uint8_t record_type;
static uint8_t data[IHEX_DATA_SIZE];
static uint8_t data_size_in_nibble;
static uint8_t checksum;

static uint8_t temp_cs;         // save checksum high byte
static uint8_t calc_cs;         // calculate checksum
static bool calc_cs_toogle = false;

#if (CONFIG_IHEX_HOOK > 0u)
static void ihex_hook()
{
    switch (record_type)
    {
    case 0:         //DATA
        printf("WriteData (0x%08X):", address);

        uint8_t i;
        uint8_t data_size = data_size_in_nibble >> 1;
        for (i = 0; i < data_size; i++)
        {
            printf("%02X", data[i]);
        }
        printf("\n");
        break;

    case 1:         //EOF
        printf("EOF\n");
        break;

    case 4:         //Set linear address
        printf("Set Linear Address:%08X\n", address);
        break;

    case 5:         // Start linear address
        printf("Start linear address\n");
        break;

    default:
        break;
    }
}
#endif


void write_flash_data(uint32_t address, const uint8_t *flashRawData, uint32_t size)
{
    //
}

bool ihex_parser(const uint8_t *steambuf, uint32_t size)
{
    uint32_t i;
    uint8_t c, hc;
    
    for (i = 0; i<size; i++)
    {
        c = steambuf[i];

        if (c == '\0')
        {
            return true;
        }

        if (state == START_CODE_STATE)
        {
            calc_cs = 0x00;
            calc_cs_toogle = false;
        }
        else if (state >= BYTE_COUNT_0_STATE && state <= CHECKSUM_1_STATE)
        {
            if ((hc = HexToDec(c)) == INVALID_HEX_CHAR)
            {
                return false;
            }

            if (!calc_cs_toogle)
            {
                temp_cs = hc;
            }
            else
            {
                calc_cs += (temp_cs << 4) | hc;
            }
            calc_cs_toogle = !calc_cs_toogle;
        }

        switch (state)
        {
        case START_CODE_STATE:
            if (c == '\r' || c == '\n')
            {
                continue;
            }
            else if (c == ':')
            {
                byte_count = 0;
                record_type = 0;
                address &= 0xFFFF0000;
                memset(data, 0, sizeof(data));
                data_size_in_nibble = 0;
                checksum = 0x00;
                ++state;
            }
            else
            {
                return false;
            }
            break;

        case BYTE_COUNT_0_STATE:
        case BYTE_COUNT_1_STATE:
            byte_count = (byte_count << 4) | hc;
            ++state;
            break;

        case ADDR_0_STATE:
        case ADDR_1_STATE:
        case ADDR_2_STATE:
        case ADDR_3_STATE:
            uint32_t address_hi = address & 0xFFFF0000;
            address = ((address << 4) | hc) & 0x0000FFFF;   // only alter lower 16-bit address
            address = address_hi | address;
            ++state;
            break;

        case RECORD_TYPE_0_STATE:
            if (hc != 0)
            {
                return false;
            }
            ++state;
            break;

        case RECORD_TYPE_1_STATE:
            if (!(hc >= 0 && hc <= 5))
            {
                return false;
            }
            
            record_type = hc;

            if (byte_count == 0)
            {
                state = CHECKSUM_0_STATE;
            }
            else if (byte_count > sizeof(data))
            {
                return false;
            }
            else
            {
                ++state;
            }

            break;

        case DATA_STATE:
            uint8_t b_index = data_size_in_nibble >> 1;
            data[b_index] = (data[b_index] << 4) | hc;

            ++data_size_in_nibble;
            if ((data_size_in_nibble >> 1) >= byte_count)
            {
                ++state;
            }
            break;

        case CHECKSUM_0_STATE:
            ++state;
            break;

        case CHECKSUM_1_STATE:
            if (calc_cs != 0x00)
            {
                return false;
            }

            if (record_type == 0)
            {
                write_flash_data(address, data, data_size_in_nibble>>1);
            }
            else if (record_type == 4)
            {
                uint32_t hiword = ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16);
                address = hiword & 0xFFFF0000;
            }

#if (CONFIG_IHEX_HOOK > 0u)
            ihex_hook();
#endif

            state = START_CODE_STATE;
            break;

        default:
            return false;
        }
    }
    return true;
}


