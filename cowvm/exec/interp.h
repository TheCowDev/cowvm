#ifndef COWVM_INTERP_H
#define COWVM_INTERP_H

#include <stdint.h>
#include <stdbool.h>
#include "../cow.h"

typedef struct {
    union {
        int64_t value_i64;
        int32_t value_i32;
        int16_t value_i16;
        int8_t value_i8;
        bool value_i1;
        float value_f32;
        double value_f64;
        void *value_ptr;
    };
} CowInterpValue;

static CowInterpValue cow_interp_value_from_i64(int64_t value) {
    CowInterpValue result;
    result.value_i64 = value;
    return result;
}

CowInterpValue cow_interpret(char *name, CowModule module, CowInterpValue *values);

#endif //COWVM_INTERP_H
