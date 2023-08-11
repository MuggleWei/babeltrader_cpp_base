/******************************************************************************
 *  @file         macro.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-10
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp macro
 *****************************************************************************/

#ifndef BABELTRADER_CPP_MACRO_H_
#define BABELTRADER_CPP_MACRO_H_

#include "babeltrader/cpp/config.h"

#if MUGGLE_PLATFORM_WINDOWS && defined(BABELTRADER_CPP_USE_DLL)
	#ifdef BABELTRADER_CPP_EXPORTS
		#define BABELTRADER_CPP_EXPORT __declspec(dllexport)
	#else
		#define BABELTRADER_CPP_EXPORT __declspec(dllimport)
	#endif
#else
	#define BABELTRADER_CPP_EXPORT
#endif

#define NS_BABELTRADER_BEGIN namespace babeltrader {
#define NS_BABELTRADER_END   }
#define USING_NS_BABELTRADER using namespace babeltrader

#endif // !BABELTRADER_CPP_MACRO_H_
