#pragma once

class CXXX
{

};

CLASS MyService
{
public:
	DESC("abc")
    READONLY(true)
	virtual bool test(void) PURE_VIRTUAL;
};