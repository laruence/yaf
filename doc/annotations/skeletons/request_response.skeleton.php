<?php
// Skeleton for group: request_response. Copy entries into your annotation file.
return [
    'Yaf_Request_Abstract' => [
        '@class' => '',
        '::isGet' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::isPost' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::isDelete' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::isPatch' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::isPut' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::isHead' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::isOptions' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::isCli' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::isXmlHttpRequest' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getQuery' => [
            'desc' => '',
            'params' => ['name' => '', 'default' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getRequest' => [
            'desc' => '',
            'params' => ['name' => '', 'default' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getPost' => [
            'desc' => '',
            'params' => ['name' => '', 'default' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getCookie' => [
            'desc' => '',
            'params' => ['name' => '', 'default' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getRaw' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getFiles' => [
            'desc' => '',
            'params' => ['name' => '', 'default' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::get' => [
            'desc' => '',
            'params' => ['name' => '', 'default' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getServer' => [
            'desc' => '',
            'params' => ['name' => '', 'default' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getEnv' => [
            'desc' => '',
            'params' => ['name' => '', 'default' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::setParam' => [
            'desc' => '',
            'params' => ['name' => '', 'value' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getParam' => [
            'desc' => '',
            'params' => ['name' => '', 'default' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getParams' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::clearParams' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getException' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getModuleName' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getControllerName' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getActionName' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::setModuleName' => [
            'desc' => '',
            'params' => ['module' => '', 'format_name' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::setControllerName' => [
            'desc' => '',
            'params' => ['controller' => '', 'format_name' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::setActionName' => [
            'desc' => '',
            'params' => ['action' => '', 'format_name' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getMethod' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getLanguage' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::setBaseUri' => [
            'desc' => '',
            'params' => ['uir' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getBaseUri' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::getRequestUri' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::setRequestUri' => [
            'desc' => '',
            'params' => ['uir' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::isDispatched' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::setDispatched' => [
            'desc' => '',
            'params' => ['flag' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::isRouted' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::setRouted' => [
            'desc' => '',
            'params' => ['flag' => ''],
            'returns' => '',
            'example' => '',
        ],
    ],
    'Yaf_Request_Http' => [
        '@class' => '',
        '::__construct' => [
            'desc' => '',
            'params' => ['requestUri' => '', 'baseUri' => ''],
            'returns' => '',
            'example' => '',
        ],
    ],
    'Yaf_Request_Simple' => [
        '@class' => '',
        '::__construct' => [
            'desc' => '',
            'params' => ['method' => '', 'module' => '', 'controller' => '', 'action' => '', 'params' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::isXmlHttpRequest' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
    ],
    'Yaf_Response_Abstract' => [
        '@class' => '',
        '::__construct' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::__toString' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::setBody' => [
            'desc' => '',
            'params' => ['body' => '', 'name' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::appendBody' => [
            'desc' => '',
            'params' => ['body' => '', 'name' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::prependBody' => [
            'desc' => '',
            'params' => ['body' => '', 'name' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::clearBody' => [
            'desc' => '',
            'params' => ['name' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getBody' => [
            'desc' => '',
            'params' => ['name' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::response' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
    ],
    'Yaf_Response_Http' => [
        '@class' => '',
        '::setHeader' => [
            'desc' => '',
            'params' => ['name' => '', 'value' => '', 'replace' => '', 'response_code' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::setAllHeaders' => [
            'desc' => '',
            'params' => ['headers' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::getHeader' => [
            'desc' => '',
            'params' => ['name' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::clearHeaders' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
        '::setRedirect' => [
            'desc' => '',
            'params' => ['url' => ''],
            'returns' => '',
            'example' => '',
        ],
        '::response' => [
            'desc' => '',
            'returns' => '',
            'example' => '',
        ],
    ],
    'Yaf_Response_Cli' => [
        '@class' => '',
    ],
];
