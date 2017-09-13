#pragma once

#ifdef EXPORT
#define WATCH_DOG_API_EXPORT Q_DECL_EXPORT
#else
#define WATCH_DOG_API_EXPORT Q_DECL_IMPORT
#endif

extern "C" WATCH_DOG_API_EXPORT void initWatchDogApi();