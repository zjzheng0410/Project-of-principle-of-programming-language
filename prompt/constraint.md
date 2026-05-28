# Constraints

- Encoding rule: when writing or modifying `Markdown/Python/Chinese comments`, always use `UTF-8` without BOM + `LF`, and prefer `apply_patch`.
- PowerShell writing rule: do not use the default `Set-Content` or `Add-Content`; use `[System.IO.File]::WriteAllText(..., New-Object System.Text.UTF8Encoding($false))` instead. Appending comments is only allowed with `Add-Content -Encoding UTF8`.

## Basic Principles

- If the goal is unclear, the requirement is ambiguous, or the input is insufficient, you must ask for confirmation first. Do not make assumptions on your own.
- Do not modify any existing code unless explicitly requested.
- Before starting analysis, design, or coding, you must first fully inspect the project code and directory structure to understand the current path, module relationships, and dependencies. The project directory is `/mnt/d/coding/job/kf/backup/rl_train_construction/RL`.
-  The project environment is Linux and python 3.8.
- Do not rely on subjective guesses. For uncertain APIs, library usage, or parameter behavior, check official or reliable sources before using them.
- Do not design fallback mechanisms. Do not secretly add degradation logic, default error swallowing, fallback branches, or implicit fault tolerance just to make the code “look more robust.”
- Do not introduce any secondary issues, cascading issues, or hidden risks. In particular, proactively check concurrency, shared state, lock contention, resource release, cache pollution, path side effects, compatibility breakage, and related risks.
- All errors should be raised directly with concise, clear, and locatable error messages. Do not fail silently.
- Important logic, key boundaries, and error-prone parts should include Chinese comments. The comments must be readable and free of mojibake.

## Coding Standards

- Prefer reusing the existing project structure, naming style, and module boundaries. Do not refactor casually. Prefer paragraph-level replacement.
- Do not introduce additional dependencies unless clearly necessary and the reason has been explained.
- Do not modify unrelated files. Do not fix unrelated issues along the way.
- Do not keep deprecated code, temporary debugging code, or meaningless comments.
- Newly added code should be as small and clear as possible. Avoid overengineering.

## Error Handling

- Do not use `except: pass`.
- Do not swallow exceptions without notification.
- Do not use vague fallback logic to hide real problems.
- Error messages should be short and should indicate which input, which step, and which type of error is involved.

## Analysis Standards

- Confirm the requirement boundaries first, then propose a modification plan.
- Inspect the existing implementation first, then discuss optimization or rewriting.
- Conclusions must be based on code, logs, documentation, or reliable sources. Do not make judgments based on intuition alone.
- When making changes, also evaluate whether secondary impacts may be introduced, especially regarding concurrency safety, state consistency, call-chain compatibility, upstream and downstream side effects, and constraints from existing data structures.
- If the user’s request conflicts with the current reality of the project, clearly point out the conflict.

## Output Requirements

- First explain your understanding of the task and the affected scope.
- If the information is insufficient, list the questions that must be confirmed first.
- If changes are started, explain which files will be changed and why.
- Unless explicitly requested, provide only the minimum necessary modification.
- All outputs must be in Chinese.