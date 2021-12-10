<?php

/** @generate-legacy-arginfo */

abstract class Yaf_Request_Abstract { 
	/* constants */
	/* const SCHEME_HTTP = "http";
	const SCHEME_HTTPS = "https"; */

	/* properties */
	public ?string    $module = NULL;
	public ?string    $controller = NULL;
	public ?string    $action = NULL;
	public ?string    $method = NULL;
	protected ?array  $params = NULL;
	protected ?string $language = NULL;
	protected object  $_exception = NULL;
	protected string  $_base_uri = "";
	protected string  $uri = "";
	protected bool  $dispatched = false;
	protected bool  $routed = false;

	/* methods */
	public function isGet():bool {}

	public function isPost():bool {}

	public function isPut():bool {}

	public function isHead():bool {}

	public function isOptions():bool {}

	public function isDelete():bool {}

	public function isPatch():bool {}

	public function isCli():bool {}

	public function isXmlHttpRequest():bool {}

	public function getServer(string $name = NULL, mixed $default = NULL):mixed {}

	public function getEnv(string $name = NULL, mixed $default = NULL):mixed {}

	public function getQuery(string $name = NULL, mixed $default = NULL):mixed {}

	public function getRequest(string $name = NULL, mixed $default = NULL):mixed {}

	public function getPost(string $name = NULL, mixed $default = NULL):mixed {}

	public function getCookie(string $name = NULL, mixed $default = NULL):mixed {}

	public function getFiles(string $name = NULL, mixed $default = NULL):mixed {}

	public function getRaw():string|null {}

	public function get(string $name, mixed $default = NULL):mixed {}

	/**
	 * @return Yaf_Request_Abstract|NULL
	 */
	public function clearParams():?object {}

	/**
	 * @return Yaf_Request_Abstract|bool|NULL
	 */
	public function setParam(mixed $name, ?mixed $value = NULL):object|bool|null {}

	public function getParam(string $name, mixed $default = NULL):mixed {}

	public function getParams():?array {}

	public function getException():?Exception {}

	public function getModuleName():?string {}

	public function getControllerName():?string {}

	public function getActionName():?string {}

	/**
	 * @return Yaf_Request_Abstract|NULL
	 */
	public function setModuleName(string $module,  bool $format_name = true):?object {}

	/**
	 * @return Yaf_Request_Abstract|NULL
	 */
	public function setControllerName(string $controller, bool $format_name = true):?object {}

	/**
	 * @return Yaf_Request_Abstract|NULL
	 */
	public function setActionName(string $action, bool $format_name = true):?object {}

	public function getMethod():?string {}

	public function getLanguage():?string {}

	/**
	 * @return Yaf_Request_Abstract|FALSE
	 */
	public function setBaseUri(string $uir):object|false {}

	public function getBaseUri():?string {}

	public function getRequestUri():?string {}

	/**
	 * @return Yaf_Request_Abstract|NULL
	 */
	public function setRequestUri(string $uir):?object {}

	public function isDispatched():bool {}

	/**
	 * @return Yaf_Request_Abstract|NULL
	 */
	public function setDispatched(bool $flag = true):?object {}

	public function isRouted():bool {}

	/**
	 * @return Yaf_Request_Abstract|NULL
	 */
	public function setRouted(bool $flag = true):?object {}
}

class Yaf_Request_Http extends Yaf_Request_Abstract { 
	/* constants */

	/* properties */

	/* methods */
	public function __construct(?string $requestUri = NULL, ?string $baseUri = NULL) {}
}

final class Yaf_Request_Simple extends Yaf_Request_Abstract { 
	/* constants */

	/* properties */

	/* methods */
	public function __construct(?string $method = NULL, ?string $module = NULL, ?string $controller = NULL, ?string $action = NULL, ?array $params = NULL) {}

	public function isXmlHttpRequest():bool{}
}
