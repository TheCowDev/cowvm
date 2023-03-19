#ifndef COWVM_TYPE_H
#define COWVM_TYPE_H

#include <stdbool.h>

typedef enum {
    COW_DATA_TYPE_VOID = 0,
    COW_DATA_TYPE_I64 = 1,
    COW_DATA_TYPE_I32 = 2,
    COW_DATA_TYPE_I16 = 3,
    COW_DATA_TYPE_I8 = 4,
    COW_DATA_TYPE_F64 = 5,
    COW_DATA_TYPE_F32 = 6,
    COW_DATA_TYPE_PTR = 7,
} CowDataType;

typedef struct {
    CowDataType data_type;
} CowType;

static CowType cow_type_from_data_type(CowDataType data_type) {
    CowType type = {0};
    type.data_type = data_type;
    return type;
}

static CowType cow_type_void() {
    return cow_type_from_data_type(COW_DATA_TYPE_VOID);
}

static CowType cow_type_i64() {
    return cow_type_from_data_type(COW_DATA_TYPE_I64);
}

static CowType cow_type_i32() {
    return cow_type_from_data_type(COW_DATA_TYPE_I32);
}

static CowType cow_type_i16() {
    return cow_type_from_data_type(COW_DATA_TYPE_I16);
}

static CowType cow_type_i8() {
    return cow_type_from_data_type(COW_DATA_TYPE_I8);
}

static CowType cow_type_f32() {
    return cow_type_from_data_type(COW_DATA_TYPE_F32);
}

static CowType cow_type_f64() {
    return cow_type_from_data_type(COW_DATA_TYPE_F64);
}

static CowType cow_type_ptr() {
    return cow_type_from_data_type(COW_DATA_TYPE_PTR);
}

static bool cow_type_is_decimal(CowType type) {
    return type.data_type == COW_DATA_TYPE_F32 || type.data_type == COW_DATA_TYPE_F64;
}

#endif //COWVM_TYPE_H
