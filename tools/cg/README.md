## Yaf Codes Generator

Generate a full, ready-to-run Yaf application skeleton that mirrors the
Quick Start tutorial in the Yaf manual. Every generated method carries a
comment describing what it is for, so the skeleton doubles as runnable
documentation.

### Usage

```
php yaf_cg -d Sample
```

generates the folder `Sample` (default app name `yaf_skeleton`):

```
Sample/
├── public/            # web entry; point DocumentRoot here
│   ├── index.php      # application entry point
│   └── .htaccess      # Apache rewrite rules
├── conf/
│   └── application.ini
├── application/
│   ├── Bootstrap.php  # _init* bootstrap methods
│   ├── controllers/   # Index, Error, User
│   ├── views/         # index/, error/, user/
│   ├── models/        # Sample
│   ├── plugins/       # Sample (all six dispatch hooks)
│   └── library/
├── nginx.conf         # nginx equivalent of the .htaccess
└── readme.txt         # deployment steps
```

Options:

```
php yaf_cg -d Sample -n        # namespace class style (\Yaf\Application)
php yaf_cg -a MyApp -d MyApp   # custom application name
php yaf_cg -d Sample -f        # overwrite an existing output directory
```

To run it, point the web server root at `public/` (see the generated
`readme.txt` and `nginx.conf`).
