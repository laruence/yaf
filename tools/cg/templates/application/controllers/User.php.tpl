<?php
/**
 * @name UserController
 * @desc Example controller for custom routes, paired with the rewrite
 *       route registered in Bootstrap::_initRoute():
 *
 *       /user/:id  =>  UserController::listAction($id)
 *
 *       :id is a route placeholder; once the route matches it can be
 *       read in two equivalent ways:
 *       1) $this->getRequest()->getParam("id");
 *       2) bound to the action's parameter of the same name.
 *       The same route can also be declared under routes.* in
 *       conf/application.ini and loaded with Yaf_Router::addConfig().
 * @author {&$AUTHOR&}
 */
class UserController extends Yaf_Controller_Abstract {

	/**
	 * Handles /user/:id.
	 * For example, requesting /user/42 yields $id = "42".
	 */
	public function listAction($id = 0) {
		// fetch the parameter again from the request; both reads are equivalent
		$alt = $this->getRequest()->getParam("id");

		// in a real project, call a model here to load the user record
		$this->getView()->assign("id", $id);

		// return TRUE to render views/user/list.phtml
		return TRUE;
	}
}
