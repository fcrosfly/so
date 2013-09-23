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
#include "ext/pcre/php_pcre.h"
#include "Zend/zend_exceptions.h"
#include <math.h>

ZEND_DECLARE_MODULE_GLOBALS(haojing)

/* True global resources - no need for thread safety here */
static int le_haojing;

/* {{{ haojing_functions[]
 *
 * Every user visible function must have an entry in haojing_functions[].
 */
const zend_function_entry haojing_functions[] = {
	PHP_FE(confirm_haojing_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(isint,  NULL)
	PHP_FE(utf8_filter, NULL)
	PHP_FE(a2o, NULL)
	PHP_FE(o2a, NULL)
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
    STD_PHP_INI_ENTRY("haojing.profiling_percent", "100", PHP_INI_ALL, OnUpdateLong, profiling_percent, zend_haojing_globals, haojing_globals)
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

#define PHP_HAOJING_EPS 1e-8
#define UTF8_ERR_BREAK 0x01
#define UTF8_ERR_EXCEPTION 0x02
#define UTF8_ERR_FILTER 0x04

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(haojing)
{
	REGISTER_INI_ENTRIES();
	REGISTER_LONG_CONSTANT("UTF8_ERR_BREAK",UTF8_ERR_BREAK,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UTF8_ERR_EXCEPTION",UTF8_ERR_EXCEPTION,CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UTF8_ERR_FILTER",UTF8_ERR_FILTER,CONST_CS | CONST_PERSISTENT);
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

/* {{{ proto bool isint(mixed arg)
Return a bool to confirm that whether the argument is integer */
PHP_FUNCTION(isint)
{
	zval **arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
		return;
	}
	if (Z_TYPE_PP(arg) != IS_DOUBLE){
		SEPARATE_ZVAL(arg);
		convert_to_double(*arg);
	}
	double value = Z_DVAL_PP(arg);
	double delta = fabs(value - round(value));
	if (delta < PHP_HAOJING_EPS){
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string utf8_filter(string subject [,int mode])
Return a string blablabla */
PHP_FUNCTION(utf8_filter)
{
	/* parameters */
	char			   *regex = "#([\x09\x0A\x0D\x20-\x7E]|[\xC2-\xDF][\x80-\xBF]|\xE0[\xA0-\xBF][\x80-\xBF]|[\xE1-\xEC\xEE\xEF][\x80-\xBF]{2}|\xED[\x80-\x9F][\x80-\xBF])++#"; 
	int					regex_len = strlen(regex); 
	char			   *subject;
	int					subject_len;
	pcre_cache_entry   *pce;
	zval			   *subpats = NULL;
	long				flags = 0;
	long				start_offset = 0;
	long				mode = UTF8_ERR_FILTER;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &subject, &subject_len, &mode) == FAILURE) {
		RETURN_STRING("", 1);
	}
	
	if ((pce = pcre_get_compiled_regex_cache(regex , regex_len TSRMLS_CC)) != NULL) {
		RETURN_STRING("", 1);
	}

	MAKE_STD_ZVAL(subpats);
	php_pcre_match_impl(pce, subject, subject_len, return_value, subpats,
			0, 0, flags, start_offset TSRMLS_CC); // preg_match

	if (Z_LVAL_P(return_value) != 0) {
		zval **tmp;
		char *match_str = NULL;
		if (zend_hash_index_find(Z_ARRVAL_P(subpats), 0, (void**)&tmp) == SUCCESS) {
			match_str = Z_STRVAL_PP(tmp);
		}
		RETVAL_STRING(subject, 0); // set default return value
		if (strcmp(match_str , subject) != 0) {
			if (mode & UTF8_ERR_BREAK) {
				RETVAL_STRING(match_str, 1);
			} else if (mode & UTF8_ERR_FILTER) {
				php_pcre_match_impl(pce, subject, subject_len, return_value, subpats,
						1, 0, flags, start_offset TSRMLS_CC); // preg_match_all

				zval **arg = NULL, *delim, *arr;
				if (zend_hash_index_find(Z_ARRVAL_P(subpats), 0, (void**)&arg) == SUCCESS) {
					MAKE_STD_ZVAL(delim);
					ZVAL_STRING(delim, "", 0);
					SEPARATE_ZVAL(arg);
					arr = *arg;
					php_implode(delim, arr, return_value TSRMLS_CC);
					FREE_ZVAL(delim);
				}
			} else if (mode & UTF8_ERR_EXCEPTION) {
				zend_throw_exception(NULL, "None UTF-8 string.", 0 TSRMLS_CC);
			}
		}
	} else {
		RETVAL_STRING("", 1);
	}
	/* clean up */
	zval_ptr_dtor(&subpats);
}
/* }}} */

/* {{{ proto object a2o(array arr, object obj [, array except])
convert array to object */
PHP_FUNCTION(a2o)
{
	zval *arr, *obj, *except = NULL;
	int is_set_except = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ao|a", &arr, &obj, &except) == FAILURE) {
		return;
	}

	if (!except) {
		// set default except
		is_set_except = 0;
		MAKE_STD_ZVAL(except);
		array_init(except);
	}
	// make the value of except be the key of except_flip
	zval *except_flip, **entry, *data;
	MAKE_STD_ZVAL(except_flip);
	array_init_size(except_flip, zend_hash_num_elements(Z_ARRVAL_P(except)));

	zend_hash_internal_pointer_reset(Z_ARRVAL_P(except));
	while (zend_hash_get_current_data(Z_ARRVAL_P(except), (void **)&entry) == SUCCESS) {
		MAKE_STD_ZVAL(data);
		Z_TYPE_P(data) = IS_LONG;
		Z_LVAL_P(data) = 0; 

		if (Z_TYPE_PP(entry) == IS_LONG) {
			zend_hash_index_update(Z_ARRVAL_P(except_flip), Z_LVAL_PP(entry), &data, sizeof(data), NULL);
		} else if (Z_TYPE_PP(entry) == IS_STRING) {
			zend_symtable_update(Z_ARRVAL_P(except_flip), Z_STRVAL_PP(entry), Z_STRLEN_PP(entry) + 1, &data, sizeof(data), NULL);
		} else {
			zval_ptr_dtor(&data);
			//php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Can only flip STRING and INTEGER values!");
		}
		zend_hash_move_forward(Z_ARRVAL_P(except));
	}

	zval **current;
	zend_object *zobj = zend_objects_get_address(obj TSRMLS_CC);
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(arr));
	while (zend_hash_get_current_data(Z_ARRVAL_P(arr), (void **) &current) == SUCCESS) {
		char *string_key = NULL;
		uint  string_key_len;
		long long num_key;

		SEPARATE_ZVAL(current);
		zend_hash_get_current_key_ex(Z_ARRVAL_P(arr), &string_key, &string_key_len, &num_key, 0, NULL);
		if (!string_key) {
			// key is numeric . convert to string
			if (!zend_hash_index_exists(Z_ARRVAL_P(except_flip), num_key)) {
				string_key = (char *)malloc(20);
				string_key_len = sprintf(string_key, "%lli", num_key);
				if (zend_check_property_access(zobj, string_key, string_key_len TSRMLS_CC) == SUCCESS)
					zend_update_property(NULL, obj, string_key, string_key_len, *current TSRMLS_CC);
			}
		} else {
			// key is string
			if (!zend_hash_exists(Z_ARRVAL_P(except_flip), string_key, string_key_len)) {
				if (zend_check_property_access(zobj, string_key, string_key_len - 1 TSRMLS_CC) == SUCCESS) {
					zend_update_property(NULL, obj, string_key, string_key_len - 1, *current TSRMLS_CC);
				}
			}
		}
		zend_hash_move_forward(Z_ARRVAL_P(arr));
	}
	
	/* clean up */
	zval_ptr_dtor(&except_flip);
	if (!is_set_except) {
		zval_ptr_dtor(&except);
	}
	RETURN_ZVAL(obj, 1, 0);
}
/* }}} */

/* {{{ proto array o2a(object obj [, array except])
convert object to array */
PHP_FUNCTION(o2a)
{
	zval *obj = NULL, *except = NULL;
	int is_set_except = 1;
	HashTable *properties;
	HashPosition pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o|a", &obj, &except) == FAILURE) {
		return;
	}
	if (Z_OBJ_HT_P(obj)->get_properties == NULL) {
		RETURN_FALSE;
	}
	properties = Z_OBJ_HT_P(obj)->get_properties(obj TSRMLS_CC);
	if (properties == NULL) {
		RETURN_FALSE;
	}

	if (!except) {
		// set default except
		is_set_except = 0;
		MAKE_STD_ZVAL(except);
		array_init(except);
	}
	// make the value of except be the key of except_flip
	zval *except_flip, **entry, *data;
	MAKE_STD_ZVAL(except_flip);
	array_init_size(except_flip, zend_hash_num_elements(Z_ARRVAL_P(except)));

	zend_hash_internal_pointer_reset(Z_ARRVAL_P(except));
	while (zend_hash_get_current_data(Z_ARRVAL_P(except), (void **)&entry) == SUCCESS) {
		MAKE_STD_ZVAL(data);
		Z_TYPE_P(data) = IS_LONG;
		Z_LVAL_P(data) = 0; 
		if (Z_TYPE_PP(entry) == IS_LONG) {
			zend_hash_index_update(Z_ARRVAL_P(except_flip), Z_LVAL_PP(entry), &data, sizeof(data), NULL);
		} else if (Z_TYPE_PP(entry) == IS_STRING) {
			zend_symtable_update(Z_ARRVAL_P(except_flip), Z_STRVAL_PP(entry), Z_STRLEN_PP(entry) + 1, &data, sizeof(data), NULL);
		} else {
			zval_ptr_dtor(&data);
		}
		zend_hash_move_forward(Z_ARRVAL_P(except));
	}

	
	zend_object *zobj = zend_objects_get_address(obj TSRMLS_CC);
	zval **value;
	const char *prop_name, *class_name;
	char *string_key ;
	uint string_key_len;
	ulong num_key;

	array_init(return_value);

	zend_hash_internal_pointer_reset_ex(properties, &pos);
	while (zend_hash_get_current_data_ex(properties, (void **) &value, &pos) == SUCCESS) {
		if (zend_hash_get_current_key_ex(properties, &string_key, &string_key_len, &num_key, 0, &pos) == HASH_KEY_IS_STRING) {
			if (!zend_symtable_exists(Z_ARRVAL_P(except_flip), string_key, string_key_len))
			if (zend_check_property_access(zobj, string_key, string_key_len-1 TSRMLS_CC) == SUCCESS) {
				zend_unmangle_property_name(string_key, string_key_len-1, &class_name, &prop_name);
				Z_ADDREF_PP(value);
				add_assoc_zval_ex(return_value, prop_name, strlen(prop_name) + 1, *value);
			}
		}
		zend_hash_move_forward_ex(properties, &pos);
	}

	/* clean up */
	zval_ptr_dtor(&except_flip);
	if (!is_set_except) {
		zval_ptr_dtor(&except);
	}
}
/* }}} */



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
