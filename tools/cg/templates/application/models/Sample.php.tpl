<?php
/**
 * @name SampleModel
 * @desc Data-access example backed by a tiny in-memory "database".
 *
 * The skeleton must run with zero external dependencies, so instead of
 * a real database this model keeps its rows in a static property and
 * mimics the shape of a DAO: find()/insert() methods that a real
 * implementation would back with PDO or a similar driver. Swapping the
 * static array for a real connection is a contained change — the
 * controllers only talk to these two methods.
 *
 * Classes under models/ are autoloaded by Yaf_Loader by class name, so
 * a plain "new SampleModel()" works — no manual require needed.
 *
 * @author {&$AUTHOR&}
 */
class SampleModel {

	/**
	 * The mock table. A static property so rows inserted earlier in
	 * the same request (or by other model instances) stay visible.
	 *
	 * @var array
	 */
	protected static $records = array(
		array("id" => 1, "name" => "Yaf",    "role" => "The C-implemented PHP framework"),
		array("id" => 2, "name" => "Yaconf", "role" => "Persistent configuration container"),
		array("id" => 3, "name" => "Yac",    "role" => "Shared-memory cache for PHP"),
		array("id" => 4, "name" => "Yar",    "role" => "Concurrent RPC framework"),
	);

	/**
	 * Read example: return every record, the way a DAO's findAll()
	 * would. Replace the array return with a real query (e.g. PDO) in
	 * a real project.
	 *
	 * @return array
	 */
	public function find() {
		return static::$records;
	}

	/**
	 * Write example: append a record and report success, the way a
	 * DAO's insert() would.
	 *
	 * @param array $arrInfo the fields to persist
	 * @return bool
	 */
	public function insert($arrInfo) {
		$arrInfo["id"] = count(static::$records) + 1;
		static::$records[] = $arrInfo;
		return true;
	}
}
