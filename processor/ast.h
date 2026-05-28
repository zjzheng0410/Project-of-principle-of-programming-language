#ifndef PPL_AST_H
#define PPL_AST_H

typedef struct Expr Expr;
typedef struct Stmt Stmt;
typedef struct Block Block;

typedef enum ExprKind {
    EXPR_INT,
    EXPR_BOOL,
    EXPR_VAR,
    EXPR_NONE,
    EXPR_SOME,
    EXPR_ARRAY,
    EXPR_CALL,
    EXPR_INDEX,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_FN,
    EXPR_IF,
    EXPR_MATCH
} ExprKind;

typedef enum StmtKind {
    STMT_LET,
    STMT_ASSIGN,
    STMT_EXPR,
    STMT_RETURN,
    STMT_FOR_EACH,
    STMT_BLOCK
} StmtKind;

typedef struct ExprList {
    Expr **items;
    int count;
    int capacity;
} ExprList;

typedef struct StringList {
    char **items;
    int count;
    int capacity;
} StringList;

typedef struct MatchArm {
    int is_some;
    char *binding;
    Expr *guard;
    Expr *value;
    int line;
} MatchArm;

typedef struct MatchArmList {
    MatchArm *items;
    int count;
    int capacity;
} MatchArmList;

struct Block {
    Stmt **items;
    int count;
    int capacity;
};

struct Expr {
    ExprKind kind;
    int line;
    union {
        long int_value;
        int bool_value;
        char *var_name;
        Expr *some_value;
        ExprList array_items;
        struct {
            Expr *callee;
            ExprList args;
        } call;
        struct {
            Expr *array;
            Expr *index;
        } index;
        struct {
            char *op;
            Expr *left;
            Expr *right;
        } binary;
        struct {
            char *op;
            Expr *expr;
        } unary;
        struct {
            StringList params;
            Block *body;
            int fn_id;
        } fn;
        struct {
            Expr *condition;
            Block *then_block;
            Block *else_block;
        } if_expr;
        struct {
            Expr *target;
            MatchArmList arms;
        } match_expr;
    } as;
};

struct Stmt {
    StmtKind kind;
    int line;
    char *expect;
    union {
        struct {
            char *name;
            Expr *value;
        } let_stmt;
        struct {
            char *name;
            Expr *value;
        } assign_stmt;
        Expr *expr;
        struct {
            char *var;
            Expr *array;
            Block *body;
        } for_each;
        Block *block;
    } as;
};

typedef struct Program {
    Block *body;
    int function_count;
} Program;

char *ast_strdup(const char *text);
Block *block_new(void);
int block_add(Block *block, Stmt *stmt);
Expr *expr_new(ExprKind kind, int line);
Stmt *stmt_new(StmtKind kind, int line);
int expr_list_add(ExprList *list, Expr *expr);
int string_list_add(StringList *list, char *text);
int match_arm_list_add(MatchArmList *list, MatchArm arm);
void program_free(Program *program);

#endif
