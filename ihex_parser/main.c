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
#include "ihex_parser.h"

int main()
{
    const uint8_t ihex_d0[] = ":108000000804002001810008C58200087181000871";
    if (!ihex_parser(ihex_d0, sizeof(ihex_d0)))
    {
        printf("Parse error\n");
    }

    const uint8_t ihex_d1[] = ":08847000A683000800A24A04E3";
    if (!ihex_parser(ihex_d1, sizeof(ihex_d1)))
    {
        printf("Parse error\n");
    }

    const uint8_t ihex_a[] = ":020000040800F2";
    if (!ihex_parser(ihex_a, sizeof(ihex_a)))
    {
        printf("Parse error\n");
    }

    const uint8_t ihex_l1[] = ":04000005080080ED82";
    if (!ihex_parser(ihex_l1, sizeof(ihex_l1)))
    {
        printf("Parse error\n");
    }

    const uint8_t ihex_eof[] = ":00000001FF";
    if (!ihex_parser(ihex_eof, sizeof(ihex_eof)))
    {
        printf("Parse error\n");
    }

    printf("------------------------------------------\n");

    const uint8_t ihex_long0[] = ":020000040800F2:108000000804002001810008C582000871810008";
    const uint8_t ihex_long1[] = "71:10801000C38200086D8100089583000800000000FD:1080";
    const uint8_t ihex_long2[] = "2000000000000000000000000000C9820008FD:10803000";
    const uint8_t ihex_long3[] = "6F81000800000000C7820008CB82";
    const uint8_t ihex_long4[] = "0008A2:108040001B8100081B8100081B8100081B810008";
    const uint8_t ihex_long5[] = "A0:08847000A683000800A24A04E3:04000005080080ED82:00000001FF";

    if (!ihex_parser(ihex_long0, sizeof(ihex_long0)))
    {
        printf("Parse error\n");
    }
    if (!ihex_parser(ihex_long1, sizeof(ihex_long1)))
    {
        printf("Parse error\n");
    }
    if (!ihex_parser(ihex_long2, sizeof(ihex_long2)))
    {
        printf("Parse error\n");
    }
    if (!ihex_parser(ihex_long3, sizeof(ihex_long3)))
    {
        printf("Parse error\n");
    }
    if (!ihex_parser(ihex_long4, sizeof(ihex_long4)))
    {
        printf("Parse error\n");
    }
    if (!ihex_parser(ihex_long5, sizeof(ihex_long5)))
    {
        printf("Parse error\n");
    }
    printf("Done");

    //getchar();

    return 0;
}