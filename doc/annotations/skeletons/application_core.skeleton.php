<?php
// Skeleton for group: application_core. Copy entries into your annotation file.
return [
    'Yaf_Application' => [
        '@class' => '',
        '::__construct' => [
            'desc' => '',
            'params' => ['config' => '', 'environ' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::run' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::execute' => [
            'desc' => '',
            'params' => ['callback' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::app' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::environ' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::bootstrap' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getConfig' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getModules' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getDispatcher' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::setAppDirectory' => [
            'desc' => '',
            'params' => ['directory' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getAppDirectory' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getLastErrorNo' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getLastErrorMsg' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::clearLastError' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getInstance' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
    ],
    'Yaf_Bootstrap_Abstract' => [
        '@class' => '',
    ],
    'Yaf_Dispatcher' => [
        '@class' => '',
        '::__construct' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::enableView' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::disableView' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::initView' => [
            'desc' => '',
            'params' => ['templates_dir' => '', 'options' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::setView' => [
            'desc' => '',
            'params' => ['view' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::setRequest' => [
            'desc' => '',
            'params' => ['request' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::setResponse' => [
            'desc' => '',
            'params' => ['response' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getApplication' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getRouter' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getResponse' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getRequest' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getDefaultModule' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getDefaultController' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getDefaultAction' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::setErrorHandler' => [
            'desc' => '',
            'params' => ['callback' => '', 'error_types' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::setDefaultModule' => [
            'desc' => '',
            'params' => ['module' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::setDefaultController' => [
            'desc' => '',
            'params' => ['controller' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::setDefaultAction' => [
            'desc' => '',
            'params' => ['action' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::returnResponse' => [
            'desc' => '',
            'params' => ['flag' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::autoRender' => [
            'desc' => '',
            'params' => ['flag' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::flushInstantly' => [
            'desc' => '',
            'params' => ['flag' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getInstance' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::dispatch' => [
            'desc' => '',
            'params' => ['request' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::throwException' => [
            'desc' => '',
            'params' => ['flag' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::catchException' => [
            'desc' => '',
            'params' => ['flag' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::registerPlugin' => [
            'desc' => '',
            'params' => ['plugin' => ''],
            'returns' => '',
            'example' => '',
        ],
    ],
    'Yaf_Loader' => [
        '@class' => '',
        '::__construct' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::autoload' => [
            'desc' => '',
            'params' => ['class_name' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getInstance' => [
            'desc' => '',
            'params' => ['local_library_path' => '', 'global_library_path' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::registerLocalNamespace' => [
            'desc' => '',
            'params' => ['namespace' => '', 'path' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getLocalNamespace' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::clearLocalNamespace' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::isLocalName' => [
            'desc' => '',
            'params' => ['class_name' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getNamespacePath' => [
            'desc' => '',
            'params' => ['class_name' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::import' => [
            'desc' => '',
            'params' => ['file' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::setLibraryPath' => [
            'desc' => '',
            'params' => ['library_path' => '', 'is_global' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getLibraryPath' => [
            'desc' => '',
            'params' => ['is_global' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::registerNamespace' => [
            'desc' => '',
            'params' => ['namespace' => '', 'path' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getNamespaces' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
    ],
];
