## Yaf Codes Generator

Generate a full, ready-to-run Yaf application skeleton that mirrors the
Quick Start tutorial in the Yaf manual. Every generated method carries a
comment describing what it is for, so the skeleton doubles as runnable
documentation.

The generated pages are a small but complete application: a styled
home page introducing Yaf's design philosophy, a model demo that loads
records through a mock DAO (`SampleModel`, ready to be swapped for a
real database driver), an error demo showing the catchException →
error controller path, plus a stylesheet, a page script and the Yaf
crab logo served as real static files.

### Usage

```
php yaf_cg [-a ApplicationName] [-d ApplicationPath] [-n] [-f]
```

Options:

| Option | Meaning |
|---|---|
| `-a` | Application name; written into comments, page titles and the readme. Defaults to `yaf_skeleton`. Must be a valid directory name. |
| `-d` | Output directory. Defaults to `./<ApplicationName>` next to yaf_cg. |
| `-n` | Generate the namespaced class style (`\Yaf\Application` instead of `Yaf_Application`) for projects running with `yaf.use_namespace=1`. |
| `-f` | Overwrite the output directory if it already exists; without it, yaf_cg refuses to touch an existing directory. |
| `-h` | Print the usage. |

Examples:

```
php yaf_cg -d Sample              # into ./Sample, default app name
php yaf_cg -d Sample -n           # same, namespaced class style
php yaf_cg -a MyApp -d /tmp/myapp # custom application name and path
php yaf_cg -d Sample -f           # overwrite an existing output directory
```

The generated layout:

```
Sample/
├── public/            # web entry; point DocumentRoot here
│   ├── index.php      # application entry point
│   ├── .htaccess      # Apache rewrite rules
│   ├── css/style.css  # shared stylesheet
│   ├── js/app.js      # shared page script
│   └── img/yaf-logo.jpg
├── conf/
│   └── application.ini
├── application/
│   ├── Bootstrap.php  # _init* bootstrap methods
│   ├── controllers/   # Index (index/user/demo actions), Error
│   ├── views/         # index/, error/
│   ├── models/        # Sample (mock DAO)
│   ├── plugins/       # Sample (all six dispatch hooks)
│   └── library/
├── nginx.conf         # nginx equivalent of the .htaccess
├── router.php         # router script for PHP's built-in server quick try
└── readme.txt         # deployment steps, incl. a PHP built-in server quick start
```

To run it, point the web server root at `public/` (see the generated
`readme.txt` and `nginx.conf`).
