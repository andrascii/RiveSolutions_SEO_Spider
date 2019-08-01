#include "stdafx.h"
#include "zippo.h"

#include "quazip.h"
#include "quazipfile.h"
#include "unzip.h"

Zippo::Zippo()
	: m_isCancelling(false)
{
	qRegisterMetaType<QDir>("QDir");

	connect(this, SIGNAL(startCompression(const QDir&, const QString&, const QStringList&, const QString&)),
		SLOT(onStartCompression(const QDir&, const QString&, const QStringList&, const QString&)),
		Qt::QueuedConnection);

	moveToThread(this);
	start();
}

Zippo::~Zippo()
{
	cancel();
}

void fillIncludedFilesRecursively(QFileInfoList &fillingList, QDir sourceDir)
{
	QString sourceDirPath = sourceDir.path();

	QFileInfoList includedFiles = sourceDir.entryInfoList(QDir::Files);
	QFileInfoList includedDirs = sourceDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

	fillingList.append(includedFiles);

	foreach(QFileInfo subDir, includedDirs)
	{
		QString subDirPath = subDir.absoluteFilePath();
		fillIncludedFilesRecursively(fillingList, QDir(subDir.absoluteFilePath()));
	}
}

void Zippo::zcompress(const QDir& reportDir, const QString& reportfile, const QStringList& userFiles, const QString& description)
{
	m_isCancelling = false;

	emit startCompression(reportDir, reportfile, userFiles, description);
}

namespace
{
	bool contains(const QSet<QString>& addedFiles, const QString& filename)
	{
		foreach (const QString& file, addedFiles)
		{
			if (file.compare(filename, Qt::CaseInsensitive) == 0)
			{
				return true;
			}
		}

		return false;
	}

	QString makeUnique(const QSet<QString>& addedFiles, const QString& filename)
	{
		QString result = filename;
		int counter = 1;
		while (contains(addedFiles, result))
		{
			result = QString::number(counter++) + filename;
		}

		return result;
	}
}

void Zippo::onStartCompression(const QDir& reportDir, const QString& reportfile, const QStringList& userFiles, const QString& description)
{
	QSet<QString> addedFiles;

	if (!description.isEmpty())
	{
		const QString userDescFileName = reportDir.absoluteFilePath("userdesc.log");

		QFile userdesc(userDescFileName);
		if (!userdesc.open(QIODevice::WriteOnly | QIODevice::Truncate))
		{
			m_error = "Can't create file for user description";
			emit error();
			return;
		}

		QTextStream out(&userdesc);
		out.setCodec("UTF-8");
		out << description;
		out.flush();
		userdesc.close();
	}

	QuaZip report(reportfile);
	if (!report.open(QuaZip::mdCreate))
	{
		m_error = ("QauZip::open" + report.getZipError());
		emit error();
		return;
	}

	report.setFileNameCodec("UTF-8");

	// QByteArray packFilesOnlyEnviroment(getenv("PACKFILESONLY"));
	//bool packUserFilesOnly = (packFilesOnlyEnviroment.toLower() == QByteArray("true"));
	const bool packUserFilesOnly = false;

	//const QByteArray packSubDirsEnviroment(getenv("INCLUDESUBDIRS"));
	//bool includeSubDirs = (packSubDirsEnviroment.toLower() == QByteArray("true"));
	const bool includeSubDirs = true;

	QFileInfoList filesToZip;

	if (!packUserFilesOnly)
	{
		if (!includeSubDirs)
		{
			filesToZip = reportDir.entryInfoList(QDir::Files);
		}
		else
		{
			fillIncludedFilesRecursively(filesToZip, reportDir);
		}
	}

	foreach(QString fileName, userFiles)
	{
		if (!fileName.isEmpty())
		{
			QFileInfo fi(fileName);
			if (fi.isRelative())
			{
				fi.setFile(reportDir, fileName);
			}
			filesToZip.append(fi);
		}
	}

	QuaZipFile zipfile(&report);

	foreach(QFileInfo fileInfo, filesToZip)
	{
		const QString absoluteFilePath = fileInfo.absoluteFilePath();

		QFile srcfile(absoluteFilePath);
		if (!srcfile.open(QIODevice::ReadOnly))
		{
			report.close();
			QFile::remove(reportfile);
			
			m_error = ("Can't open file " + absoluteFilePath);
			emit error();
			return;
		}

		const int truncatedLength = reportDir.path().length() - reportDir.dirName().length();

		const QString fullPath = fileInfo.absoluteFilePath();

		const QString internalFileName = 
			packUserFilesOnly || 
				!fullPath.contains(reportDir.path(), Qt::CaseInsensitive)
			? makeUnique(addedFiles, 
						 reportDir.dirName() + "/" + fileInfo.fileName()) 
			: fileInfo.absoluteFilePath().mid(truncatedLength);

		addedFiles << internalFileName;

		if (!zipfile.open(QIODevice::WriteOnly, QuaZipNewInfo(internalFileName, absoluteFilePath)))
		{
			srcfile.close();
			report.close();
			QFile::remove(reportfile);
			
			m_error = ("QuaZipFile::open" + zipfile.getZipError());
			emit error();
			return;
		}

		const int block = 4096;
		while (!srcfile.atEnd())
		{
			if (m_isCancelling)
			{
				srcfile.close();
				report.close();
				QFile::remove(reportfile);
				
				emit finished();
				return;
			}

			QByteArray data = srcfile.read(block);
			if (data.size() != zipfile.write(data))
			{
				srcfile.close();
				report.close();
				QFile::remove(reportfile);

				m_error = ("QuaZipFile::write" +  zipfile.getZipError());
				emit error();
				return;
			}
		}

		srcfile.close();

		if (zipfile.getZipError() != UNZ_OK)
		{
			zipfile.close();
			report.close();
			QFile::remove(reportfile);

			m_error = ("QuaZipFile::write" + zipfile.getZipError());
			emit error();
			return;
		}

		zipfile.close();
		if (zipfile.getZipError())
		{
			QFile::remove(reportfile);
			report.close();

			m_error = ("QauZipFile::close" + zipfile.getZipError());
			emit error();
			return;
		}
	}

	report.close();
	emit finished();
}

void Zippo::cancel()
{
	m_isCancelling = true;

	exit();
	wait();
}
