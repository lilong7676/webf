/*
 * Copyright (C) 2019 Alibaba Inc. All rights reserved.
 * Author: Kraken Team.
 */

#ifndef KRAKEN_BRIDGE_EXPORT_H
#define KRAKEN_BRIDGE_EXPORT_H

#include <cstdint>
#define KRAKEN_EXPORT extern "C" __attribute__((visibility("default"))) __attribute__((used))

struct Screen {
  double width;
  double height;
};
using AsyncCallback = void (*)(void *context, int32_t contextIndex, const char *errmsg);
using AsyncRAFCallback = void (*)(void *context, int32_t contextIndex, double result, const char *errmsg);
using AsyncModuleCallback = void (*)(void *context, int32_t contextIndex, char *json, void *data);
using AsyncBlobCallback = void (*)(void *context, int32_t contextIndex, const char *error, uint8_t *bytes,
                                   int32_t length);
typedef const char *(*InvokeUIManager)(void *context, int32_t contextIndex, const char *json);
typedef const char *(*InvokeModule)(void *context, int32_t contextIndex, const char *, AsyncModuleCallback callback);
typedef void (*RequestBatchUpdate)(void *context, int32_t contextIndex, AsyncCallback callback);
typedef void (*ReloadApp)(void *context, int32_t contextIndex);
typedef int32_t (*SetTimeout)(void *context, int32_t contextIndex, AsyncCallback callback, int32_t timeout);
typedef int32_t (*SetInterval)(void *context, int32_t contextIndex, AsyncCallback callback, int32_t timeout);
typedef int32_t (*RequestAnimationFrame)(void *context, int32_t contextIndex, AsyncRAFCallback callback);
typedef void (*ClearTimeout)(void *context, int32_t contextIndex, int32_t timerId);
typedef void (*CancelAnimationFrame)(void *context, int32_t contextIndex, int32_t id);
typedef Screen *(*GetScreen)(void *context, int32_t contextIndex);
typedef double (*DevicePixelRatio)(void *context, int32_t contextIndex);
typedef const char *(*PlatformBrightness)(void *context, int32_t contextIndex);
typedef void (*OnPlatformBrightnessChanged)(void *context, int32_t contextIndex);
typedef void (*ToBlob)(void *context, int32_t contextIndex, AsyncBlobCallback blobCallback, int32_t elementId,
                       double devicePixelRatio);
typedef void (*OnJSError)(void *context, int32_t contextIndex, const char *);

KRAKEN_EXPORT
void *initJSContextPool(int poolSize);
KRAKEN_EXPORT
void disposeContext(void *context, int32_t contextIndex);
KRAKEN_EXPORT
int32_t allocateNewContext();
KRAKEN_EXPORT
void *getJSContext(int32_t contextIndex);
KRAKEN_EXPORT
int32_t checkContextIndex(int32_t contextIndex);
KRAKEN_EXPORT
int32_t checkContext(void *context, int32_t contextIndex);
KRAKEN_EXPORT
void freezeContext(void *context, int32_t contextIndex);
KRAKEN_EXPORT
void unfreezeContext(void *context, int32_t contextIndex);
KRAKEN_EXPORT
void evaluateScripts(void *context, int32_t contextIndex, const char *code, const char *bundleFilename, int startLine);

KRAKEN_EXPORT
void reloadJsContext(void *context, int32_t contextIndex);
KRAKEN_EXPORT
void invokeEventListener(void *context, int32_t contextIndex, int32_t type, const char *json);
KRAKEN_EXPORT
Screen *createScreen(double width, double height);

KRAKEN_EXPORT
void registerInvokeUIManager(InvokeUIManager invokeUIManager);
KRAKEN_EXPORT
void registerInvokeModule(InvokeModule invokeUIManager);
KRAKEN_EXPORT
void registerRequestBatchUpdate(RequestBatchUpdate requestBatchUpdate);
KRAKEN_EXPORT
void registerReloadApp(ReloadApp reloadApp);
KRAKEN_EXPORT
void registerSetTimeout(SetTimeout setTimeout);
KRAKEN_EXPORT
void registerSetInterval(SetInterval setInterval);
KRAKEN_EXPORT
void registerClearTimeout(ClearTimeout clearTimeout);
KRAKEN_EXPORT
void registerRequestAnimationFrame(RequestAnimationFrame requestAnimationFrame);
KRAKEN_EXPORT
void registerCancelAnimationFrame(CancelAnimationFrame cancelAnimationFrame);
KRAKEN_EXPORT
void registerGetScreen(GetScreen getScreen);
KRAKEN_EXPORT
void registerDevicePixelRatio(DevicePixelRatio devicePixelRatio);
KRAKEN_EXPORT
void registerPlatformBrightness(PlatformBrightness platformBrightness);
KRAKEN_EXPORT
void registerOnPlatformBrightnessChanged(OnPlatformBrightnessChanged onPlatformBrightnessChanged);
KRAKEN_EXPORT
void registerToBlob(ToBlob toBlob);

#endif // KRAKEN_BRIDGE_EXPORT_H
