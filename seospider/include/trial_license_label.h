#pragma once

namespace SeoSpider
{

class TrialLicenseLabel: public QLabel
{
	Q_OBJECT
public:
	TrialLicenseLabel(QWidget* parent = nullptr);

	virtual void mousePressEvent(QMouseEvent* event) override;

private:
	Q_SLOT void onLicenseChanged();
};

}