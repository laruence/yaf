<?php
/** @generate-legacy-arginfo */

final class Yaf_Session implements \Iterator, \Traversable, \ArrayAccess, \Countable { 
	/* constants */

	/* properties */
	static protected object $_instance = NULL;
	protected array $_session = NULL;
	protected bool $_started = false;

	/* methods */
	private function __construct();

	/**
	 * @return Yaf_Session|NULL
	 */
	public static function getInstance():?object;

	/**
	 * @return Yaf_Session
	 */
	public function start():object;

	public function get(string $name = NULL):mixed;

	public function has(string $name):?bool;

	public function set(string $name, mixed $value):?bool;

	public function del(string $name):?bool;

	/**
	 * @return Yaf_Session|NULL|FALSE
	 */
	public function clear():object|null|false;

	/** Countable */
	public function count():int { }

	/** Iterator */
	public function rewind():void { }
	public function current():mixed { }
	public function next():void { }
	public function valid():bool { }
	public function key():int|string|null|bool { }

	/** ArrayAccess */
    /**
     * @implementation-alias Yaf_Session::get
     */
	public function offsetGet(mixed $name):mixed;

    /**
     * @implementation-alias Yaf_Session::set
     */
	public function offsetSet(mixed $name, mixed $value):void;

    /**
     * @implementation-alias Yaf_Session::del
     */
	public function offsetUnSet(mixed $name):void;

    /**
     * @implementation-alias Yaf_Session::has
     */
	public function offsetExists(mixed $name):bool;


	/** @implementation-alias Yaf_Session::get */
	public function __get(string $name):mixed;

    /**
     * @implementation-alias Yaf_Session::has
     */
	public function __isset(string $name):bool;

    /**
     * @implementation-alias Yaf_Session::set
     */
	public function __set(string $name, mixed $value):void;

    /**
     * @implementation-alias Yaf_Session::del
     */
	public function __unset(string $name):void;
}
