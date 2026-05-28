#include <stdio.h>
#include "ppl_runtime.h"

static PPLValue ppl_fn_1(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_2(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_3(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_4(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_5(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_6(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_7(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_8(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_9(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_10(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_11(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_12(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_13(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_14(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_15(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_16(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_17(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_18(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_19(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_20(PPLFunc *self, int argc, PPLValue *args);
static PPLValue ppl_fn_21(PPLFunc *self, int argc, PPLValue *args);

int main(void) {
    PPLEnv *env = ppl_env_new(NULL);
    PPLValue _v2 = ppl_int(10);
    PPLValue _v1 = ppl_some(_v2);
    ppl_env_define(env, "opt_value", _v1);
    PPLValue _v3 = ppl_none();
    ppl_env_define(env, "empty_value", _v3);
    PPLValue _v6 = ppl_env_get(env, "opt_value");
    PPLValue _args5[1] = {_v6};
    PPLValue _v4 = ppl_call_name(env, "is_some", 1, _args5);
    ppl_print_value(_v4);
    printf("\n");
    PPLValue _v9 = ppl_env_get(env, "empty_value");
    PPLValue _args8[1] = {_v9};
    PPLValue _v7 = ppl_call_name(env, "is_none", 1, _args8);
    ppl_print_value(_v7);
    printf("\n");
    PPLValue _v11 = ppl_env_get(env, "opt_value");
    PPLValue _v10;
    int _matched1 = 0;
    if (!_matched1 && ppl_is_some_value(_v11)) {
        PPLEnv *_prev_env1_0 = env;
        env = ppl_env_new(env);
        ppl_env_define(env, "x", ppl_unwrap_some(_v11));
        PPLValue _v13 = ppl_env_get(env, "x");
        PPLValue _v14 = ppl_int(1);
        PPLValue _v12 = ppl_binary("+", _v13, _v14);
        _v10 = _v12;
        _matched1 = 1;
        env = _prev_env1_0;
    }
    if (!_matched1 && ppl_is_none_value(_v11)) {
        PPLEnv *_prev_env1_1 = env;
        env = ppl_env_new(env);
        PPLValue _v15 = ppl_int(0);
        _v10 = _v15;
        _matched1 = 1;
        env = _prev_env1_1;
    }
    if (!_matched1) { ppl_runtime_error("match", "没有匹配分支"); }
    ppl_env_define(env, "matched", _v10);
    PPLValue _v16 = ppl_env_get(env, "matched");
    ppl_print_value(_v16);
    printf("\n");
    PPLValue _v17 = ppl_make_function("fn_1", ppl_fn_1, env);
    ppl_env_define(env, "find", _v17);
    PPLValue _v21 = ppl_int(1);
    PPLValue _v22 = ppl_int(3);
    PPLValue _v23 = ppl_int(5);
    PPLValue _v24 = ppl_int(2);
    PPLValue _v25 = ppl_int(4);
    PPLValue _items26[5] = {_v21, _v22, _v23, _v24, _v25};
    PPLValue _v20 = ppl_array(5, _items26);
    PPLValue _v27 = ppl_int(3);
    PPLValue _args19[2] = {_v20, _v27};
    PPLValue _v18 = ppl_call_name(env, "find", 2, _args19);
    ppl_print_value(_v18);
    printf("\n");
    PPLValue _v31 = ppl_int(1);
    PPLValue _v32 = ppl_int(2);
    PPLValue _v33 = ppl_int(3);
    PPLValue _items34[3] = {_v31, _v32, _v33};
    PPLValue _v30 = ppl_array(3, _items34);
    PPLValue _v35 = ppl_int(5);
    PPLValue _args29[2] = {_v30, _v35};
    PPLValue _v28 = ppl_call_name(env, "find", 2, _args29);
    ppl_print_value(_v28);
    printf("\n");
    PPLValue _v36 = ppl_make_function("fn_2", ppl_fn_2, env);
    ppl_env_define(env, "double", _v36);
    PPLValue _v39 = ppl_int(10);
    PPLValue _args38[1] = {_v39};
    PPLValue _v37 = ppl_call_name(env, "double", 1, _args38);
    ppl_print_value(_v37);
    printf("\n");
    PPLValue _v40 = ppl_make_function("fn_3", ppl_fn_3, env);
    ppl_env_define(env, "add", _v40);
    PPLValue _v43 = ppl_int(4);
    PPLValue _args42[1] = {_v43};
    PPLValue _v46 = ppl_int(3);
    PPLValue _args45[1] = {_v46};
    PPLValue _v44 = ppl_call_name(env, "add", 1, _args45);
    PPLValue _v41 = ppl_call_value(_v44, 1, _args42);
    ppl_print_value(_v41);
    printf("\n");
    PPLValue _v47 = ppl_make_function("fn_5", ppl_fn_5, env);
    ppl_env_define(env, "double_lambda", _v47);
    PPLValue _v50 = ppl_int(10);
    PPLValue _args49[1] = {_v50};
    PPLValue _v48 = ppl_call_name(env, "double_lambda", 1, _args49);
    ppl_print_value(_v48);
    printf("\n");
    PPLValue _v51 = ppl_make_function("fn_6", ppl_fn_6, env);
    ppl_env_define(env, "add_lambda", _v51);
    PPLValue _v54 = ppl_int(4);
    PPLValue _args53[1] = {_v54};
    PPLValue _v57 = ppl_int(3);
    PPLValue _args56[1] = {_v57};
    PPLValue _v55 = ppl_call_name(env, "add_lambda", 1, _args56);
    PPLValue _v52 = ppl_call_value(_v55, 1, _args53);
    ppl_print_value(_v52);
    printf("\n");
    PPLValue _v61 = ppl_int(10);
    PPLValue _v60 = ppl_some(_v61);
    PPLValue _v62 = ppl_make_function("fn_8", ppl_fn_8, env);
    PPLValue _args59[2] = {_v60, _v62};
    PPLValue _v58 = ppl_call_name(env, "map_opt", 2, _args59);
    ppl_print_value(_v58);
    printf("\n");
    PPLValue _v66 = ppl_int(8);
    PPLValue _v65 = ppl_some(_v66);
    PPLValue _v67 = ppl_int(0);
    PPLValue _args64[2] = {_v65, _v67};
    PPLValue _v63 = ppl_call_name(env, "unwrap_or", 2, _args64);
    ppl_print_value(_v63);
    printf("\n");
    PPLValue _v70 = ppl_none();
    PPLValue _v71 = ppl_int(0);
    PPLValue _args69[2] = {_v70, _v71};
    PPLValue _v68 = ppl_call_name(env, "unwrap_or", 2, _args69);
    ppl_print_value(_v68);
    printf("\n");
    PPLValue _v72 = ppl_make_function("fn_9", ppl_fn_9, env);
    ppl_env_define(env, "safe_div", _v72);
    PPLValue _v77 = ppl_int(20);
    PPLValue _v78 = ppl_int(2);
    PPLValue _args76[2] = {_v77, _v78};
    PPLValue _v75 = ppl_call_name(env, "safe_div", 2, _args76);
    PPLValue _v79 = ppl_make_function("fn_10", ppl_fn_10, env);
    PPLValue _args74[2] = {_v75, _v79};
    PPLValue _v73 = ppl_call_name(env, "and_then", 2, _args74);
    ppl_print_value(_v73);
    printf("\n");
    PPLValue _v84 = ppl_int(20);
    PPLValue _v85 = ppl_int(0);
    PPLValue _args83[2] = {_v84, _v85};
    PPLValue _v82 = ppl_call_name(env, "safe_div", 2, _args83);
    PPLValue _v86 = ppl_make_function("fn_11", ppl_fn_11, env);
    PPLValue _args81[2] = {_v82, _v86};
    PPLValue _v80 = ppl_call_name(env, "and_then", 2, _args81);
    ppl_print_value(_v80);
    printf("\n");
    PPLValue _v89 = ppl_array(0, NULL);
    PPLValue _args88[1] = {_v89};
    PPLValue _v87 = ppl_call_name(env, "len", 1, _args88);
    ppl_print_value(_v87);
    printf("\n");
    PPLValue _v92 = ppl_int(9);
    PPLValue _v93 = ppl_int(8);
    PPLValue _v94 = ppl_int(7);
    PPLValue _items95[3] = {_v92, _v93, _v94};
    PPLValue _v91 = ppl_array(3, _items95);
    PPLValue _v96 = ppl_int(1);
    PPLValue _v90 = ppl_index(_v91, _v96);
    ppl_print_value(_v90);
    printf("\n");
    PPLValue _v100 = ppl_int(1);
    PPLValue _v101 = ppl_int(2);
    PPLValue _v102 = ppl_int(3);
    PPLValue _v103 = ppl_int(4);
    PPLValue _items104[4] = {_v100, _v101, _v102, _v103};
    PPLValue _v99 = ppl_array(4, _items104);
    PPLValue _v105 = ppl_make_function("fn_12", ppl_fn_12, env);
    PPLValue _args98[2] = {_v99, _v105};
    PPLValue _v97 = ppl_call_name(env, "map", 2, _args98);
    ppl_print_value(_v97);
    printf("\n");
    PPLValue _v109 = ppl_int(1);
    PPLValue _v110 = ppl_int(3);
    PPLValue _v111 = ppl_int(5);
    PPLValue _v112 = ppl_int(2);
    PPLValue _v113 = ppl_int(4);
    PPLValue _items114[5] = {_v109, _v110, _v111, _v112, _v113};
    PPLValue _v108 = ppl_array(5, _items114);
    PPLValue _v115 = ppl_make_function("fn_13", ppl_fn_13, env);
    PPLValue _args107[2] = {_v108, _v115};
    PPLValue _v106 = ppl_call_name(env, "filter", 2, _args107);
    ppl_print_value(_v106);
    printf("\n");
    PPLValue _v119 = ppl_int(1);
    PPLValue _v120 = ppl_int(2);
    PPLValue _v121 = ppl_int(3);
    PPLValue _v122 = ppl_int(4);
    PPLValue _items123[4] = {_v119, _v120, _v121, _v122};
    PPLValue _v118 = ppl_array(4, _items123);
    PPLValue _v124 = ppl_int(0);
    PPLValue _v125 = ppl_make_function("fn_14", ppl_fn_14, env);
    PPLValue _args117[3] = {_v118, _v124, _v125};
    PPLValue _v116 = ppl_call_name(env, "reduce", 3, _args117);
    ppl_print_value(_v116);
    printf("\n");
    PPLValue _v129 = ppl_int(1);
    PPLValue _v130 = ppl_int(2);
    PPLValue _v131 = ppl_int(3);
    PPLValue _items132[3] = {_v129, _v130, _v131};
    PPLValue _v128 = ppl_array(3, _items132);
    PPLValue _v133 = ppl_make_function("fn_15", ppl_fn_15, env);
    PPLValue _args127[2] = {_v128, _v133};
    PPLValue _v126 = ppl_call_name(env, "any", 2, _args127);
    ppl_print_value(_v126);
    printf("\n");
    PPLValue _v137 = ppl_int(2);
    PPLValue _v138 = ppl_int(4);
    PPLValue _v139 = ppl_int(6);
    PPLValue _items140[3] = {_v137, _v138, _v139};
    PPLValue _v136 = ppl_array(3, _items140);
    PPLValue _v141 = ppl_make_function("fn_16", ppl_fn_16, env);
    PPLValue _args135[2] = {_v136, _v141};
    PPLValue _v134 = ppl_call_name(env, "all", 2, _args135);
    ppl_print_value(_v134);
    printf("\n");
    PPLValue _v142 = ppl_int(10);
    ppl_env_define(env, "scoped", _v142);
    PPLEnv *_prev_env2 = env;
    env = ppl_env_new(env);
    PPLValue _v143 = ppl_int(20);
    ppl_env_define(env, "scoped", _v143);
    PPLValue _v144 = ppl_env_get(env, "scoped");
    ppl_print_value(_v144);
    printf("\n");
    env = _prev_env2;
    (void)_v144;
    PPLValue _v145 = ppl_env_get(env, "scoped");
    ppl_print_value(_v145);
    printf("\n");
    PPLValue _v146 = ppl_int(100);
    ppl_env_define(env, "base", _v146);
    PPLValue _v147 = ppl_make_function("fn_17", ppl_fn_17, env);
    ppl_env_define(env, "add_base", _v147);
    PPLValue _v150 = ppl_int(23);
    PPLValue _args149[1] = {_v150};
    PPLValue _v148 = ppl_call_name(env, "add_base", 1, _args149);
    ppl_print_value(_v148);
    printf("\n");
    PPLValue _v151 = ppl_make_function("fn_18", ppl_fn_18, env);
    ppl_env_define(env, "make_counter", _v151);
    PPLValue _v154 = ppl_int(0);
    PPLValue _args153[1] = {_v154};
    PPLValue _v152 = ppl_call_name(env, "make_counter", 1, _args153);
    ppl_env_define(env, "c1", _v152);
    PPLValue *_args156 = NULL;
    PPLValue _v155 = ppl_call_name(env, "c1", 0, _args156);
    ppl_print_value(_v155);
    printf("\n");
    PPLValue *_args158 = NULL;
    PPLValue _v157 = ppl_call_name(env, "c1", 0, _args158);
    ppl_print_value(_v157);
    printf("\n");
    PPLValue _v161 = ppl_int(10);
    PPLValue _args160[1] = {_v161};
    PPLValue _v159 = ppl_call_name(env, "make_counter", 1, _args160);
    ppl_env_define(env, "c2", _v159);
    PPLValue *_args163 = NULL;
    PPLValue _v162 = ppl_call_name(env, "c2", 0, _args163);
    ppl_print_value(_v162);
    printf("\n");
    PPLValue *_args165 = NULL;
    PPLValue _v164 = ppl_call_name(env, "c1", 0, _args165);
    ppl_print_value(_v164);
    printf("\n");
    PPLValue _v166 = ppl_make_function("fn_20", ppl_fn_20, env);
    ppl_env_define(env, "fact", _v166);
    PPLValue _v169 = ppl_int(5);
    PPLValue _args168[1] = {_v169};
    PPLValue _v167 = ppl_call_name(env, "fact", 1, _args168);
    ppl_print_value(_v167);
    printf("\n");
    PPLValue _v172 = ppl_int(75);
    PPLValue _v171 = ppl_some(_v172);
    PPLValue _v170;
    int _matched3 = 0;
    if (!_matched3 && ppl_is_some_value(_v171)) {
        PPLEnv *_prev_env3_0 = env;
        env = ppl_env_new(env);
        ppl_env_define(env, "x", ppl_unwrap_some(_v171));
        PPLValue _v174 = ppl_env_get(env, "x");
        PPLValue _v175 = ppl_int(60);
        PPLValue _v173 = ppl_binary(">=", _v174, _v175);
        if (ppl_truthy(_v173)) {
            PPLValue _v176 = ppl_int(1);
            _v170 = _v176;
            _matched3 = 1;
        }
        env = _prev_env3_0;
    }
    if (!_matched3 && ppl_is_some_value(_v171)) {
        PPLEnv *_prev_env3_1 = env;
        env = ppl_env_new(env);
        ppl_env_define(env, "x", ppl_unwrap_some(_v171));
        PPLValue _v177 = ppl_int(0);
        _v170 = _v177;
        _matched3 = 1;
        env = _prev_env3_1;
    }
    if (!_matched3 && ppl_is_none_value(_v171)) {
        PPLEnv *_prev_env3_2 = env;
        env = ppl_env_new(env);
        PPLValue _v179 = ppl_int(1);
        PPLValue _v178 = ppl_unary("-", _v179);
        _v170 = _v178;
        _matched3 = 1;
        env = _prev_env3_2;
    }
    if (!_matched3) { ppl_runtime_error("match", "没有匹配分支"); }
    ppl_print_value(_v170);
    printf("\n");
    PPLValue _v180 = ppl_make_function("fn_21", ppl_fn_21, env);
    ppl_env_define(env, "max_opt", _v180);
    PPLValue _v184 = ppl_int(3);
    PPLValue _v185 = ppl_int(9);
    PPLValue _v186 = ppl_int(2);
    PPLValue _v187 = ppl_int(5);
    PPLValue _items188[4] = {_v184, _v185, _v186, _v187};
    PPLValue _v183 = ppl_array(4, _items188);
    PPLValue _args182[1] = {_v183};
    PPLValue _v181 = ppl_call_name(env, "max_opt", 1, _args182);
    ppl_print_value(_v181);
    printf("\n");
    PPLValue _v191 = ppl_array(0, NULL);
    PPLValue _args190[1] = {_v191};
    PPLValue _v189 = ppl_call_name(env, "max_opt", 1, _args190);
    ppl_print_value(_v189);
    printf("\n");
    return 0;
}

static PPLValue ppl_fn_1(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_1", argc, 2);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "arr", args[0]);
    ppl_env_define(env, "target", args[1]);
    PPLValue _v192 = ppl_env_get(env, "arr");
    for (size_t _i4 = 0; _i4 < ppl_array_len(_v192); _i4++) {
        PPLEnv *_prev_env4 = env;
        env = ppl_env_new(env);
        ppl_env_define(env, "x", ppl_array_get(_v192, _i4));
        PPLValue _v195 = ppl_env_get(env, "x");
        PPLValue _v196 = ppl_env_get(env, "target");
        PPLValue _v194 = ppl_binary(">", _v195, _v196);
        PPLValue _v193;
        if (ppl_truthy(_v194)) {
            PPLEnv *_prev_env5 = env;
            env = ppl_env_new(env);
            PPLValue _v198 = ppl_env_get(env, "x");
            PPLValue _v197 = ppl_some(_v198);
            return _v197;
            PPLValue _v199 = ppl_none();
            env = _prev_env5;
            _v193 = _v199;
        } else {
            _v193 = ppl_none();
        }
        (void)_v193;
        env = _prev_env4;
    }
    PPLValue _v200 = ppl_none();
    return _v200;
    return ppl_none();
}

static PPLValue ppl_fn_2(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_2", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "x", args[0]);
    PPLValue _v202 = ppl_env_get(env, "x");
    PPLValue _v203 = ppl_int(2);
    PPLValue _v201 = ppl_binary("*", _v202, _v203);
    return _v201;
    return ppl_none();
}

static PPLValue ppl_fn_4(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_4", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "b", args[0]);
    PPLValue _v205 = ppl_env_get(env, "a");
    PPLValue _v206 = ppl_env_get(env, "b");
    PPLValue _v204 = ppl_binary("+", _v205, _v206);
    return _v204;
    return ppl_none();
}

static PPLValue ppl_fn_3(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_3", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "a", args[0]);
    PPLValue _v207 = ppl_make_function("fn_4", ppl_fn_4, env);
    return _v207;
    return ppl_none();
}

static PPLValue ppl_fn_5(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_5", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "x", args[0]);
    PPLValue _v209 = ppl_env_get(env, "x");
    PPLValue _v210 = ppl_int(2);
    PPLValue _v208 = ppl_binary("*", _v209, _v210);
    return _v208;
    return ppl_none();
}

static PPLValue ppl_fn_7(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_7", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "b", args[0]);
    PPLValue _v212 = ppl_env_get(env, "a");
    PPLValue _v213 = ppl_env_get(env, "b");
    PPLValue _v211 = ppl_binary("+", _v212, _v213);
    return _v211;
    return ppl_none();
}

static PPLValue ppl_fn_6(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_6", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "a", args[0]);
    PPLValue _v214 = ppl_make_function("fn_7", ppl_fn_7, env);
    return _v214;
    return ppl_none();
}

static PPLValue ppl_fn_8(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_8", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "x", args[0]);
    PPLValue _v216 = ppl_env_get(env, "x");
    PPLValue _v217 = ppl_int(2);
    PPLValue _v215 = ppl_binary("*", _v216, _v217);
    return _v215;
    return ppl_none();
}

static PPLValue ppl_fn_9(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_9", argc, 2);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "a", args[0]);
    ppl_env_define(env, "b", args[1]);
    PPLValue _v220 = ppl_env_get(env, "b");
    PPLValue _v221 = ppl_int(0);
    PPLValue _v219 = ppl_binary("==", _v220, _v221);
    PPLValue _v218;
    if (ppl_truthy(_v219)) {
        PPLEnv *_prev_env6 = env;
        env = ppl_env_new(env);
        PPLValue _v222 = ppl_none();
        return _v222;
        PPLValue _v223 = ppl_none();
        env = _prev_env6;
        _v218 = _v223;
    } else {
        _v218 = ppl_none();
    }
    (void)_v218;
    PPLValue _v226 = ppl_env_get(env, "a");
    PPLValue _v227 = ppl_env_get(env, "b");
    PPLValue _v225 = ppl_binary("/", _v226, _v227);
    PPLValue _v224 = ppl_some(_v225);
    return _v224;
    return ppl_none();
}

static PPLValue ppl_fn_10(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_10", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "x", args[0]);
    PPLValue _v230 = ppl_env_get(env, "x");
    PPLValue _v231 = ppl_int(5);
    PPLValue _args229[2] = {_v230, _v231};
    PPLValue _v228 = ppl_call_name(env, "safe_div", 2, _args229);
    return _v228;
    return ppl_none();
}

static PPLValue ppl_fn_11(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_11", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "x", args[0]);
    PPLValue _v234 = ppl_env_get(env, "x");
    PPLValue _v235 = ppl_int(5);
    PPLValue _args233[2] = {_v234, _v235};
    PPLValue _v232 = ppl_call_name(env, "safe_div", 2, _args233);
    return _v232;
    return ppl_none();
}

static PPLValue ppl_fn_12(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_12", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "x", args[0]);
    PPLValue _v237 = ppl_env_get(env, "x");
    PPLValue _v238 = ppl_env_get(env, "x");
    PPLValue _v236 = ppl_binary("*", _v237, _v238);
    return _v236;
    return ppl_none();
}

static PPLValue ppl_fn_13(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_13", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "x", args[0]);
    PPLValue _v240 = ppl_env_get(env, "x");
    PPLValue _v241 = ppl_int(3);
    PPLValue _v239 = ppl_binary(">", _v240, _v241);
    return _v239;
    return ppl_none();
}

static PPLValue ppl_fn_14(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_14", argc, 2);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "acc", args[0]);
    ppl_env_define(env, "x", args[1]);
    PPLValue _v243 = ppl_env_get(env, "acc");
    PPLValue _v244 = ppl_env_get(env, "x");
    PPLValue _v242 = ppl_binary("+", _v243, _v244);
    return _v242;
    return ppl_none();
}

static PPLValue ppl_fn_15(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_15", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "x", args[0]);
    PPLValue _v246 = ppl_env_get(env, "x");
    PPLValue _v247 = ppl_int(2);
    PPLValue _v245 = ppl_binary("==", _v246, _v247);
    return _v245;
    return ppl_none();
}

static PPLValue ppl_fn_16(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_16", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "x", args[0]);
    PPLValue _v250 = ppl_env_get(env, "x");
    PPLValue _v251 = ppl_int(2);
    PPLValue _v249 = ppl_binary("%", _v250, _v251);
    PPLValue _v252 = ppl_int(0);
    PPLValue _v248 = ppl_binary("==", _v249, _v252);
    return _v248;
    return ppl_none();
}

static PPLValue ppl_fn_17(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_17", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "x", args[0]);
    PPLValue _v254 = ppl_env_get(env, "base");
    PPLValue _v255 = ppl_env_get(env, "x");
    PPLValue _v253 = ppl_binary("+", _v254, _v255);
    return _v253;
    return ppl_none();
}

static PPLValue ppl_fn_19(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_19", argc, 0);
    (void)args;
    PPLEnv *env = ppl_env_new(self->env);
    PPLValue _v257 = ppl_env_get(env, "n");
    PPLValue _v258 = ppl_int(1);
    PPLValue _v256 = ppl_binary("+", _v257, _v258);
    ppl_env_assign(env, "n", _v256);
    PPLValue _v259 = ppl_env_get(env, "n");
    return _v259;
    return ppl_none();
}

static PPLValue ppl_fn_18(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_18", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "start", args[0]);
    PPLValue _v260 = ppl_env_get(env, "start");
    ppl_env_define(env, "n", _v260);
    PPLValue _v261 = ppl_make_function("fn_19", ppl_fn_19, env);
    return _v261;
    return ppl_none();
}

static PPLValue ppl_fn_20(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_20", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "n", args[0]);
    PPLValue _v264 = ppl_env_get(env, "n");
    PPLValue _v265 = ppl_int(1);
    PPLValue _v263 = ppl_binary("<=", _v264, _v265);
    PPLValue _v262;
    if (ppl_truthy(_v263)) {
        PPLEnv *_prev_env7 = env;
        env = ppl_env_new(env);
        PPLValue _v266 = ppl_int(1);
        env = _prev_env7;
        _v262 = _v266;
    } else {
        PPLEnv *_prev_env8 = env;
        env = ppl_env_new(env);
        PPLValue _v268 = ppl_env_get(env, "n");
        PPLValue _v272 = ppl_env_get(env, "n");
        PPLValue _v273 = ppl_int(1);
        PPLValue _v271 = ppl_binary("-", _v272, _v273);
        PPLValue _args270[1] = {_v271};
        PPLValue _v269 = ppl_call_name(env, "fact", 1, _args270);
        PPLValue _v267 = ppl_binary("*", _v268, _v269);
        env = _prev_env8;
        _v262 = _v267;
    }
    return _v262;
    return ppl_none();
}

static PPLValue ppl_fn_21(PPLFunc *self, int argc, PPLValue *args) {
    ppl_expect_argc("fn_21", argc, 1);
    PPLEnv *env = ppl_env_new(self->env);
    ppl_env_define(env, "arr", args[0]);
    PPLValue _v278 = ppl_env_get(env, "arr");
    PPLValue _args277[1] = {_v278};
    PPLValue _v276 = ppl_call_name(env, "len", 1, _args277);
    PPLValue _v279 = ppl_int(0);
    PPLValue _v275 = ppl_binary("==", _v276, _v279);
    PPLValue _v274;
    if (ppl_truthy(_v275)) {
        PPLEnv *_prev_env9 = env;
        env = ppl_env_new(env);
        PPLValue _v280 = ppl_none();
        return _v280;
        PPLValue _v281 = ppl_none();
        env = _prev_env9;
        _v274 = _v281;
    } else {
        _v274 = ppl_none();
    }
    (void)_v274;
    PPLValue _v283 = ppl_env_get(env, "arr");
    PPLValue _v284 = ppl_int(0);
    PPLValue _v282 = ppl_index(_v283, _v284);
    ppl_env_define(env, "best", _v282);
    PPLValue _v285 = ppl_env_get(env, "arr");
    for (size_t _i10 = 0; _i10 < ppl_array_len(_v285); _i10++) {
        PPLEnv *_prev_env10 = env;
        env = ppl_env_new(env);
        ppl_env_define(env, "x", ppl_array_get(_v285, _i10));
        PPLValue _v288 = ppl_env_get(env, "x");
        PPLValue _v289 = ppl_env_get(env, "best");
        PPLValue _v287 = ppl_binary(">", _v288, _v289);
        PPLValue _v286;
        if (ppl_truthy(_v287)) {
            PPLEnv *_prev_env11 = env;
            env = ppl_env_new(env);
            PPLValue _v290 = ppl_env_get(env, "x");
            ppl_env_assign(env, "best", _v290);
            PPLValue _v291 = ppl_none();
            env = _prev_env11;
            _v286 = _v291;
        } else {
            _v286 = ppl_none();
        }
        (void)_v286;
        env = _prev_env10;
    }
    PPLValue _v293 = ppl_env_get(env, "best");
    PPLValue _v292 = ppl_some(_v293);
    return _v292;
    return ppl_none();
}

