--TEST--
Bug #63438 (Strange behavior with nested rendering)
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=1
yaf.lowcase_path=0
--FILE--
<?php

function shut_down() {
   echo "done";
}

register_shutdown_function("shut_down");

function view($file){
    static $view;
   
    $view = new Yaf_View_Simple(dirname(__FILE__));
    return $view->render($file);
}

file_put_contents(dirname(__FILE__) . '/outer.phtml', "1 <?php print view('inner.phtml');?> 3\n");
file_put_contents(dirname(__FILE__) . '/inner.phtml', "2");
print (view('outer.phtml'));

file_put_contents(dirname(__FILE__) . '/outer.phtml', "1 <?php \$this->display('inner.phtml');?> 3\n");
print (view('outer.phtml'));

file_put_contents(dirname(__FILE__) . '/outer.phtml', "1 <?php echo \$this->eval('2');?> 3\n");
print (view('outer.phtml'));

file_put_contents(dirname(__FILE__) . '/outer.phtml', "1 <?php \$this->display('inner.phtml');?> 3\n");
file_put_contents(dirname(__FILE__) . '/inner.phtml', "<?php undefined_function(); ?>");
print (view('outer.phtml'));
?>
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/outer.phtml');
@unlink(dirname(__FILE__) . '/inner.phtml');
?>
--EXPECTF--
1 2 3
1 2 3
1 2 3
1 
Fatal error: Call to undefined function undefined_function() in %sinner.phtml on line %d
done
