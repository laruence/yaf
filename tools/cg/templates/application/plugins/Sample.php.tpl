<?php
/**
 * @name SamplePlugin
 * @desc A plugin hooks into the dispatch cycle. Yaf defines six hooks;
 *       override the ones you need, register the plugin in the
 *       Bootstrap, and each hook is called at its point in the cycle.
 *       When several plugins are registered they run first-in-first-called.
 * @see http://www.php.net/manual/en/class.yaf-plugin-abstract.php
 * @author {&$AUTHOR&}
 */
class SamplePlugin extends Yaf_Plugin_Abstract {

	/**
	 * Fired before routing: inspect or rewrite the raw request URI.
	 */
	public function routerStartup(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {
	}

	/**
	 * Fired after routing: module/controller/action are known — a good
	 * place for per-route access control.
	 */
	public function routerShutdown(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {
	}

	/**
	 * Fired before the dispatch loop starts: one-time setup shared by
	 * every dispatch.
	 */
	public function dispatchLoopStartup(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {
	}

	/**
	 * Fired before each dispatch: filter parameters or redirect before
	 * the action runs.
	 */
	public function preDispatch(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {
	}

	/**
	 * Fired after each dispatch: decorate the response or the rendered view.
	 */
	public function postDispatch(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {
	}

	/**
	 * Fired after the dispatch loop ends: final cleanup, logging or flushing.
	 */
	public function dispatchLoopShutdown(Yaf_Request_Abstract $request, Yaf_Response_Abstract $response) {
	}
}
