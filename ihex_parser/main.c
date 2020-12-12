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

#include <stdio.h>
#include <stdlib.h> 
#include "ihex_parser.h"

bool write_flash_data(uint32_t addr, const uint8_t *buf, uint8_t bufsize)
{
    uint8_t i;

    printf("[CALLBACK PROG] %08X:", addr);
    for (i=0; i<bufsize; i++)
    {
        printf("%02X", buf[i]);
    }
    printf("\n");

    return true;
}

bool testcase_parse_indv_record_type()
{
    printf("=== testcase_parse_indv_record_type\n");

    const uint8_t ihex_d0[] = ":108000000804002001810008C58200087181000871";
    if (!ihex_parser(ihex_d0, sizeof(ihex_d0)))
    {
        printf("Parse error d0\n");
        return false;
    }

    const uint8_t ihex_d1[] = ":08847000A683000800A24A04E3";
    if (!ihex_parser(ihex_d1, sizeof(ihex_d1)))
    {
        printf("Parse error d1\n");
        return false;
    }

    const uint8_t ihex_a[] = ":020000040800F2";
    if (!ihex_parser(ihex_a, sizeof(ihex_a)))
    {
        printf("Parse error d2\n");
        return false;
    }

    const uint8_t ihex_l1[] = ":04000005080080ED82";
    if (!ihex_parser(ihex_l1, sizeof(ihex_l1)))
    {
        printf("Parse error d3\n");
        return false;
    }

    const uint8_t ihex_eof[] = ":00000001FF";
    if (!ihex_parser(ihex_eof, sizeof(ihex_eof)))
    {
        printf("Parse error d4\n");
        return false;
    }
    return true;
}

bool testcase_long_str()
{
    printf("=== testcase_long_str\n");

    const uint8_t ihex_long0[] = ":020000040800F2\n:108000000804002001810008C582000871810008";
    const uint8_t ihex_long1[] = "71\n:10801000C38200086D8100089583000800000000FD\r\n:1080";
    const uint8_t ihex_long2[] = "2000000000000000000000000000C9820008FD\n:10803000";
    const uint8_t ihex_long3[] = "6F81000800000000C7820008CB82";
    const uint8_t ihex_long4[] = "0008A2\r\n:108040001B8100081B8100081B8100081B810008";
    const uint8_t ihex_long5[] = "A0\n:08847000A683000800A24A04E3\n:04000005080080ED82\n:00000001FF";

    if (!ihex_parser(ihex_long0, sizeof(ihex_long0)))
    {
        printf("Parse error long0\n");
        return false;
    }
    if (!ihex_parser(ihex_long1, sizeof(ihex_long1)))
    {
        printf("Parse error long1\n");
        return false;
    }
    if (!ihex_parser(ihex_long2, sizeof(ihex_long2)))
    {
        printf("Parse error long2\n");
        return false;
    }
    if (!ihex_parser(ihex_long3, sizeof(ihex_long3)))
    {
        printf("Parse error long3\n");
        return false;
    }
    if (!ihex_parser(ihex_long4, sizeof(ihex_long4)))
    {
        printf("Parse error long4\n");
        return false;
    }
    if (!ihex_parser(ihex_long5, sizeof(ihex_long5)))
    {
        printf("Parse error long5\n");
        return false;
    }
    return true;
}

bool testcase_segment_address()
{
    printf("=== testcase_segment_address\n");

    const uint8_t ihex_ex_seg[] = "\n:020000021200EA\n:10246200464C5549442050524F46494C4500464C33\n:1000100018F09FE5805F20B9F0FF1FE518F09FE51D\n:00000001FF";
    if (!ihex_parser(ihex_ex_seg, sizeof(ihex_ex_seg)))
    {
        printf("Parse error - segment address\n");
        return false;
    }
    return true;
}

bool testcase_read_file()
{
    printf("=== testcase_read_file\n");

    /* Open HEX file, read per 256 byte block size*/
    bool return_status = false;

    uint8_t fbuf[256];
    size_t readcount = 0;

    FILE *fp = fopen("test.hex", "r");
    if (fp == NULL)
    {
        printf("fopen: cannot open hex file\n");
        goto EXIT;
    }

    while ((readcount = fread(fbuf, 1, sizeof(fbuf), fp)) > 0)
    {
        if (readcount < sizeof(fbuf))
        {
            fbuf[readcount] = '\0';     // Add null teminated char
        }
        if (!ihex_parser(fbuf, sizeof(fbuf)))
        {
            printf("parse failed - HEX file using fread\n");
            goto EXIT;
        }
    }

    return_status = true;

EXIT:
    if (fp)
        fclose(fp);

    return return_status;
}

void print_seperator()
{
    printf("------------------------------------------\n");
}

int main()
{
    ihex_set_callback_func(write_flash_data);

    print_seperator();

    if (!testcase_parse_indv_record_type())
    {
        printf("Failed at testcase_parse_indv_record_type()\n");
        return EXIT_FAILURE;
    }

    print_seperator();

    if (!testcase_long_str())
    {
        printf("Failed at testcase_long_str()\n");
        return EXIT_FAILURE;
    }
    
    print_seperator();

    if (!testcase_segment_address())
    {
        printf("Failed at testcase_segment_address()\n");
        return EXIT_FAILURE;
    }

    print_seperator();

    ihex_reset_state();
    
    if (!testcase_read_file())
    {
        printf("Failed at testcase_read_file()\n");
        return EXIT_FAILURE;
    }

    print_seperator();

    printf("Done");

    //getchar();

    return EXIT_SUCCESS;
}