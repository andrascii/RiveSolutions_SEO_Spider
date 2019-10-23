#pragma once

#define _SCL_SECURE_NO_WARNINGS

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
#include <algorithm>

//
// boost
//

#ifdef Q_OS_WIN

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

#endif

#include <boost/functional/hash.hpp>

#pragma warning(disable:4127)
#pragma warning(disable:745)
#pragma warning(disable:749)
#pragma warning(disable:333)
#pragma warning(disable:317)
#pragma warning(disable:597)
#pragma warning(disable:4091)
#pragma warning(disable:4251)
#pragma warning(disable:4702)
#pragma warning(disable:4456)

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
#include <QMutex>
#include <QMutexLocker>
#include <QReadWriteLock>
#include <QSemaphore>
#include <QWaitCondition>
#include <QQueue>
#include <QDebug>
#include <QProcess>
#include <QDateTime>
#include <QUuid>
#include <QDialog>
#include <QMessageBox>
#include <QLocalServer>
#include <QLocalSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QWebEngineView>
#include <QSharedMemory>
#include <QBuffer>
#include <QWebEngineSettings>

#ifdef Q_OS_WIN

//https://developercommunity.visualstudio.com/content/problem/185399/error-c2760-in-combaseapih-with-windows-sdk-81-and.html
struct IUnknown;

#include <windows.h>
#include <dbghelp.h>
#endif

#ifdef QT_DEBUG
#define VERIFY(Connection) Q_ASSERT(Connection)
#else
#define VERIFY(Connection) Connection
#endif
