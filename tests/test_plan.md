# 测试计划

本项目测试通过 `make test` 执行。测试脚本会对每个 fixture 完成同一条链路：

1. 使用 `build/pplc` 将 `.ppl` 输入生成 `generated/test_*.c`。
2. 使用 `gcc` 将生成的 C 文件与 `runtime/*.c` 编译成可执行文件。
3. 运行可执行文件并与 `tests/golden/*.out` 做文本级比对。

覆盖关系：

- `option_match.ppl`：Option 赋值、`is_some`、`is_none`、match、match guard、返回 Option 的查找函数。
- `option_chain.ppl`：`map_opt`、`unwrap_or`、`and_then` 和 `None` 短路传播。
- `functional_closure.ppl`：函数赋值、匿名函数、Lambda、函数返回函数、闭包捕获和闭包独立状态。
- `arrays.ppl`：数组字面量、空数组、索引、`len`、`map`、`filter`、`reduce`、`any`、`all`。
- `recursion_guard.ppl`：递归函数、条件表达式、返回 Option 的局部函数和 match guard。
- `examples/2026_feature_demo.ppl`：按 `2026.docx` 功能清单组合验证全部功能。
