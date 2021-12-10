<?php

/** @generate-legacy-arginfo */

final class Yaf_Router { 
	/* constants */

	/* properties */
	protected array $_routes = NULL;
	protected string $_current = NULL;

	/* methods */
	public function __construct();

	/**
	 * @param Yaf_Route_Interface $route
	 * @return Yaf_Router|NULL
	 */
	public function addRoute(string $name, object $route):?object;

	/**
	 * @param Yaf_Config_Abstract|array $config
	 * @return Yaf_Router|NULL|FALSE
	 */
	public function addConfig(object|array $config):object|null|false;

	/**
	 * @param Yaf_Request_Abstract $request
	 */
	public function route(object $request):?bool;

	/**
	 * @return Yaf_Route_Interface|NULL|FALSE
	 */
	public function getRoute(string $name):object|null|false;

	public function getRoutes():?array;

	public function getCurrentRoute():int|string|null;
}
