#include "hazard.h"

atomic<DWORD>  tlsrlist::tlskey(0);

atomic<hprecord*> hprecord::head(NULL);

atomic<hazard*> hazard::s_p(NULL);