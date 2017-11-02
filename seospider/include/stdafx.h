#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

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
#include <sstream>
#include <csignal>

//
// boost
//
#include <boost/functional/hash.hpp>
#include <boost/any.hpp>

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
#include <QScreen>
#include <QItemSelectionModel>
#include <QtSvg>

#include "logger_connection_service_api.h"
#include "common_macro_helpers.h"

using namespace std::chrono_literals;
using std::size_t;

#define theApp static_cast<Application*>(Application::instance())

#define MAKE_STRING(Any) #Any

#ifndef QT_DEBUG
#define PRODUCTION
#endif

#ifndef PRODUCTION
#define DEBUG
#endif

#ifdef Q_OS_WIN
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define INFOLOG		CrawlerEngine::LoggerConnectionServiceApi::instance()->log(CrawlerEngine::LoggerConnectionServiceApi::InformationMessageType,__FUNCTION__,__FILENAME__,__LINE__)
#define WARNINGLOG	CrawlerEngine::LoggerConnectionServiceApi::instance()->log(CrawlerEngine::LoggerConnectionServiceApi::WarningMessageType,__FUNCTION__,__FILENAME__,__LINE__)
#define ERRORLOG	CrawlerEngine::LoggerConnectionServiceApi::instance()->log(CrawlerEngine::LoggerConnectionServiceApi::ErrorMessageType,__FUNCTION__,__FILENAME__,__LINE__)

#ifdef DEBUG
#define DEBUGLOG	CrawlerEngine::LoggerConnectionServiceApi::instance()->log(CrawlerEngine::LoggerConnectionServiceApi::DebugMessageType,__FUNCTION__,__FILENAME__,__LINE__)
#else
#define DEBUGLOG	CrawlerEngine::LoggerConnectionServiceApi::instance()->log(CrawlerEngine::LoggerConnectionServiceApi::EmptyMessageType,__FUNCTION__,__FILENAME__,__LINE__)
#endif

#if defined(Q_OS_WIN)

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <ws2tcpip.h>
#include <mstcpip.h>

#define CANNOT_CREATE_SOCKET INVALID_SOCKET

#endif

#if defined(Q_OS_MACOS) || defined(Q_OS_UNIX)

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

#define SOCKET int
#define CANNOT_CREATE_SOCKET -1

#endif