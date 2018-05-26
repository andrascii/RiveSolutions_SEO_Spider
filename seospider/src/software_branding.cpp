#include "software_branding.h"
#include "svg_renderer.h"

namespace SeoSpider
{

QString SoftwareBranding::organizationName() const
{
	return "Rive Solutions";
}

QString SoftwareBranding::productName() const
{
	return "Seo Spider";
}

QPixmap SoftwareBranding::brandingLogoImage() const
{
	constexpr int c_height = 200;
	constexpr double c_ratio = 1.837318615087294;
	return SvgRenderer::render(":/images/logo-big.svg", c_height * c_ratio, c_height);
}

QIcon SoftwareBranding::applicationIcon() const
{
	return QIcon(QStringLiteral(":/images/favicon.ico"));
}

}