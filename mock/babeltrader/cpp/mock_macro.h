#ifndef BABELTRADER_CPP_MOCK_MACRO_H_
#define BABELTRADER_CPP_MOCK_MACRO_H_

#include "babeltrader/cpp/babeltrader_cpp.h"

#if MUGGLE_PLATFORM_WINDOWS && defined(BABELTRADER_CPP_USE_DLL)
	#ifdef BABELTRADER_CPP_MOCK_EXPORTS
		#define BABELTRADER_CPP_MOCK_EXPORT __declspec(dllexport)
	#else
		#define BABELTRADER_CPP_MOCK_EXPORT __declspec(dllimport)
	#endif
#else
	#define BABELTRADER_CPP_MOCK_EXPORT
#endif

#endif // !BABELTRADER_CPP_MOCK_MACRO_H_
