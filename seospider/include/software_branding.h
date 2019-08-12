#pragma once
#include "stdafx.h"

namespace SeoSpider
{

class SoftwareBranding
{
public:
	QString organizationName() const;
	QString productName() const;
	QPixmap brandingLogoImage() const;
	QIcon applicationIcon() const;
};

}
