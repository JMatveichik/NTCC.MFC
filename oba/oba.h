#pragma once

#if defined(ZHOPA)

#if defined(OBA_EXPORTS)
#define OBADLLIMPEXP __declspec(dllexport)
#else
#define OBADLLIMPEXP __declspec(dllimport)
#endif

#endif 

#include "basic.h"