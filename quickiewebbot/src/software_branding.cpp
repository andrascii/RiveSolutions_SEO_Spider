#include "software_branding.h"

namespace QuickieWebBot
{

QString SoftwareBranding::organizationName() const noexcept
{
	return "Quickie Software";
}

QString SoftwareBranding::productName() const noexcept
{
	return "QuickieWebBot";
}

QPixmap SoftwareBranding::brandingLogoImage() const noexcept
{
	return QPixmap(":/images/brand-logo.jpg");
}

}