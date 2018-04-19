#include "software_branding.h"
#include "svg_renderer.h"

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
	constexpr int c_height = 250;
	constexpr double c_ratio = 1.837318615087294;
	return SvgRenderer::render(":/images/logo_big.svg", c_height * c_ratio, c_height);
}

}