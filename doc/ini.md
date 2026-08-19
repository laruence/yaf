# Yaf 配置项 (php.ini)

以下 ini 指令由 yaf 扩展注册。"默认值" 为扩展编译期默认。

| 指令 | 默认值 | 说明 |
| --- | --- | --- |
| `yaf.action_prefer` | `0` | 开启后 URL 中优先解析 action (而非 module) |
| `yaf.environ` | `product` | 默认环境名, 用于读取配置文件对应 section |
| `yaf.forward_limit` | `5` | 分发循环最大迭代次数, 防止 forward 死循环; 0 或负数会被钳制为默认值 5 |
| `yaf.library` | `(空)` | 全局类库目录, 供 Yaf_Loader 加载第三方类 |
| `yaf.lowcase_path` | `0` | 自动加载时是否将类路径转小写 |
| `yaf.name_separator` | `(空)` | 类名与模块名之间的分隔符 |
| `yaf.name_suffix` | `1` | 类名是否携带 Controller/Action 等后缀 |
| `yaf.use_namespace` | `0` | 开启后以 Yaf\ 命名空间形式注册类 (PHP 5.3+) |
| `yaf.use_spl_autoload` | `0` | 找不到类时是否回退到 SPL autoloader |

