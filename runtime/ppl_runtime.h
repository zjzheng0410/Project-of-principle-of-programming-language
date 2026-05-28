#ifndef PPL_RUNTIME_H
#define PPL_RUNTIME_H

#include <stddef.h>

typedef struct PPLValue PPLValue;
typedef struct PPLArray PPLArray;
typedef struct PPLFunc PPLFunc;
typedef struct PPLEnv PPLEnv;

typedef PPLValue (*PPLNativeFn)(PPLFunc *self, int argc, PPLValue *args);

typedef enum PPLKind {
    PPL_INT,
    PPL_BOOL,
    PPL_NONE,
    PPL_SOME,
    PPL_ARRAY,
    PPL_FUNCTION
} PPLKind;

struct PPLValue {
    PPLKind kind;
    union {
        long int_value;
        int bool_value;
        PPLValue *some_value;
        PPLArray *array_value;
        PPLFunc *function_value;
    } as;
};

struct PPLArray {
    size_t length;
    size_t capacity;
    PPLValue *items;
};

struct PPLFunc {
    const char *name;
    PPLNativeFn call;
    PPLEnv *env;
};

PPLValue ppl_int(long value);
PPLValue ppl_bool(int value);
PPLValue ppl_none(void);
PPLValue ppl_some(PPLValue value);
PPLValue ppl_array(int count, PPLValue *items);
PPLValue ppl_make_function(const char *name, PPLNativeFn call, PPLEnv *env);

void ppl_print_value(PPLValue value);
void ppl_runtime_error(const char *stage, const char *message);
void ppl_expect_argc(const char *name, int argc, int expected);
int ppl_truthy(PPLValue value);

PPLEnv *ppl_env_new(PPLEnv *parent);
void ppl_env_define(PPLEnv *env, const char *name, PPLValue value);
PPLValue ppl_env_get(PPLEnv *env, const char *name);
void ppl_env_assign(PPLEnv *env, const char *name, PPLValue value);

PPLValue ppl_call_name(PPLEnv *env, const char *name, int argc, PPLValue *args);
PPLValue ppl_call_value(PPLValue callee, int argc, PPLValue *args);

PPLValue ppl_binary(const char *op, PPLValue left, PPLValue right);
PPLValue ppl_unary(const char *op, PPLValue value);

int ppl_is_some_value(PPLValue value);
int ppl_is_none_value(PPLValue value);
PPLValue ppl_unwrap_some(PPLValue value);

size_t ppl_array_len(PPLValue value);
PPLValue ppl_array_get(PPLValue value, size_t index);
PPLValue ppl_index(PPLValue array, PPLValue index);

#endif
