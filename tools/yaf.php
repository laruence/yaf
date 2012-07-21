<?php
/**
 * Yaf Classes signature generator
 * 
 * @author  Laruence
 * @date    2012-07-21 13:46
 * @version $Id$ 
*/

$classes = get_declared_classes();
foreach ($classes as $key => $value) {
    if (strncasecmp($value, "Yaf_", 3)) {
        unset($classes[$key]);
    }
}

echo "<?php\n";

foreach ($classes as $class_name) {
     $class = new ReflectionClass($class_name);
     $indent  = "";

     if ($class->isFinal()) {
         echo "final ";
     }

     if ($class->isAbstract()) {
         echo "abstract ";
     }

     if ($class->isInterface()) {
         echo "Interface ", $class_name;
     } else {
         echo "class ", $class_name;
     }

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
     echo " { \n";

     $indent .= "\t";
     /* constants */
     echo $indent, "/* constants */\n";
     $constatnts = $class->getConstants();
     foreach ($constatnts as $k => $v) {
         echo $indent, "const ", $k , " = ", $v , ";\n";
     }
     echo "\n";

     /* properties */
     echo $indent, "/* properties */\n";
     $properties = $class->getProperties();
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

     /* methods */
     echo $indent, "/* methods */\n";
     $methods = $class->getMethods();
     foreach ($methods as $m) {
         echo $indent;
         echo implode(' ', Reflection::getModifierNames($m->getModifiers()));
         echo " function ", $m->getName(), "(";
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
     echo "}\n\n";
}
