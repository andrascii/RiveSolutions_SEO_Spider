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
#include <sstream>

//
// boost
//
#include <boost/functional/hash.hpp>
//#include <boost/regex.hpp>
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

#include "application.h"

#ifdef Q_OS_WIN
#include <windows.h>
#else
#error You compile this code on unsupported platform!
#endif

#include "logger_connection_service_api.h"
#include "common_macro_helpers.h"

using namespace std::chrono_literals;
using std::size_t;

#define theApp static_cast<Application*>(Application::instance())

#define MAKE_STRING(Any) #Any

#ifndef QT_DEBUG
#define PRODUCTION
#endif

#ifdef Q_OS_WIN
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define INFOLOG		WebCrawler::LoggerConnectionServiceApi::instance()->log(WebCrawler::LoggerConnectionServiceApi::InformationMessageType,__FUNCTION__,__FILENAME__,__LINE__)
#define WARNINGLOG	WebCrawler::LoggerConnectionServiceApi::instance()->log(WebCrawler::LoggerConnectionServiceApi::WarningMessageType,__FUNCTION__,__FILENAME__,__LINE__)
#define ERRORLOG	WebCrawler::LoggerConnectionServiceApi::instance()->log(WebCrawler::LoggerConnectionServiceApi::ErrorMessageType,__FUNCTION__,__FILENAME__,__LINE__)

#ifdef QT_DEBUG
#define DEBUGLOG	WebCrawler::LoggerConnectionServiceApi::instance()->log(WebCrawler::LoggerConnectionServiceApi::DebugMessageType,__FUNCTION__,__FILENAME__,__LINE__)
#else
#define DEBUGLOG	WebCrawler::LoggerConnectionServiceApi::instance()->log(WebCrawler::LoggerConnectionServiceApi::EmptyMessageType,__FUNCTION__,__FILENAME__,__LINE__)
#endif
