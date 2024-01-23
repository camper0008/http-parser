#include "http_parser.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    Parser parser;
    ParserError error = parser_init(&parser);
    if (error) {
        printf("received parser error %d\n", error);
        return error;
    }
    uint8_t response[] = "HTTP/1.1 200 OK\r\nContent-Length: 30\r\n\r\nHello world "
                         "from http-parser!\n";
    for (size_t i = 0; i < strlen((char*)response); i++) {
        error = parser_feed(&parser, response[i]);
        if (error) {
            printf("received parser error %d\n", error);
            return error;
        }
    }
    assert(parser.mode == Finished);
    assert(parser.content_length == 30);
    assert(strncmp((char*)parser.buffer, "Hello world from http-parser!\n", parser.content_length) == 0);
    parser_deinit(&parser);

    return 0;
}
