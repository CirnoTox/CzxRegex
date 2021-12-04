#include "Error.h"
istream& operator>>(istream&is,Error& e)
{
	string errorStr;
	if (is) {
		is >> errorStr;
		e.sErrorLog.push(errorStr);
	}
	return is;
}

ostream& operator<<(ostream& os, Error&e)
{
	for (; !e.sErrorLog.empty();) {
		os << e.sErrorLog.top()<<string("\n");
		e.sErrorLog.pop();
	}
	return os;
}

Error& operator<<(Error& e, string str)
{
	e.sErrorLog.push(str);
	return e;
}
