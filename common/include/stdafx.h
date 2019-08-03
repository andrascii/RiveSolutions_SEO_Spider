#pragma once

#ifndef Q_OS_WIN
#define _countof(array) (sizeof(array) / sizeof(array[0]))
#endif

//https://developercommunity.visualstudio.com/content/problem/185399/error-c2760-in-combaseapih-with-windows-sdk-81-and.html
struct IUnknown;

#define _CRT_SECURE_NO_WARNINGS

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
#include <algorithm>

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
#include <QBuffer>
#include <QStack>
#include <QNetworkInterface>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "common_macro_helpers.h"

using namespace std::chrono_literals;
using std::size_t;

#ifndef QT_DEBUG
#define PRODUCTION
#endif
