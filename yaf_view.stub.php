<?php

/** @generate-legacy-arginfo */

Interface Yaf_View_Interface {

	/**
	 * @return Yaf_View_Interface|bool
	 */
	abstract public function assign(string $name, mixed $value = NULL);

	/**
	 * @return string|bool
	 */
	abstract public function render(string $tpl, array $tpl_vars = NULL);

	/**
	 * @return Yaf_View_Interface|bool
	 */
	abstract public function display(string $tpl, array $tpl_vars = NULL);

	/**
	 * @return bool
	 */
	abstract public function setScriptPath(string $template_dir);

	/**
	 * @return string
	 */
	abstract public function getScriptPath();
}

final class Yaf_View_Simple implements Yaf_View_Interface { 
	/* constants */

	/* properties */
	protected array $_tpl_vars = NULL;
	protected string $_tpl_dir = NULL;
	protected array $_options = NULL;

	/* methods */
	public function __construct(string $tempalte_dir, array $options = NULL);

	public function __isset(string $name):bool;
	
	public function get(string $name = NULL):mixed;

	/**
	 * @return Yaf_View_Simple|NULL|FALSE
	 */
	public function assign(mixed $name = NULL, mixed $default = NULL):object|null|false;

	public function render(string $tpl, ?array $tpl_vars = NULL):null|string|false;

	public function eval(string $tpl_str, ?array $vars = NULL):null|string|false;

	public function display(string $tpl, ?array $tpl_vars = NULL):?bool;

	/**
	 * @return Yaf_View_Simple|NULL
	 */
	public function assignRef(string $name = NULL, mixed &$value):?object;

	/**
	 * @return Yaf_View_Simple|NULL
	 */
	public function clear(string $name = NULL):?object;

	/**
	 * @return Yaf_View_Simple|NULL|FALSE
	 */
	public function setScriptPath(string $template_dir):object|null|bool;

	public function getScriptPath():?string;

	/* @implementation-alias Yaf_View_Simple::get */
	public function __get(string $name = NULL):mixed; 
	
	/* @implementation-alias Yaf_View_Simple::assign */
	public function __set(string $name, mixed $value):void;
}
