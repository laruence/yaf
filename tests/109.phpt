--TEST--
Check for SIMD build_camel_name
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--INI--
yaf.use_spl_autoload=0
yaf.lowcase_path=0
yaf.use_namespace=0
--FILE--
<?php
$name = array(
	'_a_b_cd_1_eF_g_', /* 15 bytes */
	'a_b_cd_1_eF_g_h',
	'_a_b_cd_1_eF_g_h', /* 16 bytes */
	'a_b_cd_1_eF_g_H_',
	'_a_b_cd_1_eF_g_h_', /* 17 bytes */
	'a_b_cd_1_eF_g_H_i',
	'_a_b_cd_1_eF_g_h_1', /* 18 bytes */
	'a_b_cd_1_eF_g_H_i1',
	'_a_b_cd_1_eF_g_h_iiiiii_J123_u', /* 32 bytes */
	'a_b_cd_1_eF_g_h_Jiiiii_j123_kl',
	'_a_b_cd_1_eF_g_h_iiiiii_J123k_l1', /* 34 bytes */
	'a_b_cd_1_eF_g_h_Jiiiii_j123_uV_L',
);

$request = new Yaf_Request_Simple();

setlocale(LC_ALL, "C");

function build_camel_name($u) {
    $u[0] = strtoupper($u[0]);
    for($i = 1; $i < strlen($u); $i++) {
       if ($u[$i - 1] == '_') {
		  $u[$i] = strtoupper($u[$i]);
       } else {
          $u[$i] = strtolower($u[$i]);
       }
    }
	return $u;
}

/* standard test */
foreach ($name as $u) {
	$request->setControllerName($u);
    if ($request->getControllerName() != build_camel_name($u)) {
		echo $u, ": expected: " , build_camel_name($u), "; got: ", $request->getControllerName(), "\n";
    }
}

/* random test */
$i = 0;
while ($i++ < 1024) {
	$len = rand(1, 64);
    $u = str_repeat(' ', $len);
    for ($c = 0; $c < $len; $c++) {
       $u[$c] = rand(1, 255);
    }
	$request->setControllerName($u);
	if ($request->getControllerName() != build_camel_name($u)) {
		echo $u, ": expected: " , build_camel_name($u), "; got: ", $request->getControllerName(), "\n";
	}
}
?>
okey
--EXPECT--
okey
