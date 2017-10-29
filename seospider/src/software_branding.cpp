#include "software_branding.h"

namespace SeoSpider
{

QString SoftwareBranding::organizationName() const noexcept
{
	return "sTechnologies";
}

QString SoftwareBranding::productName() const noexcept
{
	return "SEO Spider";
}

QPixmap SoftwareBranding::brandingLogoImage() const noexcept
{
	return QPixmap(":/images/brand-logo.png");
}

}