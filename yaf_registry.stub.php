<?php
/** @generate-legacy-arginfo */

final class Yaf_Registry { 
	/* constants */

	/* properties */
	static protected Yaf_Registry $_instance = NULL;
	protected array $_entries = NULL;

	/* methods */
	private function __construct() {}

	public static function get(string $name):mixed {}
	
	public static function has(string $name):?bool {}

	public static function set(string $name, mixed $value):?bool {}

	public static function del(string $name):?bool {}
}
