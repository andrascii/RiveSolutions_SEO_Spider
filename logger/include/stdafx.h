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
#include <iostream>

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
#include <QMessageBox>
#include <QTextCodec>
#include <QAbstractListModel>
#include <QKeyEvent>
#include <QMetaObject>
#include <QMetaMethod>
#include <QLocalSocket>
#include <QLocalServer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QProcess>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QTimer>

using namespace std::chrono_literals;
using std::size_t;

#define STRING(Any) #Any

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