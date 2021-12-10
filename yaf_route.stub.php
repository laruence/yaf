<?php

/** @generate-legacy-arginfo */

Interface Yaf_Route_Interface {
	/**
	 * @param Yaf_Request_Abstract $request
	 * @return bool
	 */
	abstract public function route($request);

	/**
	 * @return string|bool
	 */
	abstract public function assemble(array $info, array $query = NULL);
}

final class Yaf_Route_Static implements Yaf_Route_Interface {
	public function match(string $uri):bool {}

	/**
	 * @param Yaf_Request_Abstract $request
	 * @return bool
	 */
	public function route($request) {}

	public function assemble(array $info, array $query = NULL):string|null|false;
}

final class Yaf_Route_Supervar implements Yaf_Route_Interface { 
	/* constants */

	/* properties */
	protected string $_var_name = NULL;

	/* methods */
	public function __construct(string $supervar_name) {}

	/**
	 * @param Yaf_Request_Abstract $request
	 * @return bool
	 */
    public function route($request);

	public function assemble(array $info, array $query = NULL):string|null|false;
}

final class Yaf_Route_Simple implements Yaf_Route_Interface { 
	/* constants */

	/* properties */
	protected string $controller = NULL;
	protected string $module = NULL;
	protected string $action = NULL;

	/* methods */
	public function __construct(string $module_name, string $controller_name, string $action_name) {}

	/**
	 * @param Yaf_Request_Abstract $request
	 * @return bool
	 */
	public function route($request);

	public function assemble(array $info, array $query = NULL):string|null|false;
}


final class Yaf_Route_Rewrite implements Yaf_Route_Interface { 
	/* constants */

	/* properties */
	protected array $_route = NULL;
	protected array $_verify = NULL;

	/* methods */
	public function __construct(string $match, array $route, array $verify = NULL) { }

	public function match(string $uri):?bool {}

	/**
	 * @param Yaf_Request_Abstract $request
	 * @return bool
	 */
	public function route($request) {}

	public function assemble(array $info, array $query = NULL):string|null|false;
}

final class Yaf_Route_Regex implements Yaf_Route_Interface { 
	/* constants */

	/* properties */
	protected array $_route = NULL;
	protected array $_maps = NULL;
	protected array $_verify = NULL;

	/* methods */
	public function __construct(string $match, array $route, array $map = NULL, array $verify = NULL, string $reverse = NULL) {}

	public function match(string $uri):?bool {}

	/**
	 * @param Yaf_Request_Abstract $request
	 * @return bool
	 */
	public function route($request) {}

	public function assemble(array $info, array $query = NULL):string|null|false;
}

final class Yaf_Route_Map implements Yaf_Route_Interface { 
	/* constants */

	/* properties */
	protected bool $_ctl_router = 1;
	protected string $_delimiter = NULL;

	/* methods */
	public function __construct(bool $controller_prefer = 1, string $delimiter = "") {}

	/**
	 * @param Yaf_Request_Abstract $request
	 * @return bool
	 */
	public function route($request) {}

	public function assemble(array $info, array $query = NULL):string|null|false;
}
