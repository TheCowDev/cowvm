#ifndef COWVM_TYPE_H
#define COWVM_TYPE_H

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

#endif //COWVM_TYPE_H
