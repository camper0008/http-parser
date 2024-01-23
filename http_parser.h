#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    Status,
    Headers,
    Content,
    Finished,
} Mode;

typedef enum {
    Success = 0,
    AllocationError,
    FeedingFinishedParser,
    Unreachable,
} ParserError;

typedef struct {
    uint8_t* buffer;
    size_t buffer_len;
    size_t _buffer_alloc;
    size_t content_length;
    Mode mode;
} Parser;

ParserError parser_init(Parser* parser);
ParserError parser_feed(Parser* parser, uint8_t byte);
void parser_deinit(Parser* parser);

#endif
