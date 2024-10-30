#ifndef NSTEAM
#include <steam/steam_api.h>
#include <steam/steam_api_flat.h>
#include "quickjs.h"

#define countof(a) (sizeof(a)/sizeof(*(a)))

#define JSCALL(NAME) static JSValue js_##NAME (JSContext *js, JSValueConst self, int argc, JSValueConst *argv)

ISteamUserStats *stats = NULL;
ISteamApps *app = NULL;
ISteamRemoteStorage *remote = NULL;
ISteamUGC *ugc = NULL;

JSCALL(achievement_get32) {
  const char *str = JS_ToCString(js, argv[0]);
  int32 data;
  SteamAPI_ISteamUserStats_GetStatInt32(stats, str, &data);
  JS_FreeCString(js, str);
  return JS_NewInt32(js, data);
}

static JSValue js_achievement_get(JSContext *js, JSValueConst self, int argc, JSValueConst *argv) {
  const char *str = JS_ToCString(js, argv[0]);
  bool data;
  SteamAPI_ISteamUserStats_GetAchievement(stats, str, &data);
  JS_FreeCString(js, str);
  return JS_NewBool(js, data);
}

static JSValue js_achievement_set(JSContext *js, JSValueConst self, int argc, JSValueConst *argv) {
  const char *str = JS_ToCString(js, argv[0]);
  bool set = SteamAPI_ISteamUserStats_SetAchievement(stats, str);
  JS_FreeCString(js, str);
  return JS_NewBool(js, set);
}

static JSValue js_achievement_num(JSContext *js, JSValueConst self, int argc, JSValueConst *argv) {
  return JS_NewUint32(js, SteamAPI_ISteamUserStats_GetNumAchievements(stats));
}

static JSValue js_achievement_clear(JSContext *js, JSValueConst self, int argc, JSValueConst *argv) {
  const char *achievement = JS_ToCString(js,argv[0]);
  bool done = SteamAPI_ISteamUserStats_ClearAchievement(stats, achievement);
  JS_FreeCString(js,achievement);
  if (!done) return JS_EXCEPTION;
  
  return JS_UNDEFINED;
}

static const JSCFunctionListEntry js_achievement_funcs[] = {
  JS_CFUNC_DEF("clear", 1, js_achievement_clear),
  JS_CFUNC_DEF("get32", 2, js_achievement_get32),
  JS_CFUNC_DEF("get", 2, js_achievement_get),
  JS_CFUNC_DEF("set", 1, js_achievement_set),
  JS_CFUNC_DEF("num", 0, js_achievement_num)
};

static JSValue js_cloud_app_enabled(JSContext *js, JSValueConst self, int argc, JSValueConst *argv) {
  return JS_NewBool(js, SteamAPI_ISteamRemoteStorage_IsCloudEnabledForApp(remote));
}

static JSValue js_cloud_enable(JSContext *js, JSValueConst self, int argc, JSValueConst *argv) {
  SteamAPI_ISteamRemoteStorage_SetCloudEnabledForApp(remote, JS_ToBool(js, argv[0]));
}

static JSValue js_cloud_account_enabled(JSContext *js, JSValueConst self, int argc, JSValueConst *argv) {
  return JS_NewBool(js, SteamAPI_ISteamRemoteStorage_IsCloudEnabledForAccount(remote));
}

static const JSCFunctionListEntry js_cloud_funcs[] = {
  JS_CFUNC_DEF("app_enabled", 0, js_cloud_app_enabled),
  JS_CFUNC_DEF("account_enabled", 0, js_cloud_account_enabled),
  JS_CFUNC_DEF("enable", 1, js_cloud_enable)
};

static JSValue js_app_owner(JSContext *js, JSValueConst self, int argc, JSValueConst *argv) {
  uint64_t own = SteamAPI_ISteamApps_GetAppOwner(app);
  return JS_NewBigUint64(js, own);
}

static const JSCFunctionListEntry js_app_funcs[] = {
  JS_CFUNC_DEF("owner", 0, js_app_owner)
};

static int js_steam_init(JSContext *js, JSModuleDef *m)
{
  JSValue steam = JS_NewObject(js);
  
  JSValue achievements = JS_NewObject(js);
  JS_SetPropertyFunctionList(js, achievements, js_achievement_funcs, countof(js_achievement_funcs));
  JS_SetPropertyStr(js, steam, "achievements", achievements);

  JSValue js_app = JS_NewObject(js);
  JS_SetPropertyFunctionList(js, js_app, js_app_funcs, countof(js_app_funcs));
  JS_SetPropertyStr(js, steam, "app", js_app);

  JSValue cloud = JS_NewObject(js);
  JS_SetPropertyFunctionList(js, cloud, js_cloud_funcs, countof(js_cloud_funcs));
  JS_SetPropertyStr(js, steam, "cloud", cloud);

  SteamErrMsg err;
  SteamAPI_InitEx(&err);
  stats = SteamAPI_SteamUserStats();
  app = SteamAPI_SteamApps();
  remote = SteamAPI_SteamRemoteStorage();
  ugc = SteamAPI_SteamUGC();

  JS_SetModuleExport(js, m, "steam", steam);
}

#ifdef JS_SHARED_LIBRARY
#define JS_INIT_MODULE js_init_module
#else
#define JS_INIT_MODULE js_init_module_steam
#endif

extern "C" JSModuleDef *JS_INIT_MODULE(JSContext *js, const char *module_name)
{
  JSModuleDef *m;
  m = JS_NewCModule(js, module_name, js_steam_init);
  if (!m) return NULL;
  return m;
}

#endif
