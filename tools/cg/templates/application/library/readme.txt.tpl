Put project classes here that are not models (business libraries,
third-party wrappers, and so on).

These classes are autoloaded by Yaf_Loader by class name as well:
  - class Sample_Lib maps to application/library/Sample/Lib.php
    (an underscore is a directory separator)

You can also use Yaf_Loader::registerNamespace() to point a namespace
at a different path, or set yaf.library in php.ini for a shared
library directory.
