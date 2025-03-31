#include "standard_library.h"
namespace core {
string __string_constants[2];
static void __init_string_constants(void)
{
__string_constants[0] = "hello";
__string_constants[1] = ", world!";
}
struct program {
//found builtins_node: <kt_declarations.node_object object at 0x1006bae00>
struct Root {
//test_function
//main
};
struct Root_builtins {
};
// Compiling Functions
struct __closure__test_function {
kt_program::variable foo;
string part2;
};
static kt_program::variable test_function(kt_program::variable foo)
{
__closure__test_function __self_closure__;
string part1;
kt_program::variable result;
int32 i;
__self_closure__.foo = foo;
int32 register_1_0;
int32 register_1_1;
int32 register_1_2;
int32 register_1_3;
string register_3_0;
string register_3_1;
string register_3_2;
string register_3_3;
string register_3_4;
string register_3_5;
float64 register_2_0;
float64 register_2_1;
float64 register_2_2;
float64 register_2_3;
part1 = __string_constants[0];
__self_closure__.part2 = __string_constants[1];
type_convert(result, part1);
i = 0;
_gt_l1:
register_1_0 = __self_closure__.part2[i];
if(!register_1_0) goto _gt_l2;
type_convert(register_3_0, i);
print(register_3_0);
type_convert(register_3_2, result);
register_3_3 = test_function__X__get_letter(&__self_closure__, i);
register_3_1 = _concatenate_string(register_3_2, "", register_3_3);
type_convert(result, register_3_1);
type_convert(register_3_4, result);
print(register_3_4);
type_convert(register_2_0, i);
register_1_2 = 20;
type_convert(register_2_1, register_1_2);
register_1_1 = register_2_0 > register_2_1;
if(!register_1_1) goto _gt_l4;
goto _gt_l2;
_gt_l4:
_gt_l3:
register_1_3 = 1;
type_convert(register_2_2, register_1_3);
type_convert(register_2_3, i);
register_2_3 += register_2_2;
type_convert(i, register_2_3);
goto _gt_l1;
_gt_l2:
type_convert(register_3_5, __self_closure__.foo);
print(register_3_5);
return result;
}
static void main()
{
kt_program::variable the_string;
kt_program::variable register_4_0;
int32 register_1_0;
string register_3_0;
register_1_0 = 11;
type_convert(register_4_0, register_1_0);
the_string = test_function(register_4_0);
type_convert(register_3_0, the_string);
print(register_3_0);
return;
}
static string test_function__X__get_letter(__closure__test_function *__closure__,int32 index)
{
int32 register_1_0;
string register_3_0;
register_1_0 = 10;
type_convert(__closure__->foo, register_1_0);
type_convert(register_3_0, __closure__->part2[index]);
return register_3_0;
}
};
}
int main(int argc, const char **argv) { __init_string_constants(); program::main(); return 0; } 
