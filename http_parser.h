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

typedef struct {
    uint8_t *buffer;
    size_t buffer_len;
    size_t buffer_alloc;
    Mode mode;
    size_t content_length;
} Parser;

void parser_new(Parser *parser);
void parser_feed(Parser *parser, uint8_t byte);
void parser_free(Parser *parser);

#endif
