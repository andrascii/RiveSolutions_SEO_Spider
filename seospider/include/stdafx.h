#pragma once

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
#include <QtNetwork/QNetworkProxy>
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
#include <QFontDatabase>
#include <QtGlobal>
#include <QWebEngineView>
#include <QPrinter>
#include <QSystemTrayIcon>

//
// C/C++
//
#include <cstdlib>
#include <crtdbg.h>
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
#include <limits>
#include <fstream>
#include <optional>
#include <windows.h>
#include <VMProtectSDK.h>

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
#include <boost/any.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>

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

#if defined(Q_OS_WIN)

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <ws2tcpip.h>
#include <mstcpip.h>
#include <shellapi.h>
#include <ipexport.h>
#include <icmpapi.h>

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