//----------------------------------------------------------------//
// Copyright (c) 2010-2011 Zipline Games, Inc. 
// All Rights Reserved. 
// http://getmoai.com
//----------------------------------------------------------------//

#ifndef AKU_LUAEXT_H
#define AKU_LUAEXT_H

#include <moai-core/host.h>

AKU_API void	AKUExtLoadLuafilesystem ();
AKU_API void	AKUExtLoadLuasocket ();

#if MOAI_WITH_CRYPTO
	AKU_API void	AKUExtLoadLuacrypto ();
#endif

#if MOAI_WITH_CURL
	AKU_API void	AKUExtLoadLuacurl ();
#endif

#if MOAI_WITH_SQLITE3
AKU_API void	AKUExtLoadLuasql ();
#endif

#if MOAI_WITH_YAML
	AKU_API void	AKUExtLoadLuayaml	();
#endif

#endif
