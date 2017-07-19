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

//
// boost
//
#include <boost/functional/hash.hpp>

//
// Qt
//
#include <QObject>
#include <QThread>
#include <QWidget>
#include <QDialog>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QTableView>
#include <QMessageBox>
#include <QTextCodec>
#include <QAbstractListModel>
#include <QKeyEvent>
#include <QMetaObject>
#include <QMetaMethod>
#include <QMetaType>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QTimer>
#include <QTcpSocket>
#include <QTcpServer>
#include <QAbstractItemDelegate>
#include <QMutex>
#include <QMutexLocker>
#include <QReadWriteLock>
#include <QSemaphore>
#include <QWaitCondition>
#include <QQueue>
#include <QDebug>
#include <QStyledItemDelegate>
#include <QProcess>
#include <QDateTime>
#include <QLineEdit>
#include <QScrollBar>

#ifdef Q_OS_WIN
#include <windows.h>
#else
#error You compile this code on unsupported operating system!
#endif

#include "logger_connection_service_api.h"

using namespace std::chrono_literals;
using std::size_t;

#define theApp static_cast<Application*>(Application::instance())

#define MAKE_STRING(Any) #Any

#if defined(NDEBUG)
#define PRODUCTION
#endif

#if !defined(PRODUCTION)
#define DEBUG
#endif

#if !defined(PRODUCTION)
#define VERIFY(Connection) assert(Connection)
#else
#define VERIFY(Connection) Connection
#endif

#ifdef Q_OS_WIN
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define INFOLOG(...) LoggerConnectionServiceApi::instance()->log(LoggerConnectionServiceApi::InformationMessageType,__FUNCTION__,__FILENAME__,__LINE__, __VA_ARGS__)

#ifdef DEBUG
#define DEBUGLOG(...) LoggerConnectionServiceApi::instance()->log(LoggerConnectionServiceApi::DebugMessageType,__FUNCTION__,__FILENAME__,__LINE__, __VA_ARGS__)
#else
#define DEBUGLOG
#endif

#define WARNINGLOG(...) LoggerConnectionServiceApi::instance()->log(LoggerConnectionServiceApi::WarningMessageType,__FUNCTION__,__FILENAME__,__LINE__,__VA_ARGS__)
#define ERRORLOG(...) LoggerConnectionServiceApi::instance()->log(LoggerConnectionServiceApi::ErrorMessageType,__FUNCTION__,__FILENAME__,__LINE__, __VA_ARGS__)

inline void debugBreak()
{
#ifdef DEBUG

#ifdef Q_OS_WIN
	__debugbreak();
#else
	char* p = nullptr;
	*p = 0;
#endif

#endif
}