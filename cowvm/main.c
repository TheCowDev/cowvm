#include "cow.h"
#include "exec/interp.h"

int main() {

    CowModule module = cow_module_create();

    CowFunc my_func = cow_create_func(module, "main", NULL, 0, cow_type_void());

    CowBuilder builder = cow_func_get_builder(my_func);

    CowValue value1 = cow_builder_const_i64(builder, 100);
    CowValue value2 = cow_builder_const_i64(builder, 100);
    CowValue value_result = cow_builder_diff(builder, value1, value2);
    cow_builder_ret(builder, value_result);
    int result = (int) cow_interpret("main", module, NULL).value_i1;
    printf("%d", result);

    return 0;
}
