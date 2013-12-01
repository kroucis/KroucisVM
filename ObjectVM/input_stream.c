//
//  input_stream.c
//  ObjectVM
//
//  Created by Kyle Roucis on 13-11-25.
//  Copyright (c) 2013 Kyle Roucis. All rights reserved.
//

#include "input_stream.h"

#include <stdlib.h>

struct input_stream
{
    char* stream;
    uint64_t length;

    uint64_t index;

    uint64_t snapshots[16];
    uint8_t snapshotIndex;
};

input_stream* input_stream_init_cstr(char* input, uint64_t length)
{
    input_stream* stream = malloc(sizeof(input_stream));

    stream->index = 0;
    stream->length = length;
    stream->stream = input;
    stream->snapshotIndex = 0;

    return stream;
}

void input_stream_dealloc(input_stream* stream)
{
    free(stream);
}

char input_stream_next(input_stream* stream)
{
    return stream->stream[stream->index];
}

void input_stream_back(input_stream* stream, uint64_t backamount)
{
    stream->index -= backamount;
}

char input_stream_consume(input_stream* stream)
{
    return stream->stream[stream->index++];
}

int input_stream_at_end(input_stream* stream, uint64_t lookahead)
{
    if (stream->index + lookahead > stream->length)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

char input_stream_peek(input_stream* stream, uint64_t lookahead)
{
    if (input_stream_at_end(stream, lookahead))
    {
        return '\0';
    }
    else
    {
        return stream->stream[stream->index + lookahead];
    }
}

void input_stream_take_snapshot(input_stream* stream)
{
    stream->snapshots[stream->snapshotIndex++] = stream->index;
}

void input_stream_resume_snapshot(input_stream* stream)
{
    stream->index = stream->snapshots[--stream->snapshotIndex];
}

void input_stream_discard_snapshot(input_stream* stream)
{
    stream->snapshotIndex--;
}
