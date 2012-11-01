PHP_ARG_ENABLE(haojing, [Whether to enable the "haojing" extension], [ enable-haojing Enable "haojing" extension support])
if test $PHP_HAOJING != "no"; then 
	PHP_SUBST(HAOJING_SHARED_LIBADD) 
	PHP_NEW_EXTENSION(haojing, haojing.c, $ext_shared) 
fi
