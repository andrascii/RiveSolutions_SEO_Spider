#pragma once

namespace SeoSpider
{

class TrialLicenseLabel: public QLabel
{
	Q_OBJECT
public:
	TrialLicenseLabel(QWidget* parent = nullptr);

private:
	Q_SLOT void onLicenseChanged();
};

}