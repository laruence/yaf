<?php

/** @generate-legacy-arginfo */

abstract class Yaf_Controller_Abstract { 
	/* constants */

	/* properties */
	protected ?array $actions = NULL;
	protected ?string $_module = NULL;
	protected ?string $_name = NULL;
	protected ?object $_request = NULL;
	protected ?object $_response = NULL;
	protected ?array $_invoke_args = NULL;
	protected ?object $_view = NULL;

	/* methods */

	public function __construct();

	/**
	 * @return Yaf_Request_Abstract|NULL
	 */
	final public function getRequest():?object;

	/**
	 * @return Yaf_Response_Abstract|NULL
	 */
	final public function getResponse():?object;

	final public function getModuleName():?string;

	/**
	 * @return Yaf_View_Interface|NULL
	 */
	final public function getView():?object;

	/**
	 * @return Yaf_View_Interface|NULL
	 */
	final public function initView(?array $options = NULL):?object;

	final public function setViewpath(string $view_directory):?bool;

	final public function getViewpath():?string;

	/**
	 *
	 * @polymorphism
	 * final public function forward(string $action):?bool
	 * final public function forward(string $controller, $action):?bool
	 * final public function forward(string $action, array $invoke_args):?bool
	 * final public function forward(string $module, string $controller, string $action):?bool
	 * final public function forward(string $controller, string $action, array $invoke_args):?bool
	 * final public function forward(string $module, string $controller, string $action, array $invoke_args):?bool
	 */
	final public function forward(mixed $args1, mixed $args2 = NULL, mixed $args3 = NULL, mixed $args4 = NULL):?bool {}

	final public function redirect(string $url):?bool;

	final public function getInvokeArgs():?array;

	final public function getInvokeArg(string $name):?string;

	final public function getName():?string;

	final protected function render(string $tpl, ?array $parameters = NULL):string|null|bool;

	final protected function display(string $tpl, ?array $parameters = NULL):?bool;
}

abstract class Yaf_Action_Abstract extends Yaf_Controller_Abstract { 
	/* constants */

	/* properties */

	/* methods */

	/* @tentative-return-type */
	/**
	 * @return bool
	 */
	abstract public function execute();

	/**
	 * @return Yaf_Controller_Abstract|NULL
	 */
	public function getController():?object;

	public function getControllerName():?string;
}
