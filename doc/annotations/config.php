<?php
/**
 * Annotations: Yaf_Config family.
 *
 * Behaviors verified against yaf_config.c, configs/yaf_config_ini.c,
 * configs/yaf_config_simple.c:
 *   - Abstract::get matches an exact top-level key only (no dot path);
 *     Yaf_Config_Ini overrides get() with dot-path ("a.b.c") traversal.
 *   - Yaf_Config_Ini is always readonly (YAF_CONFIG_READONLY is set
 *     unconditionally in yaf_config_ini_init); set/__set/offsetSet emit
 *     E_WARNING "config is readonly".
 *   - Section inheritance "[child : parent]" is a deep merge; multiple
 *     parents merge right-to-left, leftmost parent wins, then the
 *     section's own keys.
 *   - Yaf_Config_Simple's 2nd ctor arg is the readonly flag (not a
 *     section name); writable by default; offsetUnset deletes even when
 *     readonly (warning only).
 */

return [
    'Yaf_Config_Abstract' => [
        '@class' => "配置适配器基类，为应用提供对象化的配置数据访问。实现了 `Iterator`、`ArrayAccess`、`Countable`，因此可以直接 `foreach`、`count()`、按数组下标访问。有两个实现：`Yaf_Config_Ini`（INI 文件，只读）和 `Yaf_Config_Simple`（PHP 数组，默认可写）。读取到的数组值会被自动包装为同类型的子配置对象（继承父对象的只读状态）。抽象类不可实例化，且不可序列化、不可克隆。",
        '::get' => [
            'desc' => '按键名读取配置项。基类实现只做顶层键的精确匹配，**不解析点号路径**（`Yaf_Config_Ini` 重写了本方法才支持 `a.b.c`）。值为数组时自动包装为子配置对象；键不存在返回 `NULL`。',
            'params' => ['name' => '配置键名。可省略（或传 `NULL`），此时返回配置对象自身。'],
            'returns' => '配置值；数组值返回子配置对象；键不存在返回 `NULL`；省略参数返回 `$this`。',
            'example' => "\$config = new Yaf_Config_Simple(['db' => ['host' => '127.0.0.1'], 'name' => 'demo']);\nvar_dump(\$config->get('name')); // string(4) \"demo\"\n\$db = \$config->get('db');       // 数组 → 子配置对象\nvar_dump(\$db instanceof Yaf_Config_Simple); // true",
        ],
        '::count' => [
            'desc' => '返回顶层配置项数量。实现了 `Countable`，可直接对配置对象使用 `count()`。',
            'returns' => '顶层配置项个数（int）。',
            'example' => "\$config = new Yaf_Config_Simple(['a' => 1, 'b' => 2]);\nvar_dump(count(\$config)); // int(2)",
        ],
        '::toArray' => [
            'desc' => '导出配置数据为原生 PHP 数组。返回的是原始数据，嵌套数组不会包装成配置对象。',
            'returns' => '当前（顶层）配置的数组表示。',
            'example' => "\$config = new Yaf_Config_Ini('app.ini', 'product');\n\$arr = \$config->toArray();\nvar_dump(\$arr['name']); // 原生数组，值不再是配置对象",
        ],
        '::offsetUnset' => [
            'desc' => '基类实现一律拒绝删除：产生 `E_WARNING`（config is readonly）并返回 `FALSE`。`Yaf_Config_Simple` 重写后可真正删除。',
            'params' => ['name' => '要删除的键（被忽略，不做实际操作）。'],
            'returns' => '`FALSE`。',
            'example' => "\$config = new Yaf_Config_Ini('app.ini');\nunset(\$config['foo']); // E_WARNING: config is readonly",
        ],
        '::rewind' => [
            'desc' => '将内部迭代指针重置回第一个元素（`Iterator` 实现）。`foreach` 开始遍历时会自动调用。',
            'returns' => '无返回值。',
            'example' => "\$config = new Yaf_Config_Ini('app.ini', 'product');\nforeach (\$config as \$key => \$value) {\n    // 遍历结束时指针停在末尾\n}\n\$config->rewind(); // 重置后可再次手动遍历",
        ],
        '::current' => [
            'desc' => '返回当前迭代指针指向的值（`Iterator` 实现）。值为数组时包装为同类型子配置对象；指针越界或配置为空返回 `FALSE`。',
            'returns' => '当前配置值（标量或子配置对象），无元素时 `FALSE`。',
            'example' => '',
        ],
        '::key' => [
            'desc' => '返回当前迭代指针指向的键（`Iterator` 实现）。',
            'returns' => '字符串键或整数索引；指针越界时返回 `FALSE`。',
            'example' => '',
        ],
        '::next' => [
            'desc' => '将内部迭代指针前移一位（`Iterator` 实现）。',
            'returns' => '无返回值。',
            'example' => '',
        ],
        '::valid' => [
            'desc' => '检查当前指针位置是否仍有元素（`Iterator` 实现），供 `foreach` 判断是否继续。',
            'returns' => '还有元素返回 `TRUE`，否则 `FALSE`。',
            'example' => '',
        ],
        '::__isset' => [
            'desc' => '检查顶层键是否存在，支持 `isset($config->name)`。只做顶层精确匹配，不支持点号路径。',
            'params' => ['name' => '要检查的键名。'],
            'returns' => '存在返回 `TRUE`，否则 `FALSE`。',
            'example' => "\$config = new Yaf_Config_Simple(['name' => 'demo']);\nvar_dump(isset(\$config->name));    // true\nvar_dump(isset(\$config->dummy));   // false",
        ],
        '::__get' => [
            'desc' => '`get()` 的别名，支持属性式读取 `$config->name`。',
            'params' => ['name' => '配置键名。'],
            'returns' => '与 `get()` 相同。',
            'example' => "\$config = new Yaf_Config_Simple(['name' => 'demo']);\necho \$config->name; // 等价于 \$config->get('name')",
        ],
        '::offsetGet' => [
            'desc' => '`get()` 的别名，支持数组式读取 `$config[\'name\']`。在 `Yaf_Config_Ini` 上同样支持点号路径。',
            'params' => ['name' => '配置键名。'],
            'returns' => '与 `get()` 相同。',
            'example' => '',
        ],
        '::offsetExists' => [
            'desc' => '`__isset()` 的别名，支持 `isset($config[\'name\'])`。',
            'params' => ['name' => '要检查的键名。'],
            'returns' => '存在返回 `TRUE`，否则 `FALSE`。',
            'example' => '',
        ],
        '::offsetSet' => [
            'desc' => '抽象方法，由子类实现写入。`Yaf_Config_Ini` 永远失败并告警；`Yaf_Config_Simple` 在非只读时可写。',
            'params' => ['name' => '键名。', 'value' => '要写入的值。'],
            'returns' => '由子类决定。',
            'example' => '',
        ],
        '::set' => [
            'desc' => '抽象方法，由子类实现写入配置项。写入契约见各子类。',
            'params' => ['name' => '键名。', 'value' => '要写入的值。'],
            'returns' => '由子类决定。',
            'example' => '',
        ],
        '::readonly' => [
            'desc' => '抽象方法，返回配置是否只读。`Yaf_Config_Ini` 恒为 `TRUE`；`Yaf_Config_Simple` 取决于构造参数。',
            'returns' => '由子类决定。',
            'example' => '',
        ],
    ],
    'Yaf_Config_Ini' => [
        '@class' => "INI 文件配置适配器，`final` 类，**恒为只读**。特性：\n\n- **section 继承**：`[child : parent]` 语法，子节深合并（递归合并嵌套数组）父节配置，子节同名键覆盖；支持多父节 `[a : b : c]`，自右向左合并，左侧父节优先，最后是节自身定义的键。\n- **点号键自动嵌套**：节内 `a.b.c=1` 解析为嵌套数组 `['a']['b']['c']`。\n- **变量插值**：值中 `\${CONST}` 引用 PHP 常量、`\${ini.directive}` 引用 php.ini 指令。\n\n读取数组值返回只读的子 `Yaf_Config_Ini` 对象。迭代、计数、数组访问能力继承自 `Yaf_Config_Abstract`。",
        '::__construct' => [
            'desc' => '解析 INI 配置文件。文件不存在、不是常规文件或解析失败时抛出 `Yaf_Exception`。指定 `section` 时只加载该节（节支持 `:` 继承语法，父节配置会被深合并进来），节不存在则抛出 `Yaf_Exception`（"There is no section ..."）；省略时整个文件的所有节作为顶层键加载。',
            'params' => [
                'config_file' => 'INI 文件路径。',
                'section' => '可选。要加载的节名；省略或传 `NULL` 时加载全部节。',
            ],
            'returns' => '构造 `Yaf_Config_Ini` 实例，失败抛异常。',
            'example' => "// app.ini:\n//   [common]\n//   db.host = \"127.0.0.1\"\n//   [product : common]\n//   name = \"demo\"\n\$config = new Yaf_Config_Ini('app.ini', 'product');\necho \$config->get('name');     // demo\necho \$config->get('db.host');  // 127.0.0.1（自 common 继承）",
        ],
        '::get' => [
            'desc' => '读取配置项，**支持点号路径**如 `routes.rewrite.match`：按 `.` 逐层深入，中间层不是数组时直接返回该中间值；最终键不存在返回 `NULL`。值为数组时包装为只读的子 `Yaf_Config_Ini` 对象。省略参数返回对象自身。',
            'params' => ['name' => '配置键名，可含 `.` 分隔的层级路径。可省略，此时返回对象自身。'],
            'returns' => '配置值；数组返回子配置对象；不存在返回 `NULL`。',
            'example' => "\$config = new Yaf_Config_Ini('app.ini', 'product');\necho \$config->get('routes.rewrite.match'); // 点号路径\necho \$config['routes.rewrite.match'];      // offsetGet 是 get 的别名\n\$routes = \$config->get('routes');          // 数组 → 只读子配置对象\nvar_dump(\$config->get('not.exists'));      // NULL",
        ],
        '::set' => [
            'desc' => 'INI 配置恒为只读，写入永远失败：产生 `E_WARNING`（config is readonly）。',
            'params' => ['name' => '键名。', 'value' => '要写入的值。'],
            'returns' => '`FALSE`。',
            'example' => "\$config = new Yaf_Config_Ini('app.ini');\nvar_dump(\$config->set('name', 'new')); // E_WARNING + bool(false)",
        ],
        '::readonly' => [
            'desc' => 'INI 配置恒为只读，本方法恒返回 `TRUE`。',
            'returns' => '`TRUE`。',
            'example' => '',
        ],
        '::__set' => [
            'desc' => 'INI 配置恒为只读，属性式赋值 `$config->name = ...` 被忽略并产生 `E_WARNING`（config is readonly）。',
            'params' => ['name' => '键名。', 'value' => '要写入的值。'],
            'returns' => '无返回值。',
            'example' => '',
        ],
        '::offsetGet' => [
            'desc' => '`get()` 的别名，数组式访问 `$config[\'name\']`，同样支持点号路径。',
            'params' => ['name' => '配置键名，可含 `.` 分隔的层级路径。'],
            'returns' => '与 `get()` 相同。',
            'example' => '',
        ],
        '::offsetSet' => [
            'desc' => '`set()` 的别名。INI 配置只读，`$config[\'name\'] = ...` 永远失败并产生 `E_WARNING`。',
            'params' => ['name' => '键名。', 'value' => '要写入的值。'],
            'returns' => '`FALSE`。',
            'example' => '',
        ],
    ],
    'Yaf_Config_Simple' => [
        '@class' => "PHP 数组配置适配器，`final` 类。直接包装一个 PHP 数组，**默认可写**；构造时传第二个参数 `TRUE` 可设为只读。数组值同样会被包装为子配置对象；`get()` 继承自 `Yaf_Config_Abstract`，只做顶层键精确匹配，不支持点号路径。",
        '::__construct' => [
            'desc' => '用 PHP 数组创建配置。第一个参数必须是数组，否则触发 TypeError。注意：第二个参数名为 `section` 但实际语义是**只读标志**（bool，默认 `FALSE` 可写）——本类没有节的概念。只读模式下直接持有传入的数组（零拷贝）；可写模式下复制一份，后续与原数组互不影响。',
            'params' => [
                'config_file' => '配置数据数组。',
                'section' => '实际为只读标志：传 `TRUE` 表示只读，默认 `FALSE` 可写。',
            ],
            'returns' => '构造 `Yaf_Config_Simple` 实例。',
            'example' => "\$config = new Yaf_Config_Simple(['db' => ['host' => '127.0.0.1']]);\nvar_dump(\$config->readonly()); // false，默认可写\n\$ro = new Yaf_Config_Simple(['a' => 1], true);\nvar_dump(\$ro->readonly());     // true，只读",
        ],
        '::set' => [
            'desc' => '写入**顶层**配置项（已存在则覆盖）。只读模式下静默失败返回 `FALSE`（不产生告警，与 Ini 的行为不同）。不支持点号路径。',
            'params' => ['name' => '键名。', 'value' => '要写入的值。'],
            'returns' => '写入成功返回 `TRUE`；只读或写入失败返回 `FALSE`。',
            'example' => "\$config = new Yaf_Config_Simple(['name' => 'demo']);\n\$config->set('version', '1.0');   // 或 \$config->version = '1.0'\nprint_r(\$config->toArray());",
        ],
        '::readonly' => [
            'desc' => '返回配置是否只读，即构造时传入的只读标志。',
            'returns' => '只读返回 `TRUE`，可写返回 `FALSE`。',
            'example' => '',
        ],
        '::offsetUnset' => [
            'desc' => '删除顶层配置项，支持 `unset($config[\'key\'])`。仅字符串和整数偏移有效。**注意**：即使配置是只读模式也会真正删除，只读时仅额外产生一条 `E_WARNING`。',
            'params' => ['name' => '要删除的键（字符串或整数）。'],
            'returns' => '无返回值。',
            'example' => "\$config = new Yaf_Config_Simple(['foo' => 1, 'bar' => 2]);\nunset(\$config['foo']);\nprint_r(\$config->toArray()); // ['bar' => 2]",
        ],
        '::__set' => [
            'desc' => '属性式写入 `$config->name = ...`，内部调用 `set()`；只读模式下被忽略。',
            'params' => ['name' => '键名。', 'value' => '要写入的值。'],
            'returns' => '无返回值。',
            'example' => '',
        ],
        '::offsetSet' => [
            'desc' => '`set()` 的别名，支持 `$config[\'key\'] = value` 数组式写入；只读模式下静默失败。',
            'params' => ['name' => '键名。', 'value' => '要写入的值。'],
            'returns' => '写入成功返回 `TRUE`，只读返回 `FALSE`。',
            'example' => '',
        ],
    ],
];
