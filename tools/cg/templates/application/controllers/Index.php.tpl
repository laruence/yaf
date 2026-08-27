<?php
/**
 * @name IndexController
 * @author {&$AUTHOR&}
 * @desc The default controller. Requests that match no module/controller
 *       land here; every method ending in Action is an action, and
 *       indexAction is the default one.
 * @see http://www.php.net/manual/en/class.yaf-controller-abstract.php
 */
class IndexController extends Yaf_Controller_Abstract {

	/**
	 * Controller initialization hook: called automatically after the
	 * controller is constructed and before the action runs. Good place
	 * for controller-specific preparation (access checks, shared view
	 * variables). Safe to delete if not needed.
	 */
	public function init() {
		$this->getView()->assign("app_name", "{&$APP_NAME&}");
	}

	/**
	 * The default action.
	 *
	 * Yaf binds route parameters with a matching name directly to the
	 * action's formal parameters: requesting
	 * /index/index/index/name/{&$AUTHOR&} yields $name = "{&$AUTHOR&}"
	 * (the three "index" segments are module, controller and action).
	 *
	 * Return value semantics: returning TRUE (or nothing) lets Yaf
	 * auto-render views/index/index.phtml; returning FALSE tells Yaf the
	 * action produced the response itself and skips auto-rendering;
	 * returning a string sends it as the response body.
	 */
	public function indexAction($name = "Yaf") {
		//1. fetch a GET query parameter, with a fallback default value;
		//   try /?name=Laruence or /index/index/name/Laruence
		$get = $this->getRequest()->getQuery("get", "default value");

		//2. hand variables to the view engine; every key becomes a
		//   local variable in views/index/index.phtml
		$this->getView()->assign("name", $name);

		//3. let Yaf render the template
		return TRUE;
	}
}
