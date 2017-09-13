#pragma once

namespace WatchDogApi
{

class IWatchDogServiceApi;

IWatchDogServiceApi* watchDogServiceApi();
bool lockExceptionFilter();

}