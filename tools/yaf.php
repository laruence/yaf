<?php
/**
 * Yaf Classes signature generator
 *
 * @author  Laruence
 * @date    2012-07-21 13:46
 * @version $Id$
 */

$useNamespace = (bool) ini_get("yaf.use_namespace");
$yafClassPrefix = sprintf("Yaf%s", $useNamespace ? "\\" : "_");
$classes = array_merge(get_declared_classes(), get_declared_interfaces());
foreach ($classes as $key => $value) {
    if (strncasecmp($value, $yafClassPrefix, 4)) {
        unset($classes[$key]);
    }
}

echo "<?php\n";

foreach ($classes as $class_name) {
    $class = new ReflectionClass($class_name);
    $indent  = "";

    if ($useNamespace && false !== ($backslash = strrpos($class_name, "\\"))) {
        $namespaceName  = substr($class_name, 0, $backslash);
        $class_name = substr($class_name, $backslash + 1);
        echo "namespace ", $namespaceName, " {\n";

        $indent = "\t";
    }

    $classAttributes = "";

    if ($class->isInterface()) {
        $classAttributes .= "interface ";
    } else {
        if ($class->isFinal()) {
            $classAttributes .= "final ";
        }

        if ($class->isAbstract()) {
            $classAttributes .= "abstract ";
        }

        $classAttributes .= "class ";
    }

    echo $indent, $classAttributes, $class_name;

    /* parent */
    $parent = $class->getParentClass();
    if ($parent) {
        echo " extends ", $parent->getName();
    }

    /* interface */
    $interfaces = $class->getInterfaceNames();
    if (count($interfaces)) {
        echo " implements ", join(", ", $interfaces);
    }
    echo " {\n";

    $indent .= "\t";
    /* constants */
    $constants = $class->getConstants();
    if (0 < count($constants)) {
      echo $indent, "/* constants */\n";

      foreach ($constants as $k => $v) {
           echo $indent, "const ", $k , " = \"", $v , "\";\n";
      }
      echo "\n";
    }

    /* properties */
    $properties = $class->getProperties();
    if (0 < count($properties)) {
      echo $indent, "/* properties */\n";
      $values     = $class->getDefaultProperties();
      foreach ($properties as $p) {
          echo $indent;

          if ($p->isStatic()) {
              echo "static ";
          }

          if ($p->isPublic()) {
              echo "public ";
          } else if ($p->isProtected()) {
              echo "protected ";
          } else {
              echo "private ";
          }

          echo '$', $p->getName(), " = ";

          if (isset($values[$p->getName()])) {
              echo '"', $values[$p->getName()], '"';
          } else {
              echo "NULL";
          }
          echo ";\n";
      }
      echo "\n";
    }

    /* methods */
    $methods = $class->getMethods();
    if (0 < count($methods)) {
      echo $indent, "/* methods */\n";

      foreach ($methods as $m) {
          echo $indent;
          echo implode(' ', Reflection::getModifierNames($m->getModifiers()));
          echo " function ", $m->getName(), "(";

          if ($m->isAbstract()) {
              // abstract methods are without a body "{ ... }"
              echo ");\n";
              continue;
          }

          $parameters = $m->getParameters();
          $number = count($parameters);
          $index  = 0;
          foreach ($parameters as $a) {
              if (($type = $a->getClass())) {
                  echo $type->getName(), " ";
              } else if ($a->isArray()) {
                  echo "array ";
              }

              if ($a->isPassedByReference()) {
                  echo "&";
              }

              $name = $a->getName();
              if ($name == "...") {
                  echo '$_ = "..."';
              } else {
                  echo "$", $name;
              }

              if ($a->isOptional()) {
                  if ($a->isDefaultValueAvailable()) {
                      echo " = ", $a->getDefaultValue();
                  } else {
                      echo " = NULL";
                  }
              }

              if (++$index < $number) {
                  echo ", ";
              }
          }

          echo ") {\n";
          echo $indent, "}\n";
      }
    }

    $indent = substr($indent, 0, -1);
    echo $indent, "}\n";

    if ($useNamespace && false !== $backslash) {
        echo "}\n\n";
    }
}