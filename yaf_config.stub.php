<?php
/** @generate-legacy-arginfo */

abstract class Yaf_Config_Abstract implements \Iterator, \Traversable, \ArrayAccess, \Countable { 
	/* constants */

	/* properties */
	protected ?array $_config = NULL;
	protected bool $_readonly = 0;

	/* methods */
	public function count():int { }
	public function rewind():void { }
	public function current():mixed { }
	public function next():void { }
	public function valid():bool { }
	public function key():int|string|null|bool { }

	/* @tentative-return-type */
	public function __isset(string $name):bool {}

	public function __get(string $name):mixed {}

    /**
     * @implementation-alias Yaf_Config_Abstract::get
     */
	public function offsetGet(mixed $name):mixed {}

    /**
     * @implementation-alias Yaf_Config_Abstract::set
     */
	public function offsetSet(mixed $name, mixed $value):void {}

	public function offsetUnSet(mixed $name):void {}

    /**
     * @implementation-alias Yaf_Config_Abstract::__isset
     */
	public function offsetExists(mixed $name):bool {}


	abstract public function get(?string $name = NULL):mixed;
	abstract public function set(string $name, mixed $value):bool;
	abstract public function readonly():bool;
	abstract public function toArray():array;
}

final class Yaf_Config_Simple extends Yaf_Config_Abstract implements \Iterator, \Traversable, \ArrayAccess, \Countable { 
	/* constants */

	/* properties */

	/* methods */
	public function __construct(array|string $config_file, ?string $section = NULL) {}

	public function __isset(string $name):bool {}

	public function __set(string $name, mixed $value):void {}

	public function get(?string $name = NULL):mixed {}

	public function set(string $name, mixed $value):bool {}

	public function readonly():bool {}

	/** @implementation-alias Yaf_Config_Simple::get */
	public function offsetGet(mixed $name):mixed {}

	/** @implementation-alias Yaf_Config_Simple::set */
	public function offsetSet(mixed $name, mixed $value):void {}

	public function offsetUnSet(mixed $name):void {}
}

final class Yaf_Config_Ini extends Yaf_Config_Abstract implements \Iterator, \Traversable, \ArrayAccess, \Countable { 
	/* constants */

	/* properties */
	protected bool $_readonly = 1;

	/* methods */
	public function __construct(array|string $config_file, ?string $section = NULL) {}

	public function __isset(string $name):bool {}

	public function get(?string $name = NULL):mixed {}

	public function set(string $name, mixed $value):bool {}

	public function readonly():bool {}

	/** @implementation-alias Yaf_Config_Ini::get */
	public function offsetGet(mixed $name):mixed {}

	/** @implementation-alias Yaf_Config_Ini::set */
	public function offsetSet(mixed $name, mixed $value):void {}

	public function __set(mixed $name, mixed $value):void {}
}
