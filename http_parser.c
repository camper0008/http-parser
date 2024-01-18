#include "http_parser.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void parser_new(Parser *parser) {
    parser->mode = Status;
    parser->buffer_len = 0;
    parser->buffer_alloc = 256;
    uint8_t *buffer = malloc(parser->buffer_alloc);
    parser->buffer = buffer;
}

void __parser_maybe_realloc(Parser *parser) {
    if (parser->buffer_len < parser->buffer_alloc) {
        return;
    }
    parser->buffer_alloc *= 2;
    parser->buffer = realloc(parser->buffer, parser->buffer_alloc);
}

void __parser_header_parse(Parser *parser) {
    bool is_key = true;
    for (size_t i = 0; i < parser->buffer_len && is_key; i++) {
        if (parser->buffer[i] == ':') {
            is_key = false;
            continue;
        }
        parser->buffer[i] = tolower(parser->buffer[i]);
    }
    size_t header_key_length = strlen("content-length");
    if (strncmp((const char *)parser->buffer, "content-length",
                header_key_length) != 0) {
        parser->buffer_len = 0;
        return;
    };
    char header_content[256] = {0};
    size_t header_length = 0;
    for (size_t i = header_key_length; i < parser->buffer_len; i++) {
        char curr = parser->buffer[i];
        if (curr == ' ' || curr == '\r' || curr == '\n') {
            continue;
        }
        header_content[header_length] = curr;
    }
    parser->content_length = atoi(header_content);
    parser->buffer_len = 0;
}

void parser_feed(Parser *parser, uint8_t byte) {
    size_t current = parser->buffer_len;
    __parser_maybe_realloc(parser);
    parser->buffer[current] = byte;
    parser->buffer_len += 1;
    switch (parser->mode) {
    case Status:
        if (parser->buffer_len >= 2 && parser->buffer[current] == '\n' &&
            parser->buffer[current - 1] == '\r') {
            parser->mode = Headers;
        }
        break;
    case Headers:
        if (parser->buffer_len >= 2 && parser->buffer[current] == '\n' &&
            parser->buffer[current - 1] == '\r') {
            if (parser->buffer_len == 2) {
                parser->buffer_len = 0;
                parser->mode = Content;
                return;
            }
            __parser_header_parse(parser);
        }
        break;
    case Content:
        if (parser->buffer_len == parser->content_length) {
            parser->mode = Finished;
        }
        break;
    case Finished:
        break;
    }
}

void parser_free(Parser *parser) { free(parser->buffer); }
