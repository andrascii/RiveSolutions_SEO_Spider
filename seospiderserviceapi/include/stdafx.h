#pragma once

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

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

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/shared_ptr.hpp>

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

#ifdef Q_OS_WIN
#include <windows.h>
#include <new.h>
#else
#error You compile this code on unsupported platform!
#endif

using namespace std::chrono_literals;
using std::size_t;

#define D_FUNCTION_DECL(ClassName) ClassName##Private* d_function() const

#define D_FUNCTION_IMPL(ClassName) static ClassName##Private* d_function() \
{ \
    static ClassName##Private d_object; \
    return &d_object; \
}

#define D_FUNCTION(ClassName) ClassName##Private* const d = d_function()

#ifndef QT_DEBUG
#define PRODUCTION
#endif

#ifndef PRODUCTION
#define DEBUG
#endif

#define SEO_SPIDER_SERVICE_API_NAMESPACE_BEGIN namespace SeoSpiderServiceApi {
#define SEO_SPIDER_SERVICE_API_NAMESPACE_END }