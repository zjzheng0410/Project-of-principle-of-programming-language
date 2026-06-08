# 独立行期望注释与隐藏风格测试实施计划

## 目标边界

本次只实现两个目标：

1. 支持独立一行的 `// => ...` 期望注释，并把它关联到上一条已解析语句。
2. 增加一组“改写版隐藏测试风格”的 fixture，验证当前处理程序不是只适配现有示例文本。

不扩展 `2026.docx` 之外的新语言能力，不引入新依赖，不加入兜底解析，不保留临时调试代码，不写冗余测试。

## 必须遵守的实现约束

- 禁止设置兜底逻辑：不能在解析失败后尝试另一套解析路径，不能吞掉错误后继续猜测。
- 禁止冗余代码：不能复制一套独立的语句解析逻辑来专门处理 `// =>`，只能在现有语句解析流程中加入明确规则。
- 禁止硬编码文件名、行号、测试名、固定输出文本。
- 禁止让 `// =>` 后面的说明文本驱动程序输出；输出仍然来自对应语句的实际求值结果。
- 禁止把没有上一条语句的独立 `// =>` 静默忽略；必须报明确语法错误。
- 禁止让连续多个独立 `// =>` 都挂到同一条语句；第二个应报明确语法错误。
- 现有同一行 `expr // => value` 行为必须保持不变。

## 需要修改的文件

- `processor/parser.c`
  - 修改顶层和块内语句解析流程，让独立一行 `TOK_EXPECT` 能绑定到上一条语句。
  - 不修改 lexer 的 token 生成规则，继续由 `lexer.c` 把 `// =>` 识别为 `TOK_EXPECT`。

- `tests/fixtures/hidden_rewrite.ppl`
  - 新增一组隐藏测试风格输入。
  - 该文件使用不同变量名、不同数组、不同阈值、`None` 分支优先、空数组、不同闭包实例、多层 match。
  - 该文件必须包含独立一行 `// =>`，覆盖新注释绑定规则。

- `tests/golden/hidden_rewrite.out`
  - 新增对应期望输出。

- `tests/run_tests.sh`
  - 将 `hidden_rewrite` 加入 fixtures 列表。

- `tests/test_plan.md`
  - 补充新 fixture 的覆盖说明。

- `README.md`
  - 更新输出规则，说明 `// =>` 可写在语句同一行，也可独立写在紧随语句之后的一行。

## 独立行 `// =>` 的解析规则

明确采用单一确定规则：

1. lexer 仍然把 `// => ...` 产出为 `TOK_EXPECT`。
2. parser 解析一条语句后，仍优先支持同一行后紧跟的 `TOK_EXPECT`。
3. 如果解析循环遇到独立的 `TOK_EXPECT`：
   - 若当前 block/program 中没有上一条语句，报错：`期望输出注释缺少关联语句`。
   - 若上一条语句已经有 `expect`，报错：`一条语句不能绑定多个期望输出注释`。
   - 否则把该 token 的文本赋给上一条语句的 `expect` 字段。
4. 绑定完成后消费该 `TOK_EXPECT`，再跳过换行。
5. 该规则同时适用于顶层 program 和 `{ ... }` block 内部。

该方案不是兜底机制，因为它不依赖解析失败后重试；它是在语法层明确定义 `TOK_EXPECT` 作为“上一语句的输出标注”。

## parser 实施步骤

1. 在 `processor/parser.c` 中新增一个小函数，例如 `attach_expect_to_previous(Parser *p, Block *block)`。
   - 输入当前 parser 和当前 block。
   - 只处理当前位置为 `TOK_EXPECT` 的情况。
   - 通过 `block->count` 找上一条语句。
   - 成功时复制 token 文本到上一条语句的 `expect`。
   - 失败时调用 `diag_error`，不静默继续。

2. 调整 `parse_block` 的循环。
   - 在循环开头 `skip_newlines` 后，如果当前 token 是 `TOK_EXPECT`，调用 `attach_expect_to_previous`。
   - 否则正常 `parse_stmt`。
   - 不复制 `parse_stmt` 主体逻辑。

3. 调整 `parse_program` 的顶层循环。
   - 使用同样的 `TOK_EXPECT` 绑定逻辑。
   - 避免只支持块内、不支持顶层。

4. 保留 `parse_stmt` 中现有的同一行 `TOK_EXPECT` 处理。
   - 同一行注释仍由语句解析完成后直接绑定。
   - 独立行注释由 block/program 循环绑定。

5. 错误处理要求。
   - 文件开头出现 `// => ...`：语法错误。
   - 空块内出现 `// => ...`：语法错误。
   - 连续两个 `// => ...`：第二个语法错误。
   - 不能把错误注释当作普通表达式解析。

## 隐藏风格 fixture 设计

新增 `tests/fixtures/hidden_rewrite.ppl`，只做一组综合改写测试，不拆成多个重复 fixture。

必须覆盖以下场景：

- 换变量名：避免只适配 `x`、`arr`、`target`、`base`、`c1` 等现有名字。
- 换数组内容：使用不同长度、不同顺序、不同结果。
- 换阈值：例如查找大于 `6` 的第一个元素，而不是大于 `3`。
- `None` 分支优先：例如 `match None { None => 0 Some(v) => v }`。
- 空数组：例如 `filter([], fn n => n > 0)`、`reduce([], 100, fn acc, n => acc + n)`。
- 不同闭包实例：创建两个计数器或累加器，交错调用。
- 多层 match：外层匹配 `Some(...)`，内层再匹配 `None` 或另一个 `Some(...)`。
- 独立行 `// =>`：至少 5 条输出使用下一行注释形式。

建议测试点和期望输出：

1. `match None { None => 0 Some(v) => v }` 输出 `0`，验证 `None` 分支优先。
2. 改写版查找函数 `first_above([2, 4, 9, 7], 6)` 输出 `Some(9)`。
3. `first_above([2, 4, 5], 6)` 输出 `None`。
4. `map([4, 1, 3], fn n => n + 2)` 输出 `[6, 3, 5]`。
5. `filter([], fn n => n > 0)` 输出 `[]`。
6. `reduce([], 100, fn acc, n => acc + n)` 输出 `100`。
7. 两个闭包实例交错调用，输出能证明状态独立，例如 `6`、`11`、`7`。
8. 多层 match 输出一个非原示例值，例如 `42`。
9. match guard 使用不同阈值，例如 `Some(12) if score > 10 => 1` 输出 `1`。

## 期望输出文件

`tests/golden/hidden_rewrite.out` 必须只包含程序实际打印值，每行一个输出，不包含 `// =>` 后的说明文本。

输出顺序必须与 fixture 中带 `// =>` 的语句顺序一致。

## 验证命令

实现后必须运行：

```bash
make -B test
make demo
```

额外手工验证：

```bash
./build/pplc tests/fixtures/hidden_rewrite.ppl -o generated/test_hidden_rewrite.c --dump-tokens
gcc -Iruntime -std=c11 -Wall -Wextra -pedantic -g -o build/test_hidden_rewrite.bin generated/test_hidden_rewrite.c runtime/*.c
./build/test_hidden_rewrite.bin
diff -u tests/golden/hidden_rewrite.out build/test_hidden_rewrite.out
```

手工验证中的 `generated/test_hidden_rewrite.c`、`build/test_hidden_rewrite.bin`、`build/test_hidden_rewrite.out` 都是测试生成物，不提交为核心代码。

## 验收标准

- `make -B test` 必须通过所有旧 fixture 和新增 `hidden_rewrite` fixture。
- `make demo` 输出必须保持不变。
- 同一行 `// =>` 和独立下一行 `// =>` 都能触发打印。
- 没有关联语句的 `// =>` 必须报语法错误。
- 连续重复 `// =>` 必须报语法错误。
- 处理程序仍然根据 AST 生成 C，不按文件名、测试名、行号或期望文本生成固定输出。
- 新增代码不包含临时调试输出、重复解析分支、兜底重试路径或无意义注释。
