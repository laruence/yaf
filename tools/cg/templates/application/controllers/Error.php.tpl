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
	 * in as a parameter. Map the exception type to a proper HTTP status
	 * (dispatch failures are 404, everything else 500), log the error,
	 * and hand the exception to the error view.
	 */
	public function errorAction($exception) {
		/* dispatch failures (controller/action/view not found) are
		 * client errors; anything else is a server error */
		$code = 500;
		switch (true) {
			case $exception instanceof Yaf_Exception_LoadFailed_Controller:
			case $exception instanceof Yaf_Exception_LoadFailed_Action:
			case $exception instanceof Yaf_Exception_LoadFailed_Module:
			case $exception instanceof Yaf_Exception_LoadFailed_View:
				$code = 404;
				break;
		}

		// report the status to the client (HTTP responses only — under
		// CLI the response object has no header methods);
		// Yaf_Response_Http::setHeader()'s 4th argument sets the
		// response code sent along with the headers
		$response = $this->getResponse();
		if ($response instanceof Yaf_Response_Http) {
			$response->setHeader('Content-Type', 'text/html; charset=UTF-8', true, $code);
		}

		// in production you may want to log the details and only show a
		// generic message: error_log($exception->getMessage());

		// hand the exception and the status code to the view
		// (application/views/error/error.phtml)
		$this->getView()->assign("exception", $exception);
		$this->getView()->assign("code", $code);
	}
}
