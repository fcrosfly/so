/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_haojing.h"

ZEND_DECLARE_MODULE_GLOBALS(haojing)

/* True global resources - no need for thread safety here */
static int le_haojing;

/* {{{ haojing_functions[]
 *
 * Every user visible function must have an entry in haojing_functions[].
 */
const zend_function_entry haojing_functions[] = {
	PHP_FE(confirm_haojing_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in haojing_functions[] */
};
/* }}} */

/* {{{ haojing_module_entry
 */
zend_module_entry haojing_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"haojing",
	haojing_functions,
	PHP_MINIT(haojing),
	PHP_MSHUTDOWN(haojing),
	PHP_RINIT(haojing),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(haojing),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(haojing),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_HAOJING
ZEND_GET_MODULE(haojing)
#endif

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("haojing.profiling", "0", PHP_INI_ALL, OnUpdateLong, profiling, zend_haojing_globals, haojing_globals)
    STD_PHP_INI_ENTRY("profiling_percent", "100", PHP_INI_ALL, OnUpdateLong, profiling, zend_haojing_globals, haojing_globals)
    STD_PHP_INI_ENTRY("haojing.profiling_mintime", "300", PHP_INI_ALL, OnUpdateLong, profiling_mintime, zend_haojing_globals, haojing_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_haojing_init_globals
 */
static void php_haojing_init_globals(zend_haojing_globals *haojing_globals)
{
	haojing_globals->profiling = 0;
	haojing_globals->profiling_percent = 100;
	haojing_globals->profiling_mintime = 300;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(haojing)
{
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(haojing)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(haojing)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(haojing)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(haojing)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "haojing support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_haojing_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_haojing_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "haojing", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
