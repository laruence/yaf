<?php
/** @generate-legacy-arginfo */

abstract class Yaf_Response_Abstract { 
	/* constants */

	/* properties */
	protected array $_header = NULL;
	protected array $_body = NULL;
	protected bool $_sendheader = false;

	/* methods */
	public function __construct() {}

	/**
	 * @param string $body
	 * @param string $name
	 * @return Yaf_Response_Abstract|NULL|FALSE
	 */
	public function setBody($body, $name = NULL) {}

	/**
	 * @param string $body
	 * @param string $name
	 * @return Yaf_Response_Abstract|NULL|FALSE
	 */
	public function appendBody($body, $name = NULL) {}

	/**
	 * @param string $body
	 * @param string $name
	 * @return Yaf_Response_Abstract|NULL|FALSE
	 */
	public function prependBody($body, $name = NULL) {}

	/**
	 * @param string $name
	 * @return Yaf_Response_Abstract|NULL
	 */
	public function clearBody($name = NULL) {}

	public function getBody(string $name = NULL):mixed {}

	public function setRedirect(string $url):?bool {}

	/**
	 * @return bool
	 */
	public function response() {}

	public function __toString():string {}
}

class Yaf_Response_Http extends Yaf_Response_Abstract { 
	/* constants */

	/* properties */
	protected bool $_sendheader = true;
	protected int $_response_code = 200;

	/* methods */
	public function setHeader(string $name, string $value, bool $replace = 0, int $response_code = 0):?bool {}
	
	protected function setAllHeaders(array $headers):?bool {}

	public function getHeader(string $name = NULL):mixed {}

	/**
	 * @return Yaf_Response_Http|NULL|FALSE
	 */
	public function clearHeaders():object|null|false {}

	public function setRedirect(string $url):?bool {}

	public function response():?bool {}
}

class Yaf_Response_Cli extends Yaf_Response_Abstract { 
	/* constants */

	/* properties */

	/* methods */
}
