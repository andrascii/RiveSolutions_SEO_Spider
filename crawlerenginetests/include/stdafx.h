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
#include <QtGlobal>
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
#include <QDir>
#include <QXmlQuery>

// google tests
#include <gtest/gtest.h>

#include "test_environment.h"
#include "tests_crawler.h"

#ifdef Q_OS_WIN
#include <windows.h>
#else
#error You compile this code on unsupported platform!
#endif

#include "common_macro_helpers.h"

using namespace std::chrono_literals;
using std::size_t;

#define testEnv static_cast<TestEnvironment*>(TestEnvironment::instance())

#ifdef Q_OS_WIN
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif