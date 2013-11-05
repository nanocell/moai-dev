// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include <moai-luaext/host.h>

extern "C" {	
	extern int luaopen_lfs				( lua_State *L );
	extern int luapreload_fullluasocket ( lua_State *L );

#if MOAI_WITH_YAML
	extern int luaopen_yaml				      ( lua_State *L );
#endif
}

//================================================================//
// aku-luaext
//================================================================//

//----------------------------------------------------------------//
void AKUExtLoadLuafilesystem () {

	lua_State* state = AKUGetLuaState ();
	luaopen_lfs ( state );
}

//----------------------------------------------------------------//
void AKUExtLoadLuasocket () {

	lua_State* state = AKUGetLuaState ();
	luapreload_fullluasocket ( state );
}

//----------------------------------------------------------------//
#if MOAI_WITH_CRYPTO
extern "C" {
	extern int luaopen_crypto			( lua_State *L );
}

void AKUExtLoadLuacrypto () {

	lua_State* state = AKUGetLuaState ();
	luaopen_crypto ( state );
}
#endif

//----------------------------------------------------------------//
#if MOAI_WITH_CURL
extern "C" {
	extern int luaopen_luacurl			( lua_State *L );
}

void AKUExtLoadLuacurl () {

	lua_State* state = AKUGetLuaState ();
	luaopen_luacurl ( state );
}
#endif 

//----------------------------------------------------------------//
#if MOAI_WITH_SQLITE3
extern "C" {
	extern int luaopen_luasql_sqlite3	( lua_State *L );
}
void AKUExtLoadLuasql () {

	lua_State* state = AKUGetLuaState ();
	luaopen_luasql_sqlite3 ( state );
}
#endif

//----------------------------------------------------------------//
#if MOAI_WITH_YAML

void AKUExtLoadLuayaml () {
	lua_State* state = AKUGetLuaState ();
	luaopen_yaml( state );
}

#endif