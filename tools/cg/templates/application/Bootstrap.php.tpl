<?php
/**
 * @name Bootstrap
 * @author {&$AUTHOR&}
 * @desc The application bootstrap class.
 *
 * Every method of the Bootstrap class whose name starts with _init is
 * called automatically by Yaf_Application::bootstrap() (methods with
 * other names are not), in declaration order, each receiving the
 * Yaf_Dispatcher instance as its argument. Keep initialization logic
 * split into focused _init methods.
 *
 * @see http://www.php.net/manual/en/class.yaf-bootstrap-abstract.php
 */
class Bootstrap extends Yaf_Bootstrap_Abstract {

	/**
	 * Stash the configuration in Yaf_Registry.
	 * Yaf_Application::app()->getConfig() returns the (read-only) config
	 * object; putting it in the Registry makes it reachable from
	 * anywhere via Yaf_Registry::get('config').
	 */
	public function _initConfig() {
		$arrConfig = Yaf_Application::app()->getConfig();
		Yaf_Registry::set('config', $arrConfig);
	}

	/**
	 * Register plugins. Plugins hook into the six points of the dispatch
	 * cycle (see application/plugins/Sample.php); when several plugins
	 * are registered, they run in registration order.
	 */
	public function _initPlugin(Yaf_Dispatcher $dispatcher) {
		$objSamplePlugin = new SamplePlugin();
		$dispatcher->registerPlugin($objSamplePlugin);
	}

	/**
	 * Register custom routes. The default route is Yaf_Route_Static
	 * (URIs like /controller/action/name/value). This example maps
	 * /user/:id to UserController::listAction with a Yaf_Route_Rewrite;
	 * alternatively load routes from configuration via
	 * $dispatcher->getRouter()->addConfig($routes).
	 */
	public function _initRoute(Yaf_Dispatcher $dispatcher) {
		$dispatcher->getRouter()->addRoute('user', new Yaf_Route_Rewrite(
			'/user/:id',
			array('controller' => 'User', 'action' => 'list')
		));
	}

	/**
	 * Replace the view engine. Yaf uses Yaf_View_Simple by default
	 * (templates are plain PHP scripts, see application/views/).
	 * Register a third-party template engine such as Smarty here.
	 */
	public function _initView(Yaf_Dispatcher $dispatcher) {
	}
}
