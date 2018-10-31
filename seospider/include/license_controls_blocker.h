#pragma once

namespace SeoSpider
{

class LicenseControlsBlocker : public QObject
{
	Q_OBJECT

public:
	LicenseControlsBlocker(QObject* parent);

private:
	void addWidget(QWidget* object);

private slots:
	void onLicenseChanged();

private:
	QVector<QWidget*> m_widgets;
};

}