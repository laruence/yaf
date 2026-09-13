# IDE support for Yaf

Yaf is implemented in C, so there is no PHP source for an IDE to read —
without help, editors cannot offer completion, parameter hints or
jump-to-definition for classes like `Yaf_Application`. This directory
provides that help:

- **yaf_classes.php** — the full Yaf API as plain PHP signatures,
  underscore style (`Yaf_Application`).
- **yaf_classes_namespace.php** — the same, namespaced style
  (`Yaf\Application`), for projects running with
  `yaf.use_namespace=1`. Register the ONE that matches your project's
  class style.
- **yaf.php** — the generator. It reflects over the loaded Yaf
  extension and dumps every class, interface, method and parameter.
  None of these files is meant to be executed or included at runtime.

## Using the stub

Register `yaf_classes.php` with your IDE, for example in PhpStorm:
Settings → PHP → Include path (add this directory), or open the file
and mark it "as stub". VSCode with Intelephense picks it up from the
workspace automatically.

## Regenerating

Run the generator whenever the extension's API changes:

```bash
php -d extension=yaf.so yaf.php > yaf_classes.php
php -d extension=yaf.so -d yaf.use_namespace=1 yaf.php > yaf_classes_namespace.php
```

The output must stay valid PHP source (`php -l`) in both styles.
