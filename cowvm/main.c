#include "cow.h"
#include "exec/interp.h"

int main() {

    CowModule module = cow_module_create();

    CowType arg_type = cow_type_i64();

    CowFunc my_func = cow_create_func(module, "fib", &arg_type, 1, cow_type_i64());
    CowBuilder builder = cow_func_get_builder(my_func);
    CowBlock n_true = cow_builder_create_block(builder);
    CowBlock n_false = cow_builder_create_block(builder);

    CowValue n_arg = cow_builder_get_arg(builder, 0);
    CowValue const_1 = cow_builder_const_i64(builder, 1);
    CowValue test_n = cow_builder_smaller_eq(builder, n_arg, const_1);
    cow_builder_cond_br(builder, test_n, n_true, n_false);

    cow_builder_set_current_block(builder, n_true);
    cow_builder_ret(builder, n_arg);

    cow_builder_set_current_block(builder, n_false);
    CowValue call_one_arg = cow_builder_sub(builder, n_arg, const_1);
    CowValue first_call_result = cow_builder_call_func(builder, my_func, &call_one_arg, 1);
    CowValue call_two_arg = cow_builder_sub(builder, n_arg, cow_builder_const_i64(builder, 2));
    CowValue second_call_result = cow_builder_call_func(builder, my_func, &call_two_arg, 1);
    cow_builder_ret(builder, cow_builder_add(builder, first_call_result, second_call_result));

    CowInterpValue arg;
    arg.value_i64 = 22;
    int result = (int) cow_interpret("fib", module, &arg).value_i64;

    printf("%d", result);

    return 0;
}
