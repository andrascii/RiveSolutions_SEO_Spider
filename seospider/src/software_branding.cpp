#include "software_branding.h"

namespace SeoSpider
{

QString SoftwareBranding::organizationName() const noexcept
{
	return "Rive Solutions";
}

QString SoftwareBranding::productName() const noexcept
{
	return "Seo Spider";
}

QPixmap SoftwareBranding::brandingLogoImage() const noexcept
{
	return QPixmap(":/images/brand-logo.png");
}

}