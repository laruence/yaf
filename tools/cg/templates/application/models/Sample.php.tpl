<?php
/**
 * @name SampleModel
 * @desc Sample data-access class. In a real project this is where you
 *       talk to a database, files, or other services.
 *
 * Classes under models/ are autoloaded by Yaf_Loader by class name, so
 * a plain "new SampleModel()" works — no manual require needed.
 *
 * @author {&$AUTHOR&}
 */
class SampleModel {

	/**
	 * Constructor: establish connections or read configuration here.
	 * A real project might do dependency injection or lazy DB setup.
	 */
	public function __construct() {
	}

	/**
	 * Example read method: returns a sample record.
	 * Replace with a real query (e.g. PDO) in your project.
	 */
	public function selectSample() {
		return 'Hello world';
	}

	/**
	 * Example write method: stores a record and reports success.
	 *
	 * @param array $arrInfo the fields to persist
	 */
	public function insertSample($arrInfo) {
		return true;
	}
}
