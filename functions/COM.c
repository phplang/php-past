/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2000 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
 */


/* !!!! experimental, use at your own risk! !!!! 
   $Id: COM.c,v 1.11 2000/02/20 18:40:33 eschmid Exp $ */

#if WIN32|WINNT

#include "php.h"
#include "internal_functions.h"
#include "php3_list.h"
#include "php3_COM.h"

#include "objbase.h"

static int le_idispatch;

function_entry COM_functions[] = {
	{"com_load",		php3_COM_load,		NULL},
	{"com_invoke",		php3_COM_invoke,	NULL},
	{"com_propget",		php3_COM_propget,	NULL},
	{"com_get",			php3_COM_propget,	NULL},
	{"com_propput",		php3_COM_propput,	NULL},
	{"com_propset",		php3_COM_propput,	NULL},
	{"com_set",			php3_COM_propput,	NULL},
	{NULL, NULL, NULL}
};

php3_module_entry COM_module_entry = {
	"Win32 COM", COM_functions, php3_minit_COM, php3_mshutdown_COM, NULL, NULL, NULL, STANDARD_MODULE_PROPERTIES
};


static char *_php3_COM_error_message(HRESULT hr)
{
	void *pMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &pMsgBuf,
		0,
		NULL);

	return pMsgBuf;
}


static OLECHAR *_php3_C_str_to_unicode(char *C_str, uint strlen)
{
	OLECHAR *unicode_str = (OLECHAR *) emalloc(sizeof(OLECHAR)*(strlen+1));
	OLECHAR *unicode_ptr = unicode_str;

	while (*C_str) {
		*unicode_ptr++ = (unsigned short) *C_str++;
	}
	*unicode_ptr = 0;

	return unicode_str;
}


char *_php3_unicode_to_C_str(OLECHAR *unicode_str, uint *out_length)
{
	uint length = SysStringLen(unicode_str);
	char *C_str = (char *) emalloc(length+1), *p = C_str;

	while (*unicode_str) {
		*p++ = (char) *unicode_str++;
	}
	*p = 0;

	if (out_length) {
		*out_length = length;
	}

	return C_str;
}


static char *_php3_string_from_clsid(CLSID *clsid)
{
	LPOLESTR s_clsid;
	char *clsid_str;

	StringFromCLSID(clsid, &s_clsid);
	clsid_str = _php3_unicode_to_C_str(s_clsid, NULL);
	LocalFree(s_clsid);

	return clsid_str;
}


static void _php3_idispatch_destructor(IDispatch *i_dispatch)
{
	i_dispatch->lpVtbl->Release(i_dispatch);
}

int php3_minit_COM(INIT_FUNC_ARGS)
{
	CoInitialize(NULL);
	le_idispatch = register_list_destructors(_php3_idispatch_destructor,NULL);
	return SUCCESS;
}


int php3_mshutdown_COM(void)
{
	CoUninitialize();
	return SUCCESS;
}


/* {{{ proto int com_load(string module_name)
   Loads a COM module */
void php3_COM_load(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *module_name;
	CLSID clsid;
	HRESULT hr;
	OLECHAR *ProgID;
	IUnknown FAR *i_unknown = NULL;
	IDispatch FAR *i_dispatch = NULL;
	char *error_message;
	char *clsid_str;
	int i;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &module_name)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(module_name);
	ProgID = (OLECHAR *) emalloc(sizeof(OLECHAR)*(module_name->value.str.len+1));
	for (i=0; i<module_name->value.str.len; i++) {
		ProgID[i] = module_name->value.str.val[i];
	}
	ProgID[i] = 0;
	hr=CLSIDFromProgID(ProgID, &clsid);
	efree(ProgID);

	/* obtain CLSID */
	if (FAILED(hr)) {
		error_message = _php3_COM_error_message(hr);
		php3_error(E_WARNING,"Invalid ProgID:  %s\n", error_message);
		LocalFree(error_message);
		RETURN_FALSE;
	}

	/* obtain IUnknown */
	if (FAILED(hr=CoCreateInstance(&clsid, NULL, CLSCTX_ALL, &IID_IUnknown, (void FAR* FAR*) &i_unknown))) {
		error_message = _php3_COM_error_message(hr);
		clsid_str = _php3_string_from_clsid(&clsid);
		php3_error(E_WARNING,"Unable to obtain IUnknown interface for CLSID %s:  %s",clsid_str,error_message);
		LocalFree(error_message);
		efree(clsid_str);
		RETURN_FALSE;
	}

	/* obtain IDispatch */
	if (FAILED(hr=i_unknown->lpVtbl->QueryInterface(i_unknown, &IID_IDispatch, (void FAR* FAR*) &i_dispatch))) {
		error_message = _php3_COM_error_message(hr);
		clsid_str = _php3_string_from_clsid(&clsid);
		php3_error(E_WARNING,"Unable to obtain IDispatch interface for CLSID %s:  %s",clsid_str,error_message);
		LocalFree(error_message);
		efree(clsid_str);
		RETURN_FALSE;
	}
	i_unknown->lpVtbl->Release(i_unknown);

	RETURN_LONG(php3_list_insert(i_dispatch,le_idispatch));
}
/* }}} */



static void _php3_variant_to_pval(VARIANTARG *var_arg, pval *pval_arg)
{
	switch (var_arg->vt) {
	case VT_EMPTY:
		var_uninit(pval_arg);
		break;
	case VT_UI1:
		pval_arg->value.lval = (long) var_arg->bVal;
		pval_arg->type = IS_LONG;
		break;
	case VT_I2:
		pval_arg->value.lval = (long) var_arg->iVal;
		pval_arg->type = IS_LONG;
		break;
	case VT_I4:
		pval_arg->value.lval = var_arg->lVal;
		pval_arg->type = IS_LONG;
		break;
	case VT_R4:
		pval_arg->value.dval = (double) var_arg->fltVal;
		pval_arg->type = IS_DOUBLE;
		break;
	case VT_R8:
		pval_arg->value.dval = var_arg->dblVal;
		pval_arg->type = IS_DOUBLE;
		break;
	case VT_BOOL:
		if (var_arg->boolVal & 0xFFFF) {
			pval_arg->value.lval = 1;
		} else {
			pval_arg->value.lval = 0;
		}
		pval_arg->type = IS_LONG;
		break;
	case VT_BSTR:
		pval_arg->value.str.val = _php3_unicode_to_C_str(var_arg->bstrVal, &pval_arg->value.str.len);
		pval_arg->type = IS_STRING;
		break;
	default:
		php3_error(E_WARNING,"Unsupported variant type");
		var_uninit(pval_arg);
		break;
	}
}


static void _php3_pval_to_variant(pval *pval_arg, VARIANTARG *var_arg)
{
	OLECHAR *unicode_str;

	switch (pval_arg->type) {
	case IS_OBJECT:
	case IS_ARRAY:
		var_arg->vt = VT_EMPTY;
		break;
	case IS_LONG:
		var_arg->vt = VT_I4;  /* assuming 32-bit platform */
		var_arg->lVal = pval_arg->value.lval;
		break;
	case IS_DOUBLE:
		var_arg->vt = VT_R8;  /* assuming 64-bit double precision */
		var_arg->dblVal = pval_arg->value.dval;
		break;
	case IS_STRING:
		unicode_str = _php3_C_str_to_unicode(pval_arg->value.str.val, pval_arg->value.str.len);
		var_arg->bstrVal = SysAllocString(unicode_str);
		var_arg->vt = VT_BSTR;
		efree(unicode_str);
		break;
	}
}


/* {{{ proto mixed com_invoke(int module, string handler_name [, mixed arg [, ...]])
   Invokes a COM module */
void php3_COM_invoke(INTERNAL_FUNCTION_PARAMETERS)
{
	pval **arguments;
	IDispatch *i_dispatch;
	int type;
	DISPID dispid;
	HRESULT hr;
	OLECHAR *funcname;
	char *error_message;
	VARIANTARG *variant_args;
	int arg_count = ARG_COUNT(ht), current_arg, current_variant;
	DISPPARAMS dispparams;
	VARIANTARG var_result;

	if (arg_count<2) {
		WRONG_PARAM_COUNT;
	}
	arguments = (pval **) emalloc(sizeof(pval *) * arg_count);
	if (getParametersArray(ht, arg_count, arguments)==FAILURE) {
		efree(arguments);
		RETURN_FALSE;
	}

	/* obtain i_dispatch interface */
	convert_to_long(arguments[0]);
	i_dispatch = php3_list_find(arguments[0]->value.lval,&type);
	if (!i_dispatch || (type!=le_idispatch)) {
		php3_error(E_WARNING,"%d is not a COM object handler", arguments[0]->value.lval);
		efree(arguments);
		RETURN_FALSE;
	}

	/* obtain property/method handler */
	convert_to_string(arguments[1]);
	funcname = _php3_C_str_to_unicode(arguments[1]->value.str.val, arguments[1]->value.str.len);

	hr = i_dispatch->lpVtbl->GetIDsOfNames(i_dispatch, &IID_NULL, &funcname,
											1, LOCALE_SYSTEM_DEFAULT, &dispid);

	if (FAILED(hr)) {
		error_message = _php3_COM_error_message(hr);
		php3_error(E_WARNING,"Unable to lookup %s:  %s\n", arguments[1]->value.str.val, error_message);
		LocalFree(error_message);
		efree(funcname);
		efree(arguments);
		RETURN_FALSE;
	}

	arg_count -= 2;
	variant_args = (VARIANTARG *) emalloc(sizeof(VARIANTARG)*arg_count);

	for (current_arg=0; current_arg<arg_count; current_arg++) {
		current_variant = arg_count - current_arg - 1;
		_php3_pval_to_variant(arguments[2+current_arg], &variant_args[current_variant]);
	}


	dispparams.rgvarg = variant_args;
	dispparams.rgdispidNamedArgs = NULL;
	dispparams.cArgs = arg_count;
	dispparams.cNamedArgs = 0;

	hr = i_dispatch->lpVtbl->Invoke(i_dispatch, dispid, &IID_NULL,
									LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD,
									&dispparams, &var_result, NULL, 0);

	if (FAILED(hr)) {
		error_message = _php3_COM_error_message(hr);
		php3_error(E_WARNING,"Invoke() failed:  %s\n", error_message);
		LocalFree(error_message);
		efree(funcname);
		efree(arguments);
		efree(variant_args);
		RETURN_FALSE;
	}

	_php3_variant_to_pval(&var_result, return_value);

	efree(arguments);
	efree(variant_args);
	efree(funcname);
}
/* }}} */


/* {{{ proto mixed com_get(int module, string property_name)
   An alias for com_propget */
/* }}} */

/* {{{ proto mixed com_propget(int module, string property_name)
   Gets properties from a COM module */
void php3_COM_propget(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg_idispatch, *arg_property;
	IDispatch *i_dispatch;
	int type;
	DISPID dispid;
	HRESULT hr;
	OLECHAR *funcname;
	char *error_message;
	VARIANTARG var_result;
	DISPPARAMS dispparams;

	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &arg_idispatch, &arg_property)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* obtain i_dispatch interface */
	convert_to_long(arg_idispatch);
	i_dispatch = php3_list_find(arg_idispatch->value.lval,&type);
	if (!i_dispatch || (type!=le_idispatch)) {
		php3_error(E_WARNING,"%d is not a COM object handler", arg_idispatch->value.lval);
	}

	/* obtain property/method handler */
	convert_to_string(arg_property);
	funcname = _php3_C_str_to_unicode(arg_property->value.str.val, arg_property->value.str.len);

	hr = i_dispatch->lpVtbl->GetIDsOfNames(i_dispatch, &IID_NULL, &funcname,
											1, LOCALE_SYSTEM_DEFAULT, &dispid);

	if (FAILED(hr)) {
		error_message = _php3_COM_error_message(hr);
		php3_error(E_WARNING,"Unable to lookup %s:  %s\n", arg_property->value.str.val, error_message);
		LocalFree(error_message);
		efree(funcname);
		RETURN_FALSE;
	}

	dispparams.cArgs = 0;
	dispparams.cNamedArgs = 0;

	hr = i_dispatch->lpVtbl->Invoke(i_dispatch, dispid, &IID_NULL,
									LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET,
									&dispparams, &var_result, NULL, 0);

	if (FAILED(hr)) {
		error_message = _php3_COM_error_message(hr);
		php3_error(E_WARNING,"Invoke() failed:  %s\n", error_message);
		LocalFree(error_message);
		efree(funcname);
		RETURN_FALSE;
	}

	_php3_variant_to_pval(&var_result, return_value);

	efree(funcname);
}
/* }}} */


/* {{{ proto bool com_set(int module, string property_name, mixed value)
   An alias for com_propput */
/* }}} */

/* {{{ proto bool com_propset(int module, string property_name, mixed value)
   An alias for com_propput */
/* }}} */

/* {{{ proto bool com_propput(int module, string property_name, mixed value)
   Puts the properties for a module */
void php3_COM_propput(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg_idispatch, *arg_property, *arg_new_value;
	IDispatch *i_dispatch;
	int type;
	DISPID dispid;
	HRESULT hr;
	OLECHAR *funcname;
	char *error_message;
	VARIANTARG new_value;
	DISPPARAMS dispparams;
	DISPID mydispid = DISPID_PROPERTYPUT;

	if (ARG_COUNT(ht)!=3 || getParameters(ht, 3, &arg_idispatch, &arg_property, &arg_new_value)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* obtain i_dispatch interface */
	convert_to_long(arg_idispatch);
	i_dispatch = php3_list_find(arg_idispatch->value.lval,&type);
	if (!i_dispatch || (type!=le_idispatch)) {
		php3_error(E_WARNING,"%d is not a COM object handler", arg_idispatch->value.lval);
	}

	/* obtain property/method handler */
	convert_to_string(arg_property);
	funcname = _php3_C_str_to_unicode(arg_property->value.str.val, arg_property->value.str.len);

	hr = i_dispatch->lpVtbl->GetIDsOfNames(i_dispatch, &IID_NULL, &funcname,
											1, LOCALE_SYSTEM_DEFAULT, &dispid);

	if (FAILED(hr)) {
		error_message = _php3_COM_error_message(hr);
		php3_error(E_WARNING,"Unable to lookup %s:  %s\n", arg_property->value.str.val, error_message);
		LocalFree(error_message);
		efree(funcname);
		RETURN_FALSE;
	}

	_php3_pval_to_variant(arg_new_value, &new_value);
	dispparams.rgvarg = &new_value;
	dispparams.rgdispidNamedArgs = &mydispid;
	dispparams.cArgs = 1;
	dispparams.cNamedArgs = 1;

	hr = i_dispatch->lpVtbl->Invoke(i_dispatch, dispid, &IID_NULL,
									LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUT,
									&dispparams, NULL, NULL, 0);

	if (FAILED(hr)) {
		error_message = _php3_COM_error_message(hr);
		php3_error(E_WARNING,"Invoke() failed:  %s\n", error_message);
		LocalFree(error_message);
		efree(funcname);
		RETURN_FALSE;
	}

	efree(funcname);

	RETURN_TRUE;
}
/* }}} */

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
