<?php

abstract class Yaf_Plugin_Abstract {

	/** methods */

	/**
	 * @return bool
	 */
	public function routerStartup(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {}

	/**
	 * @return bool
	 */
	public function routerShutdown(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {}

	/**
	 * @return bool
	 */
	public function dispatchLoopStartup(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {}

	/**
	 * @return bool
	 */
	public function dispatchLoopShutdown(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {}

	/**
	 * @return bool
	 */
	public function preDispatch(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {}

	/**
	 * @return bool
	 */
	public function postDispatch(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {}

	/**
	 * @return bool
	 */
	public function preResponse(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {}
}

/**
 * for yaf.use_namespace
 */
abstract class NS_Yaf_Plugin_Abstract {

	/** methods */
	/**
	 * @return bool
	 */
	public function routerStartup(\Yaf\Request_Abstract $request, \Yaf\Response_Abstract $response) {}

	/**
	 * @return bool
	 */
	public function routerShutdown(\Yaf\Request_Abstract $request, \Yaf\Response_Abstract $response) {}

	/**
	 * @return bool
	 */
	public function dispatchLoopStartup(\Yaf\Request_Abstract $request, \Yaf\Response_Abstract $response) {}

	/**
	 * @return bool
	 */
	public function dispatchLoopShutdown(\Yaf\Request_Abstract $request, \Yaf\Response_Abstract $response) {}

	/**
	 * @return bool
	 */
	public function preDispatch(\Yaf\Request_Abstract $request, \Yaf\Response_Abstract $response) {}

	/**
	 * @return bool
	 */
	public function postDispatch(\Yaf\Request_Abstract $request, \Yaf\Response_Abstract $response) {}

	/**
	 * @return bool
	 */
	public function preResponse(\Yaf\Request_Abstract $request, \Yaf\Response_Abstract $response) {}
}
