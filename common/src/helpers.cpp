#include "stdafx.h"
#include "helpers.h"
#include "qaes_encryption.h"

namespace
{

using namespace Common;

constexpr int c_myServiceRawDataParts = 3;

qreal primaryScreenDpi()
{
	return QApplication::screens().at(0)->logicalDotsPerInch();
}

Helpers::ParsedSerialNumberData parseMySerialNumberData(const QByteArray& serialNumber)
{
	const QByteArray& rawData = Common::Helpers::decryptAesKey(serialNumber, QByteArray("111111111111111111111111"));
	const QList<QByteArray> rawDataParts = rawData.split(' ');

	if (rawDataParts.size() != c_myServiceRawDataParts)
	{
		return std::make_tuple(QByteArray(), QByteArray(), QDate());
	}


	const QList<QByteArray> ddMMYY = rawDataParts[2].split('/');
	QDate date = QDate();
	if (ddMMYY.size() == 3)
	{
		const QByteArray dd = ddMMYY[0].size() == 1 ? "0" + ddMMYY[0] : ddMMYY[0];
		const QByteArray MM = ddMMYY[1].size() == 1 ? "0" + ddMMYY[1] : ddMMYY[1];
		const QByteArray yyyy = ddMMYY[2];

		date = QDate::fromString(dd + '/' + MM + '/' + yyyy, "dd/MM/yyyy");
	}

	return std::make_tuple(rawDataParts[0], rawDataParts[1], date);
}

}

namespace Common
{

int Helpers::pointsToPixels(double sizeInPoints, double dpi)
{
	if (dpi == 0)
	{
		dpi = primaryScreenDpi();
	}

	const double pointSizeInPixels = dpi / 72.0;
	return qRound(sizeInPoints * pointSizeInPixels);
}

double Helpers::pixelsToPoints(int sizeInPixels, double dpi)
{
	if (dpi == 0)
	{
		dpi = primaryScreenDpi();
	}

	return static_cast<double>(sizeInPixels) * 72.0 / dpi;
}

std::vector<QMetaMethod> Helpers::allUserSignals(QObject* qobject, QVector<QByteArray> exclude)
{
	const QMetaObject* metaObject = qobject->metaObject();
	std::vector<QMetaMethod> signalsCollection;

	for (int i = 0; i < metaObject->methodCount(); ++i)
	{
		QMetaMethod metaMethod = metaObject->method(i);
		QByteArray methodSignature = metaMethod.methodSignature();

		if (metaMethod.methodType() != QMetaMethod::Signal ||
			methodSignature.contains("destroyed") ||
			methodSignature.contains("objectNameChanged") ||
			exclude.contains(methodSignature.data()))
		{
			continue;
		}

		signalsCollection.push_back(metaMethod);
	}

	return signalsCollection;
}

void Helpers::connectMetaMethods(QObject* sender, QMetaMethod senderMethod, QObject* receiver, QMetaMethod receiverMethod)
{
	ASSERT(senderMethod.methodType() == QMetaMethod::Signal);
	ASSERT(receiverMethod.methodType() == QMetaMethod::Signal || receiverMethod.methodType() == QMetaMethod::Slot);

	QByteArray senderSignalSignature = "2" + senderMethod.methodSignature();
	QByteArray receiverMethodSignature = receiverMethod.methodSignature();

	if (receiverMethod.methodType() == QMetaMethod::Signal)
	{
		receiverMethodSignature = "2" + receiverMethodSignature;
	}
	else
	{
		receiverMethodSignature = "1" + receiverMethodSignature;
	}

	VERIFY(QObject::connect(sender, senderSignalSignature, receiver, receiverMethodSignature));
}

void Helpers::connectSignalsToMetaMethod(QObject* sender, std::vector<QMetaMethod> senderMethods, QObject* receiver, QMetaMethod receiverMethod)
{
	for (const auto& metaMethod : senderMethods)
	{
		connectMetaMethods(sender, metaMethod, receiver, receiverMethod);
	}
}

void Helpers::connectSignalsToMetaMethods(QObject* sender, std::vector<QMetaMethod> senderMethods, QObject* receiver, std::vector<QMetaMethod> receiverMethods)
{
	ASSERT(senderMethods.size() == receiverMethods.size());

	for (std::size_t i = 0, commonSize = senderMethods.size(); i < commonSize; ++i)
	{
		connectMetaMethods(sender, senderMethods[i], receiver, receiverMethods[i]);
	}
}

QMetaMethod Helpers::metaMethod(QObject* qobject, const char* methodSignature)
{
	const QMetaObject* metaObject = qobject->metaObject();
	const int methodIndex = metaObject->indexOfMethod(methodSignature);

	ASSERT(methodIndex != -1 && "Check existence of the specified method");

	return metaObject->method(methodIndex);
}

QMetaMethod Helpers::metaMethodOfSlot(QObject* qobject, const char* slotSignature)
{
	const QMetaObject* metaObject = qobject->metaObject();
	const int slotIndex = metaObject->indexOfSlot(slotSignature);

	ASSERT(slotIndex != -1 && "Check existence of the specified method and that it's actually a slot");

	return metaObject->method(slotIndex);
}

QMetaMethod Helpers::metaMethodOfSignal(QObject* qobject, const char* signalSignature)
{
	const QMetaObject* metaObject = qobject->metaObject();
	const int signalIndex = metaObject->indexOfSignal(signalSignature);

	ASSERT(signalIndex != -1 && "Check existence of the specified method and that it's actually a signal");

	return metaObject->method(signalIndex);
}

void Helpers::connectSignalsToMetaMethodsWithTheSameName(QObject* sender,
	std::vector<QMetaMethod> senderMethods,
	QObject* receiver,
	std::vector<QMetaMethod> receiverMethods)
{
	ASSERT(senderMethods.size() == receiverMethods.size());

	for (std::size_t i = 0, commonSize = senderMethods.size(); i < commonSize; ++i)
	{
		QByteArray methodSignature = senderMethods[i].methodSignature();

		for (std::size_t j = 0; j < commonSize; ++j)
		{
			QByteArray receiverMethodSignature = receiverMethods[j].methodSignature();

			if (methodSignature == receiverMethodSignature)
			{
				connectMetaMethods(sender, senderMethods[i], receiver, receiverMethods[j]);
			}
		}
	}
}

QByteArray Helpers::decryptAesKey(const QByteArray& keyBase64, const QByteArray& helperKey)
{
	const QByteArray base64DecodedKey = QByteArray::fromBase64(keyBase64);
	const QByteArray cryptedValue = base64DecodedKey.mid(16);
	const QByteArray iv = base64DecodedKey.mid(0, 16);

	QAESEncryption encryption(QAESEncryption::AES_192, QAESEncryption::CBC, QAESEncryption::PKCS7);
	const QByteArray padededDecryptedValue = encryption.decode(cryptedValue, helperKey, iv);

	const int lastCharacterValue = static_cast<int>(padededDecryptedValue[padededDecryptedValue.size() - 1]);
	return padededDecryptedValue.left(padededDecryptedValue.size() - lastCharacterValue);
}

bool Helpers::isMyLicenseSerialNumber(const QByteArray& serialNumber)
{
	const ParsedSerialNumberData parsedSerialNumberData = parseMySerialNumberData(serialNumber);
	const QDate& date = std::get<2>(parsedSerialNumberData);
	return !date.isNull();
}

Helpers::ParsedSerialNumberData Helpers::parseMySerialNumber(const QByteArray& serialNumber)
{
	return parseMySerialNumberData(serialNumber);
}

QString Helpers::serialNumberFilePath()
{
	QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	QDir dir(path);

	if (!dir.exists())
	{
		dir.mkpath(path);
	}

	return QDir::cleanPath(path + QString("/serial.txt"));
}

QString Helpers::macAddress()
{
	foreach(QNetworkInterface networkInterface, QNetworkInterface::allInterfaces())
	{
		if (networkInterface.flags() & QNetworkInterface::IsLoopBack)
		{
			continue;
		}

		return networkInterface.hardwareAddress();
	}

	return QString();
}

}