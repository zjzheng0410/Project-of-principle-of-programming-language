#include "ppl_runtime.h"

#include "rt_memory.h"

#include <stdio.h>
#include <string.h>

typedef struct Binding {
    const char *name;
    PPLValue value;
    struct Binding *next;
} Binding;

struct PPLEnv {
    Binding *bindings;
    PPLEnv *parent;
};

static PPLValue builtin_call(const char *name, int argc, PPLValue *args);
static int is_builtin_name(const char *name);

PPLValue ppl_int(long value) {
    PPLValue v;
    v.kind = PPL_INT;
    v.as.int_value = value;
    return v;
}

PPLValue ppl_bool(int value) {
    PPLValue v;
    v.kind = PPL_BOOL;
    v.as.bool_value = value ? 1 : 0;
    return v;
}

PPLValue ppl_none(void) {
    PPLValue v;
    v.kind = PPL_NONE;
    return v;
}

PPLValue ppl_some(PPLValue value) {
    PPLValue v;
    v.kind = PPL_SOME;
    v.as.some_value = (PPLValue *)ppl_xmalloc(sizeof(PPLValue));
    *v.as.some_value = value;
    return v;
}

PPLValue ppl_array(int count, PPLValue *items) {
    if (count < 0) {
        ppl_runtime_error("数组运行时", "数组长度不能为负数");
    }
    PPLArray *array = (PPLArray *)ppl_xcalloc(1, sizeof(PPLArray));
    array->length = (size_t)count;
    array->capacity = (size_t)count;
    if (count > 0) {
        array->items = (PPLValue *)ppl_xmalloc(sizeof(PPLValue) * (size_t)count);
        for (int i = 0; i < count; i++) {
            array->items[i] = items[i];
        }
    }
    PPLValue v;
    v.kind = PPL_ARRAY;
    v.as.array_value = array;
    return v;
}

PPLValue ppl_make_function(const char *name, PPLNativeFn call, PPLEnv *env) {
    PPLFunc *fn = (PPLFunc *)ppl_xcalloc(1, sizeof(PPLFunc));
    fn->name = name;
    fn->call = call;
    fn->env = env;
    PPLValue v;
    v.kind = PPL_FUNCTION;
    v.as.function_value = fn;
    return v;
}

void ppl_expect_argc(const char *name, int argc, int expected) {
    if (argc != expected) {
        char buf[160];
        snprintf(buf, sizeof(buf), "%s 参数数量错误：期望 %d，实际 %d", name, expected, argc);
        ppl_runtime_error("函数调用", buf);
    }
}

int ppl_truthy(PPLValue value) {
    switch (value.kind) {
        case PPL_BOOL:
            return value.as.bool_value != 0;
        case PPL_INT:
            return value.as.int_value != 0;
        case PPL_NONE:
            return 0;
        case PPL_SOME:
            return 1;
        case PPL_ARRAY:
            return value.as.array_value->length != 0;
        case PPL_FUNCTION:
            return 1;
    }
    return 0;
}

static void print_array(PPLArray *array) {
    printf("[");
    for (size_t i = 0; i < array->length; i++) {
        if (i > 0) {
            printf(", ");
        }
        ppl_print_value(array->items[i]);
    }
    printf("]");
}

void ppl_print_value(PPLValue value) {
    switch (value.kind) {
        case PPL_INT:
            printf("%ld", value.as.int_value);
            break;
        case PPL_BOOL:
            printf("%s", value.as.bool_value ? "true" : "false");
            break;
        case PPL_NONE:
            printf("None");
            break;
        case PPL_SOME:
            printf("Some(");
            ppl_print_value(*value.as.some_value);
            printf(")");
            break;
        case PPL_ARRAY:
            print_array(value.as.array_value);
            break;
        case PPL_FUNCTION:
            printf("<fn %s>", value.as.function_value->name);
            break;
    }
}

PPLEnv *ppl_env_new(PPLEnv *parent) {
    PPLEnv *env = (PPLEnv *)ppl_xcalloc(1, sizeof(PPLEnv));
    env->parent = parent;
    return env;
}

void ppl_env_define(PPLEnv *env, const char *name, PPLValue value) {
    Binding *binding = (Binding *)ppl_xcalloc(1, sizeof(Binding));
    binding->name = name;
    binding->value = value;
    binding->next = env->bindings;
    env->bindings = binding;
}

static Binding *env_find(PPLEnv *env, const char *name) {
    for (PPLEnv *e = env; e; e = e->parent) {
        for (Binding *b = e->bindings; b; b = b->next) {
            if (strcmp(b->name, name) == 0) {
                return b;
            }
        }
    }
    return NULL;
}

PPLValue ppl_env_get(PPLEnv *env, const char *name) {
    Binding *binding = env_find(env, name);
    if (!binding) {
        char buf[160];
        snprintf(buf, sizeof(buf), "未定义变量 '%s'", name);
        ppl_runtime_error("变量读取", buf);
    }
    return binding->value;
}

void ppl_env_assign(PPLEnv *env, const char *name, PPLValue value) {
    Binding *binding = env_find(env, name);
    if (!binding) {
        char buf[160];
        snprintf(buf, sizeof(buf), "未定义变量 '%s'", name);
        ppl_runtime_error("变量赋值", buf);
    }
    binding->value = value;
}

PPLValue ppl_call_value(PPLValue callee, int argc, PPLValue *args) {
    if (callee.kind != PPL_FUNCTION) {
        ppl_runtime_error("函数调用", "被调用值不是函数");
    }
    return callee.as.function_value->call(callee.as.function_value, argc, args);
}

PPLValue ppl_call_name(PPLEnv *env, const char *name, int argc, PPLValue *args) {
    Binding *binding = env_find(env, name);
    if (binding) {
        return ppl_call_value(binding->value, argc, args);
    }
    if (is_builtin_name(name)) {
        return builtin_call(name, argc, args);
    }
    char buf[160];
    snprintf(buf, sizeof(buf), "未定义函数 '%s'", name);
    ppl_runtime_error("函数调用", buf);
    return ppl_none();
}

static long expect_int(PPLValue value, const char *op) {
    if (value.kind != PPL_INT) {
        char buf[160];
        snprintf(buf, sizeof(buf), "运算符 %s 需要整数", op);
        ppl_runtime_error("表达式运算", buf);
    }
    return value.as.int_value;
}

PPLValue ppl_binary(const char *op, PPLValue left, PPLValue right) {
    if (strcmp(op, "&&") == 0) return ppl_bool(ppl_truthy(left) && ppl_truthy(right));
    if (strcmp(op, "||") == 0) return ppl_bool(ppl_truthy(left) || ppl_truthy(right));
    long a = expect_int(left, op);
    long b = expect_int(right, op);
    if (strcmp(op, "+") == 0) return ppl_int(a + b);
    if (strcmp(op, "-") == 0) return ppl_int(a - b);
    if (strcmp(op, "*") == 0) return ppl_int(a * b);
    if (strcmp(op, "/") == 0) {
        if (b == 0) ppl_runtime_error("表达式运算", "除数不能为 0");
        return ppl_int(a / b);
    }
    if (strcmp(op, "%") == 0) {
        if (b == 0) ppl_runtime_error("表达式运算", "取模除数不能为 0");
        return ppl_int(a % b);
    }
    if (strcmp(op, "==") == 0) return ppl_bool(a == b);
    if (strcmp(op, "!=") == 0) return ppl_bool(a != b);
    if (strcmp(op, "<") == 0) return ppl_bool(a < b);
    if (strcmp(op, "<=") == 0) return ppl_bool(a <= b);
    if (strcmp(op, ">") == 0) return ppl_bool(a > b);
    if (strcmp(op, ">=") == 0) return ppl_bool(a >= b);
    ppl_runtime_error("表达式运算", "未知二元运算符");
    return ppl_none();
}

PPLValue ppl_unary(const char *op, PPLValue value) {
    if (strcmp(op, "!") == 0) return ppl_bool(!ppl_truthy(value));
    if (strcmp(op, "-") == 0) return ppl_int(-expect_int(value, op));
    ppl_runtime_error("表达式运算", "未知一元运算符");
    return ppl_none();
}

int ppl_is_some_value(PPLValue value) {
    return value.kind == PPL_SOME;
}

int ppl_is_none_value(PPLValue value) {
    return value.kind == PPL_NONE;
}

PPLValue ppl_unwrap_some(PPLValue value) {
    if (value.kind != PPL_SOME) {
        ppl_runtime_error("Option", "尝试解包非 Some 值");
    }
    return *value.as.some_value;
}

size_t ppl_array_len(PPLValue value) {
    if (value.kind != PPL_ARRAY) {
        ppl_runtime_error("数组运行时", "len/index 需要数组");
    }
    return value.as.array_value->length;
}

PPLValue ppl_array_get(PPLValue value, size_t index) {
    if (value.kind != PPL_ARRAY) {
        ppl_runtime_error("数组运行时", "索引目标不是数组");
    }
    if (index >= value.as.array_value->length) {
        ppl_runtime_error("数组运行时", "数组索引越界");
    }
    return value.as.array_value->items[index];
}

PPLValue ppl_index(PPLValue array, PPLValue index) {
    long idx = expect_int(index, "[]");
    if (idx < 0) {
        ppl_runtime_error("数组运行时", "数组索引不能为负数");
    }
    return ppl_array_get(array, (size_t)idx);
}

static void expect_builtin_argc(const char *name, int argc, int expected) {
    if (argc != expected) {
        char buf[160];
        snprintf(buf, sizeof(buf), "%s 参数数量错误：期望 %d，实际 %d", name, expected, argc);
        ppl_runtime_error("内置函数", buf);
    }
}

static PPLArray *expect_array_value(PPLValue value, const char *name) {
    if (value.kind != PPL_ARRAY) {
        char buf[160];
        snprintf(buf, sizeof(buf), "%s 需要数组参数", name);
        ppl_runtime_error("内置函数", buf);
    }
    return value.as.array_value;
}

static PPLValue call_unary_fn(PPLValue fn, PPLValue arg) {
    PPLValue args[1] = {arg};
    return ppl_call_value(fn, 1, args);
}

static PPLValue call_binary_fn(PPLValue fn, PPLValue a, PPLValue b) {
    PPLValue args[2] = {a, b};
    return ppl_call_value(fn, 2, args);
}

static int is_builtin_name(const char *name) {
    static const char *names[] = {
        "is_some", "is_none", "map_opt", "unwrap_or", "and_then",
        "len", "map", "filter", "reduce", "any", "all", NULL
    };
    for (int i = 0; names[i]; i++) {
        if (strcmp(name, names[i]) == 0) return 1;
    }
    return 0;
}

static PPLValue builtin_call(const char *name, int argc, PPLValue *args) {
    if (strcmp(name, "is_some") == 0) {
        expect_builtin_argc(name, argc, 1);
        return ppl_bool(args[0].kind == PPL_SOME);
    }
    if (strcmp(name, "is_none") == 0) {
        expect_builtin_argc(name, argc, 1);
        return ppl_bool(args[0].kind == PPL_NONE);
    }
    if (strcmp(name, "map_opt") == 0) {
        expect_builtin_argc(name, argc, 2);
        if (args[0].kind == PPL_NONE) return ppl_none();
        if (args[0].kind != PPL_SOME) ppl_runtime_error("Option", "map_opt 第一个参数必须是 Option");
        return ppl_some(call_unary_fn(args[1], *args[0].as.some_value));
    }
    if (strcmp(name, "unwrap_or") == 0) {
        expect_builtin_argc(name, argc, 2);
        if (args[0].kind == PPL_NONE) return args[1];
        if (args[0].kind != PPL_SOME) ppl_runtime_error("Option", "unwrap_or 第一个参数必须是 Option");
        return *args[0].as.some_value;
    }
    if (strcmp(name, "and_then") == 0) {
        expect_builtin_argc(name, argc, 2);
        if (args[0].kind == PPL_NONE) return ppl_none();
        if (args[0].kind != PPL_SOME) ppl_runtime_error("Option", "and_then 第一个参数必须是 Option");
        return call_unary_fn(args[1], *args[0].as.some_value);
    }
    if (strcmp(name, "len") == 0) {
        expect_builtin_argc(name, argc, 1);
        return ppl_int((long)ppl_array_len(args[0]));
    }
    if (strcmp(name, "map") == 0) {
        expect_builtin_argc(name, argc, 2);
        PPLArray *array = expect_array_value(args[0], name);
        PPLValue *items = NULL;
        if (array->length > 0) {
            items = (PPLValue *)ppl_xmalloc(sizeof(PPLValue) * array->length);
        }
        for (size_t i = 0; i < array->length; i++) {
            items[i] = call_unary_fn(args[1], array->items[i]);
        }
        return ppl_array((int)array->length, items);
    }
    if (strcmp(name, "filter") == 0) {
        expect_builtin_argc(name, argc, 2);
        PPLArray *array = expect_array_value(args[0], name);
        PPLValue *items = NULL;
        if (array->length > 0) {
            items = (PPLValue *)ppl_xmalloc(sizeof(PPLValue) * array->length);
        }
        int count = 0;
        for (size_t i = 0; i < array->length; i++) {
            if (ppl_truthy(call_unary_fn(args[1], array->items[i]))) {
                items[count++] = array->items[i];
            }
        }
        return ppl_array(count, items);
    }
    if (strcmp(name, "reduce") == 0) {
        expect_builtin_argc(name, argc, 3);
        PPLArray *array = expect_array_value(args[0], name);
        PPLValue acc = args[1];
        for (size_t i = 0; i < array->length; i++) {
            acc = call_binary_fn(args[2], acc, array->items[i]);
        }
        return acc;
    }
    if (strcmp(name, "any") == 0) {
        expect_builtin_argc(name, argc, 2);
        PPLArray *array = expect_array_value(args[0], name);
        for (size_t i = 0; i < array->length; i++) {
            if (ppl_truthy(call_unary_fn(args[1], array->items[i]))) return ppl_bool(1);
        }
        return ppl_bool(0);
    }
    if (strcmp(name, "all") == 0) {
        expect_builtin_argc(name, argc, 2);
        PPLArray *array = expect_array_value(args[0], name);
        for (size_t i = 0; i < array->length; i++) {
            if (!ppl_truthy(call_unary_fn(args[1], array->items[i]))) return ppl_bool(0);
        }
        return ppl_bool(1);
    }
    ppl_runtime_error("内置函数", "未知内置函数");
    return ppl_none();
}
