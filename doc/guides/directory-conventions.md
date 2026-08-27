# 目录结构与类名约定

Yaf 不扫描目录，只按**类名 → 路径**的确定性映射定位文件。理解了这套规则，就不会再为"类找不到"或"视图找不到"困惑。本文所有规则对照 `yaf_loader.c`、`yaf_application.c`、`yaf_dispatcher.c` 源码（3.3.x）校验过。

## 标准目录结构

一切从配置项 `application.directory` 开始（`yaf_application.c` 的 `yaf_application_parse_option`）。这是配置中**唯一必填**项——缺失会直接报 `YAF_ERR_STARTUP_FAILED`。末尾斜杠会被自动剥掉。

```text
application/                     # = application.directory
├── controllers/                 # 默认模块（Index）的控制器
│   ├── Index.php                # IndexController
│   └── Sub/                     # 类 Sub_IndexController
├── models/                      # *Model 类
├── plugins/                     # *Plugin 类
├── views/                       # 默认模块的视图（phtml）
├── library/                     # 本地类库（可经 application.library 改名）
└── modules/                     # 多模块时才有（默认模块除外）
    └── Api/
        ├── controllers/
        ├── views/
        ├── models/
        └── plugins/
```

约定而非强制的目录名（源码常量见 `yaf_loader.h`）：`controllers`、`models`、`plugins`、`views`、`library`、`modules`。类文件默认扩展名 `.php`（可用配置项 `application.ext` 改），视图默认 `.phtml`（`application.view.ext` 可改）。

## 类名 → 文件路径映射（Yaf_Loader 核心规则）

[Yaf_Loader](../classes/yaf_loader.md) 的 `autoload()` 是唯一入口，规则分四步。

### 第 1 步：类名规范化

- 去掉开头的 `\`。
- 所有 `\` 一律替换为 `_`（`yaf_loader_sanitize_name`）。命名空间和下划线对加载器完全等价：`Foo\Bar_Baz` ≡ `Foo_Bar_Baz`。

### 第 2 步：判定类别（yaf_loader_identify_category）

判断这个类是 MVC 类（Controller/Model/Plugin）还是普通类库类。

`yaf.name_suffix=1`（默认）时，先看类名**最后一个字符**做快速分派，再做后缀匹配：

| 末字符 | 尝试匹配的后缀 | 目录 |
| --- | --- | --- |
| `l` | `Model` | `models/` |
| `n` | `Plugin` | `plugins/` |
| `r` | `Controller` | `controllers/` |
| 其他 | — | 普通类 |

匹配要求：类名长度必须**大于**后缀长度（类名恰好叫 `Model` 不算），且后缀匹配是**大小写敏感**的精确匹配（内部用按位比较的 `yaf_slip_equal`），所以 `indexController`、`FooModeler` 都不算 MVC 类。

`yaf.name_suffix=0` 时对称翻转：按**首字母**（`M`/`P`/`C`）分派，匹配**前缀** `Model_`/`Plugin_`/`Controller_`，例如 `Model_User` → `models/User.php`。

### 第 3 步：拼接路径

- **MVC 类**：`{application.directory}/{controllers|models|plugins}/{名字}.php`，名字 = 类名剥掉后缀。类名中间/结尾的 `_` 全部换成 `/`（`yaf_loader_sanitize_path`）。
- **普通类**：`{类库目录}/{类名下划线转斜杠}.php`，类库目录按下面的本地/全局规则选取。

### 第 4 步：修饰项

- `yaf.name_separator`（默认空）：非空时，MVC 后缀与类名之间**必须**出现该分隔符才算 MVC 类，拼接时去掉。如 `name_separator=_` 时，类 `User_Model` 是 Model（→ `models/User.php`），`UserModel` 不是，按普通类处理。
- `yaf.lowcase_path=1`：路径部分（文件名部分）先转小写再换斜杠。**只影响自动加载生成的路径，类名本身不变**。

### 示例（全部默认配置）

| 类名 | 实际加载的文件 |
| --- | --- |
| `IndexController` | `{dir}/controllers/Index.php` |
| `Sub_IndexController` | `{dir}/controllers/Sub/Index.php` |
| `UserModel` | `{dir}/models/User.php` |
| `Smarty_Plugin` | `{dir}/plugins/Smarty/Plugin.php` |
| `Foo\Bar_Baz` | `{library}/Foo/Bar/Baz.php` |
| `Model` / `UserModels` | 普通类，走类库目录（不是 MVC 类） |

## 本地类库 vs 全局类库

普通类的根目录在 [Yaf_Loader](../classes/yaf_loader.md) 中有两个：

- **本地类库** `loader->library`：默认 = `{application.directory}/library`；配置了 `application.library` 则用它。也可写数组形式 `library.directory` + `library.namespace`。
- **全局类库** `loader->glibrary`：来自 php.ini 的 `yaf.library`，供多个应用共享第三方类。

选择规则（`yaf_loader_load_user`）：

1. 按 `_` 逐级查找**已注册的本地命名空间**（`registerLocalNamespace`，或配置 `application.library.namespace`）：
   - 注册时指定了路径 → 用该路径作根；
   - 注册了前缀但没指定路径 → 用本地类库目录；
2. 前缀不在本地命名空间中：`yaf.library` 非空 → 用全局类库，否则退回本地类库。

`isLocalName()` / `getNamespacePath()` 就是这套解析逻辑的查询接口。

## 模块（modules）

- 模块列表从配置 `application.modules` 读取，格式是**逗号分隔字符串**（`"Index,Api,Admin"`）。每个段经 `yaf_build_camel_name` 转成驼峰存储，所以 `api` 与 `Api` 等价。
- 未配置时只有默认模块 `Index`（`yaf_application.c`：`app->modules == NULL` 时仅默认模块算合法模块）。
- 模块匹配**大小写不敏感**（`zend_string_equals_ci`）。
- 多模块目录形态：非默认模块的 MVC 类位于 `modules/{ModuleX}/` 下；**默认模块不走 modules/ 目录**，仍用顶层 `controllers/`、`views/` 等。
- 模块内 `models/`、`plugins/` 只服务于 [Yaf_Dispatcher](../classes/yaf_dispatcher.md) 的控制器加载路径——[Yaf_Loader](../classes/yaf_loader.md) 自动加载 MVC 类时**始终只用顶层** `application.directory` 下的三个目录，模块控制器文件实际由分发器按目录直接 include（`yaf_loader_load_internal`）。所以在模块代码里 `new UserModel` 仍从顶层 `models/` 加载。

## 控制器 / Action 文件命名

URL 段经默认路由转驼峰（`yaf_route_static.c`）：`/user-list` → 控制器 `UserList`。分发器按以下规则找文件（`yaf_dispatcher_get_controller`）：

- 默认模块：`{dir}/controllers/{Controller}.php`
- 非默认模块：`{dir}/modules/{Module}/controllers/{Controller}.php`
- 文件名取自控制器名**原样**（不转小写，除非 `yaf.lowcase_path=1`），`_` → `/`：类 `UserListController` → `UserList.php`，类 `Sub_IndexController` → `Sub/Index.php`。
- 加载后校验类名：`strtolower(controller) [+ separator] + "controller"`（类表查找不区分大小写）。
- 方法不存在时回退 `$actions` 映射表：`action 名 → 相对 application.directory 的脚本路径`，期望类名为 `action 名 + "Action"`（`yaf_dispatcher_get_action`）。

## 视图目录

[Yaf_Dispatcher](../classes/yaf_dispatcher.md) 在分发时确定模板目录（`yaf_dispatcher.c` 中 view_dir 逻辑）：

- **默认模块**：`{dir}/views`
- **非默认模块**：`{dir}/modules/{Module}/views`

即"模块内 views 优先"——一旦请求落在非默认模块，顶层 `views/` 就不再参与。控制器可用 `setViewpath()` 再覆盖；已设置过模板目录时 Dispatcher 不再重写。

视图文件名由 action 决定：`strtolower(controller) + "/" + action + "." + ext`，控制器名中的 `_` 同样换为 `/`（如 `render("list")` 于 `Sub_IndexController` → `sub/index/list.phtml`）。

## Bootstrap

配置 `application.bootstrap` 指向 Bootstrap 类文件；未配置时约定为 `{application.directory}/Bootstrap.php`，类名 `Bootstrap`，继承 [Yaf_Bootstrap_Abstract](../classes/yaf_bootstrap_abstract.md)。

## 常见误区

1. **"模块内 `new XxxModel` 为什么没找到？"** — Loader 只认顶层 `models/`，模块内 `modules/X/models/` 不会被自动加载器扫描。
2. **"类名写成了 `Usercontroller`（小写 c）。"** — 后缀匹配大小写敏感，判定失败，会去类库目录找 `Usercontroller.php` 而不是 `controllers/`。
3. **"开了 `yaf.lowcase_path` 后 Linux 上突然找不到文件。"** — 加载路径被转成全小写，文件名必须同步改为小写。
4. **"改了 `application.modules` 但模块没生效。"** — 必须同时建 `modules/{ModuleX}/controllers/` 目录；且默认模块不要放进 `modules/`，它始终用顶层目录。
