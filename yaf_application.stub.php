<?php

/** @generate-legacy-arginfo */

final class Yaf_Application { 
	/* constants */

	/* properties */
	static protected object $_app = NULL;

	protected array $config = NULL;
	protected object $dispatcher = NULL;
	protected array $_modules = NULL;
	protected bool $_running = false;
	protected string $_environ = "product";
	protected int $_err_no = 0;
	protected string $_err_msg = "";

	/* methods */
	public function __construct(string|array $config, string $environ = NULL);

	/**
	 * @return Yaf_Response_Abstract|FALSE
	 */
	public function run():object|false;

	public function execute(mixed $callback):mixed;

	public function environ():?string;

	/**
	 * @return Yaf_Application|false|null
	 */
	public function bootstrap():object|false|null;

	public function getConfig():?object;

	public function getModules():?array;

	/**
	 * @return Yaf_Dispatcher|NULL
	 */
	public function getDispatcher():?object;

	/**
	 * @return Yaf_Application|NULL|FALSE
	 */
	public function setAppDirectory(string $directory):object|null|false;

	public function getAppDirectory():?string;

	public function getLastErrorNo():?int;

	public function getLastErrorMsg():?string;

	/**
	 * @return Yaf_Application|NULL
	 */
	public function clearLastError():?object;

	/**
	 * @return Yaf_Application|NULL
	 */
	public static function app():?object;
}
