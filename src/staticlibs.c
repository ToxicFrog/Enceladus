#include <lua.h>

// If you want to 'bake' any lua binary modules into this enceladus build, this
// is your hook; it will be called just after luaL_openlibs during init.
// This is your chance to call luaopen_lfs or whatever else you need.

void enceladus_init_static_libs(lua_State * L) {}
