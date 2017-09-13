#pragma once

//
// C/C++
//
#include <vector>
#include <deque>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <future>
#include <chrono>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <atomic>
#include <memory>
#include <utility>
#include <cassert>
#include <functional>
#include <optional>
#include <csignal>

//
// boost
//
#include <boost/functional/hash.hpp>

//
// Qt
//
#include <QThread>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QTableView>
#include <QTextCodec>
#include <QAbstractListModel>
#include <QMetaObject>
#include <QMetaMethod>
#include <QMetaType>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QTimer>
#include <QTcpSocket>
#include <QTcpServer>
#include <QMutex>
#include <QMutexLocker>
#include <QReadWriteLock>
#include <QSemaphore>
#include <QWaitCondition>
#include <QQueue>
#include <QDebug>
#include <QProcess>
#include <QDateTime>

//#include "application.h"

#ifdef Q_OS_WIN
#include <windows.h>
#else
#error You compile this code on unsupported platform!
#endif

#include "logger_connection_service_api.h"
#include "common_macro_helpers.h"

using namespace std::chrono_literals;
using std::size_t;

#ifdef Q_OS_WIN
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define INFOLOG		LoggerConnectionServiceApi::instance()->log(LoggerConnectionServiceApi::InformationMessageType,__FUNCTION__,__FILENAME__,__LINE__)
#define DEBUGLOG	LoggerConnectionServiceApi::instance()->log(LoggerConnectionServiceApi::DebugMessageType,__FUNCTION__,__FILENAME__,__LINE__)
#define WARNINGLOG	LoggerConnectionServiceApi::instance()->log(LoggerConnectionServiceApi::WarningMessageType,__FUNCTION__,__FILENAME__,__LINE__)
#define ERRORLOG	LoggerConnectionServiceApi::instance()->log(LoggerConnectionServiceApi::ErrorMessageType,__FUNCTION__,__FILENAME__,__LINE__)