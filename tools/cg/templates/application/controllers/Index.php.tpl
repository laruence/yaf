<?php
/**
 * @name IndexController
 * @desc 默认控制器
 * @see http://www.php.net/manual/en/class.yaf-controller-abstract.php
 * @author huixinchen
 */
class IndexController extends Yaf_Controller_Abstract {

	//默认动作
	public function indexAction($exception) {
		//1. fetch query
		$name = $this->getRequest()->getQuery("name", "Laruence");
		//2. fetch model
		$model = new SampleModel();
		//3. assign
		$this->getView()->assign("content", $model->selectSample());

		//4. render by Yaf, 如果这里返回FALSE, Yaf将不会调用自动视图引擎Render模板
        return TRUE;
	}
}
