# Intel-HEX-file-parser

This HEX file parser analyses each byte by using state machine method.

The fields of Intel HEX records includes: <br />
![#f03c15](https://via.placeholder.com/15/f03c15/000000?text=+) `Start code`
![#c5f015](https://via.placeholder.com/15/c5f015/000000?text=+) `Byte count`
![#1589F0](https://via.placeholder.com/15/1589F0/000000?text=+) `Address`
![#330077](https://via.placeholder.com/15/330077/000000?text=+) `Record type`
![#7700F0](https://via.placeholder.com/15/7700F0/000000?text=+) `Data`
![#3333F0](https://via.placeholder.com/15/3333F0/000000?text=+) `Checksum`

For example:<br/>
If the input string is <b>":020000040800F2:10"</b><br/>
The parser will iterate through the string, the last character will be recognized as "Byte count last nibble" field.
Then, the parser will save current state and return back to the caller function.

If the 2nd input string is <b>"8000000804002001810008C582000871810008"</b>
The parser will parse the string based on previous saved state. Hence, the first character will be recognized as "Address" field.

#### Test Code:
```C
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
```

#### Result: <br/>
Set Linear Address:08000000 <br/>
WriteData (0x08008000):0804002001810008C582000871810008 <br/>
WriteData (0x08008010):C38200086D8100089583000800000000 <br/>
WriteData (0x08008020):000000000000000000000000C9820008 <br/>
WriteData (0x08008030):6F81000800000000C7820008CB820008 <br/>
WriteData (0x08008040):1B8100081B8100081B8100081B810008 <br/>
WriteData (0x08008470):A683000800A24A04 <br/>
Start linear address <br/>
EOF

### Example Code - use fread() to read and parse the ihex file:
```C
    ihex_set_callback_func(write_flash_data);
    ihex_reset_state();
    
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
            fbuf[readcount - 1] = '\0';     // Add null teminated char
        }
        if (!ihex_parser(fbuf, sizeof(fbuf)))
        {
            printf("parse failed\n");
            goto EXIT;
        }
    }
    
    return_status = true;

EXIT:
    if (fp)
        fclose(fp);

    return return_status;
```
