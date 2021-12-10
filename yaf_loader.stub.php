<?php

/** @generate-legacy-arginfo */

final class Yaf_Loader { 
	/* constants */

	/* properties */
	protected string $_local_ns = NULL;
	protected string $_library = NULL;
	protected string $_global_library = NULL;
	static protected Yaf_Loader $_instance = NULL;

	/* methods */
	private function __construct();

	/**
	 * @return Yaf_Loader
	 */
	public static function getInstance(string $local_path = NULL, string $global_path = NULL):object;

	public static function import(string $file):?bool;

	public function autoload(string $class_name):bool;

	/**
	 * @return Yaf_Loader|NULL|FALSE
	 */
	public function registerLocalNamespace(string|array $name_prefix, string $path = NULL):object|null|false;

	public function getLocalNamespace():?array;

	public function clearLocalNamespace():?bool;

	public function isLocalName(string $class_name):?bool;

	/**
	 * @return Yaf_Loader|NULL
	 */
	public function setLibraryPath(string $library_path, bool $is_global = false):object|null;

	public function getLibraryPath(bool $is_global = false):string;
}
