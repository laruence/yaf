<?php

/** @generate-legacy-arginfo */

final class Yaf_Dispatcher { 
	/* constants */

	/* properties */
	static protected object $_instance = NULL;

	protected object $_router = NULL;
	protected object $_view = NULL;
	protected object $_request = NULL;
	protected array $_plugins = NULL;
	protected bool $_auto_render = true;
	protected bool $_return_response = false;
	protected bool $_instantly_flush = true;
	protected string $_default_module = NULL;
	protected string $_default_controller = NULL;
	protected string $_default_action = NULL;

	/* methods */
	private function __construct();

	/**
	 * @return Yaf_Dispatcher|NULL
	 */
	public function enableView():?object;

	/**
	 * @return Yaf_Dispatcher|NULL
	 */
	public function disableView():?object;

	/**
	 * @return Yaf_View_Interface|NULL|FALSE
	 */
	public function initView(string $templates_dir, array $options = NULL):object|null|false;

	/**
	 * @param Yaf_View_Interface $view
	 * @return Yaf_Dispatcher|NULL
	 */
	public function setView(object $view):?object;

	/**
	 * @param Yaf_Request_Abstract $request
	 * @return Yaf_Dispatcher|NULL
	 */
	public function setRequest(object $request):?object;

	/**
	 * @param Yaf_Response_Abstract $response
	 * @return Yaf_Dispatcher|NULL
	 */
	public function setResponse(object $response):?object;

	/**
	 * @return Yaf_Application|NULL
	 */
	public function getApplication():?object;

	/**
	 * @return Yaf_Router|NULL
	 */
	public function getRouter():?object;

	/**
	 * @return Yaf_Request_Abstract|NULL
	 */
	public function getRequest():?object;

	/**
	 * @return Yaf_Response_Abstract|NULL
	 */
	public function getResponse():?object;

	/**
	 * @return Yaf_Dispatcher|NULL|FALSE
	 */
	public function setErrorHandler(mixed $callback, int $error_types = 0):object|null|false;

	/**
	 * @return Yaf_Dispatcher|NULL|FALSE
	 */
	public function setDefaultModule(string $module):object|null|false;

	/**
	 * @return Yaf_Dispatcher|NULL|FALSE
	 */
	public function setDefaultController(string $controller):object|null|false;

	/**
	 * @return Yaf_Dispatcher|NULL|FALSE
	 */
	public function setDefaultAction(string $action):object|null|false;

	/**
	 * @return Yaf_Dispatcher|NULL|FALSE
	 */
	public function returnResponse(bool $flag = false):object|null|false;

	/**
	 * @return Yaf_Dispatcher|NULL|BOOL
	 */
	public function autoRender(?bool $flag = NULL):object|null|bool;

	/**
	 * @return Yaf_Dispatcher|NULL|BOOL
	 */
	public function flushInstantly(?bool $flag = NULL):object|null|bool;

	/**
	 * @param Yaf_Request_Abstract $request
	 * @return Yaf_Response_Abstract|NULL|FALSE
	 */
	public function dispatch(object $request):object|null|false;

	/**
	 * @return Yaf_Dispatcher|NULL|BOOL
	 */
	public function throwException(?bool $flag = NULL):object|null|bool;

	/**
	 * @return Yaf_Dispatcher|NULL|BOOL
	 */
	public function catchException(?bool $flag = NULL):object|null|bool;

	/**
	 * @param Yaf_Plugin_Abstract $plugin
	 * @return Yaf_Dispatcher|NULL|FALSE
	 */
	public function registerPlugin(object $plugin):object|null|false;

	public function getDefaultModule():?string;
	public function getDefaultController():?string;
	public function getDefaultAction():?string;

	/**
	 * @return Yaf_Dispatcher|NULL
	 */
	public static function getInstance():?object;
}
