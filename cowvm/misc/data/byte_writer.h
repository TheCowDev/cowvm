#ifndef COWVM_BYTE_WRITER_H
#define COWVM_BYTE_WRITER_H

#include <stdint.h>

typedef struct {
    char *data;
    size_t current_index;
    size_t capacity;
} ByteWriter;

void byte_writer_free(ByteWriter *writer);

size_t byte_writer_uint8(ByteWriter *writer, uint8_t value);

size_t byte_writer_int8(ByteWriter *writer, int8_t value);

size_t byte_writer_int32(ByteWriter *writer, int32_t value);

void byte_writer_int32_at(ByteWriter *writer, int32_t value, size_t offset);

size_t byte_writer_int64(ByteWriter *writer, int64_t value);

size_t byte_writer_f32(ByteWriter *writer, float value);

size_t byte_writer_f64(ByteWriter *writer, double value);

#endif //COWVM_BYTE_WRITER_H
