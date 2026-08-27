# Yaf 手册

Yaf (Yet Another Framework) `3.3.8-dev` 的 API 参考与使用指南。

## 这份手册的特点

- **签名零手抄**：全部类/方法签名由 PHP Reflection 从运行中的 yaf 扩展生成，与源码注册信息逐字节一致。不会出现传统文档里"方法名拼错、参数过时"的问题。
- **行为以 C 源码为准**：所有描述和示例经过与 yaf 3.3.8 源码逐条核对；近期修复的行为变化（如 `yaf.forward_limit=0` 的语义）在文档中有明确标注。
- **AI 友好**：纯 Markdown，无 HTML 噪音；仓库根目录提供 [llms.txt](../llms.txt)（目录）与 [llms-full.txt](../llms-full.txt)（全文合并，适合 AI 一次性读取）。

## 怎么用

**人类用户**：

1. 第一次接触 Yaf → [快速上手](guides/quick-start.md)
2. 想理解框架行为 → [请求生命周期](guides/lifecycle.md)、[目录结构与命名约定](guides/directory-conventions.md)、[路由机制](guides/routing.md)、[错误与异常处理](guides/error-handling.md)
3. 查 API → [index.md](index.md) 或直接进 [classes/](classes/)

**AI 助手**：

- 有选择地检索：读 [llms.txt](../llms.txt) 拿目录，按需打开具体页面
- 需要完整上下文：直接读 [llms-full.txt](../llms-full.txt)
- 注意：本手册反映的是 yaf **3.3.8-dev**（含未发布的行为修复），与线上 3.3.x 稳定版有少量差异

## 目录结构

```
yaf/
├── llms.txt             AI 目录（llms.txt 约定，仓库根目录）
├── llms-full.txt        AI 全文（合并版，仓库根目录）
└── doc/
    ├── README.md        本文件
    ├── index.md         全部类/接口目录
    ├── constants.md     预定义常量
    ├── ini.md           php.ini 配置项
    ├── guides/          概念指南（手写）
    │   ├── quick-start.md
    │   ├── lifecycle.md
    │   ├── directory-conventions.md
    │   ├── routing.md
    │   └── error-handling.md
    └── classes/         38 个类/接口参考页（自动生成）
```

## 生成方式

类参考页由生成脚本构建（脚本不随仓库分发）：加载 yaf 扩展后，签名经 Reflection 直接读取扩展的注册信息，文案维护在 `doc/annotations/`（与签名分离）。生成器同时输出覆盖率 QA 报告。本目录下的文档基于 **3.3.8-dev**（commit `192e34d` 之后的 master）生成。
