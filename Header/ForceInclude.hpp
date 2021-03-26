#pragma once

#include <string>

#ifdef _DEBUG

static void err_msg(const char *const file, const char *const function, const int line, const char *const msg) {
	char errmsgbuf[1024];
	snprintf(errmsgbuf, sizeof(errmsgbuf), "%s\n�t�@�C����:%s\n�֐���:%s\nthrow:%d�s��\n", msg, file, function, line);
	throw errmsgbuf;
}

#define mythrow(msg) err_msg(__FILE__, __FUNCTION__, __LINE__, msg)

#else

static void err_msg(const char *const function, const int line, const char *const msg) {
	char errmsgbuf[256];
	snprintf(errmsgbuf, sizeof(errmsgbuf), "%s\n�֐���:%s\nthrow:%d�s��\n", msg, function, line);
	throw errmsgbuf;
}

#define mythrow(msg) err_msg(__FUNCTION__, __LINE__, msg)

#endif // _DEBUG

#define FORCEEND throw "FORCEEND"

#define TO_STRING(VariableName) #VariableName

#define TYPENAME(type) typeid(type).name()