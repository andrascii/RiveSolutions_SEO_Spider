#pragma once

namespace SeoSpider
{

class SoftwareBranding
{
public:
	QString organizationName() const noexcept;
	QString productName() const noexcept;
	QPixmap brandingLogoImage() const noexcept;
};

}
