#pragma once

#define _SCL_SECURE_NO_WARNINGS
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#pragma warning(disable:4127)
#pragma warning(disable:745)
#pragma warning(disable:749)
#pragma warning(disable:333)
#pragma warning(disable:317)
#pragma warning(disable:597)
#pragma warning(disable:4251)
#pragma warning(disable:4702)
#pragma warning(disable:4456)

#define CHECK_LICENSE

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
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QAuthenticator>
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
#include <QAbstractNetworkCache>
#include <QStringBuilder>
#include <QXmlQuery>
#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QBuffer>
#include <QPointer>
#include <QSharedMemory>
#include <QDesktopServices>
#include <QDir>
#include <QUrlQuery>
#include <QJsonDocument>

//
// C/C++
//
#include <cstdlib>
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
#include <optional>

#include <VMProtectSDK.h>
#include <curl/curl.h>

//
// boost
//

namespace std
{

//
// In C++11 std::unary_function template are deprecated
// In C++17 it was removed
// We use /std:c++latest flag for compile this project
// And therefore boost::lexical_cast does not compiled
// Because it uses removed std::unary_function template
//
// DELETE THIS AFTER CHANGING BOOST ON LATER VERSION WHERE IT WILL BE FIXED
//
// http://en.cppreference.com/w/cpp/utility/functional/unary_function
// https://svn.boost.org/trac10/ticket/12972
//

template <class _Arg, class _Result>
struct unary_function
{
	typedef _Arg argument_type;
	typedef _Result result_type;
};

}

#include <boost/functional/hash.hpp>
#include <boost/process/child.hpp>
#include <boost/process/search_path.hpp>
#include <boost/process/cmd.hpp>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "common_macro_helpers.h"
#include "counted_ptr.h"

using namespace std::chrono_literals;
using std::size_t;

#ifndef QT_DEBUG
#define PRODUCTION
#endif

#ifndef PRODUCTION
#define DEBUG
#endif

#if defined(Q_OS_WIN)

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

using namespace std::chrono_literals;