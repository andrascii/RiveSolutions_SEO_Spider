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


using namespace std::chrono_literals;
using std::size_t;

#define theApp static_cast<Application*>(Application::instance())

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

#define INFOLOG(tag, text) Logger::instance()->log(Logger::InformationMessageType,tag,text,__FUNCTION__)
#define DEBUGLOG(tag, text) Logger::instance()->log(Logger::DebugMessageType,tag,text,__FUNCTION__)
#define WARNINGLOG(tag, text) Logger::instance()->log(Logger::WarningMessageType,tag,text,__FUNCTION__)
#define ERRORLOG(tag, text) Logger::instance()->log(Logger::ErrorMessageType,tag,text,__FUNCTION__)
