#include "http_parser.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ParserError parser_init(Parser* parser)
{
    parser->mode = Status;
    parser->buffer_len = 0;
    parser->_buffer_alloc = 256;
    uint8_t* buffer = malloc(parser->_buffer_alloc);
    if (buffer == NULL) {
        return AllocationError;
    }
    parser->buffer = buffer;
    return Success;
}

static inline ParserError parser_maybe_realloc(Parser* parser)
{
    if (parser->buffer_len < parser->_buffer_alloc) {
        return 0;
    }
    parser->_buffer_alloc *= 2;
    uint8_t* new_ptr = realloc(parser->buffer, parser->_buffer_alloc);
    if (new_ptr == NULL) {
        return AllocationError;
    }
    parser->buffer = new_ptr;
    return Success;
}

static inline ParserError parser_header_parse(Parser* parser)
{
#define MAX_HEADER_VALUE_SIZE 256
    bool is_key = true;
    for (size_t i = 0; i < parser->buffer_len && is_key; i++) {
        if (parser->buffer[i] == ':') {
            is_key = false;
            continue;
        }
        parser->buffer[i] = (uint8_t)tolower(parser->buffer[i]);
    }
    size_t header_key_length = strlen("content-length");
    if (strncmp((const char*)parser->buffer, "content-length",
            header_key_length)
        != 0) {
        parser->buffer_len = 0;
        return Success;
    };

    assert(parser->buffer[header_key_length] == ':');
    header_key_length++;

    uint8_t header_content[MAX_HEADER_VALUE_SIZE] = { 0 };
    size_t header_length = 0;
    for (size_t i = header_key_length; i < parser->buffer_len; i++) {
        uint8_t curr = parser->buffer[i];
        if (curr == ' ' && header_length == 0) {
            continue;
        }
        header_content[header_length] = curr;
        header_length += 1;
    }
    parser->content_length
        = strtoull((const char*)header_content, NULL, 10);
    parser->buffer_len = 0;
    return 0;
}

ParserError parser_feed(Parser* parser, uint8_t byte)
{
    if (parser->mode == Finished) {
        return FeedingFinishedParser;
    }
    size_t current = parser->buffer_len;
    int realloc_error = parser_maybe_realloc(parser);
    if (realloc_error) {
        return realloc_error;
    };
    parser->buffer[current] = byte;
    parser->buffer_len += 1;
    switch (parser->mode) {
    case Status:
        if (parser->buffer_len >= 2 && parser->buffer[current] == '\n' && parser->buffer[current - 1] == '\r') {
            parser->mode = Headers;
            parser->buffer_len = 0;
        }
        return Success;
    case Headers:
        if (parser->buffer_len >= 2 && parser->buffer[current] == '\n' && parser->buffer[current - 1] == '\r') {
            if (parser->buffer_len == 2) { // blank line with '\r\n' = content follows
                parser->buffer_len = 0;
                parser->mode = Content;
                return Success;
            }
            parser->buffer_len -= 2; // strip \r\n
            parser_header_parse(parser);
        }
        return Success;
    case Content:
        if (parser->buffer_len >= parser->content_length) {
            parser->mode = Finished;
        }
        return Success;
    case Finished:
        printf("Somehow reached unreachable code.");
        exit(1);
    }
}

void parser_deinit(Parser* parser) { free(parser->buffer); }
