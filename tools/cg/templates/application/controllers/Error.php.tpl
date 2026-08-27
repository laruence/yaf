<?php
/**
 * @name ErrorController
 * @desc The error controller: the single entry point for uncaught
 *       exceptions thrown during dispatch. Requires
 *       application.dispatcher.catchException = TRUE
 *       (already enabled in conf/application.ini).
 * @see http://www.php.net/manual/en/yaf-dispatcher.catchexception.php
 * @author {&$AUTHOR&}
 */
class ErrorController extends Yaf_Controller_Abstract {

	/**
	 * The error action: since Yaf 2.1 the exception instance is passed
	 * in as a parameter. Log the error, set a 500 status code, and hand
	 * the exception to the error view.
	 */
	public function errorAction($exception) {
		// report a 500 status to the client (HTTP responses only — under
		// CLI the response object has no header methods)
		$response = $this->getResponse();
		if ($response instanceof Yaf_Response_Http) {
			$response->setHeader('Content-Type', 'text/html; charset=UTF-8', true, 500);
		}

		// in production you may want to log the details and only show a
		// generic message: error_log($exception->getMessage());

		// hand the exception to the view (application/views/error/error.phtml)
		$this->getView()->assign("exception", $exception);
	}
}
