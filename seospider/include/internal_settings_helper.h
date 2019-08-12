#pragma once
#include "stdafx.h"

namespace SeoSpider
{

class InternalSettingsHelper : public QObject
{
	Q_OBJECT

public:
	InternalSettingsHelper(QObject* parent = nullptr);

	const QVariant& value() const;

public slots:
	void setValue(const QVariant& value = QVariant());

signals:
	void valueChanged(const QVariant& value);

private:
	QVariant m_value;
};

}