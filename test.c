#include "http_parser.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    Parser parser;
    parser_new(&parser);
    char *response = "HTTP/1.1 200 OK\r\nContent-Length: 29\r\n\r\nHello world "
                     "from http-parser!\n";
    for (size_t i = 0; i < strlen(response); i++) {
        parser_feed(&parser, response[i]);
    }
    for (size_t i = 0; i < parser.buffer_len; i++) {
        printf("%c", parser.buffer[i]);
    }
    parser_free(&parser);

    return 0;
}
