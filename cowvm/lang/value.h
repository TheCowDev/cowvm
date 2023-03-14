#ifndef COWVM_VALUE_H
#define COWVM_VALUE_H

#include <stdint.h>
#include "type.h"


typedef struct {
    size_t id;
    CowType type;
} _CowValue;

typedef _CowValue *CowValue;

#endif //COWVM_VALUE_H
