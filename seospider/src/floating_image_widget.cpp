#include "floating_image_widget.h"

#include "download_request.h"
#include "download_response.h"
#include "helpers.h"

namespace
{
double widthPoints = 300;
}

namespace SeoSpider
{

FloatingImageWidget::FloatingImageWidget(QWidget* parent)
	: QLabel(parent, Qt::WindowType::ToolTip)
	, m_loadingDone(false)
{
	setObjectName("floatingimage");
	setFixedWidth(Common::Helpers::pointsToPixels(widthPoints));
	// setStyleSheet("background-color: rgba(0,0,0,0)");
	setStyleSheet("background-color: rgba(255,255,255,0.5)");
}

void FloatingImageWidget::setCurrentUrl(const QUrl& url)
{
	if (m_url.toDisplayString() == url.toDisplayString())
	{
		return;
	}

	m_url = url;

	if (!url.isValid())
	{
		m_requester.reset();
		hide();
		return;
	}

	hide();
	CrawlerEngine::DownloadRequest request({ url, CrawlerEngine::DownloadRequestType::RequestTypeGet }, 0, CrawlerEngine::DownloadRequest::Status::LinkStatusFirstLoading,
		CrawlerEngine::DownloadRequest::BodyProcessingCommand::CommandDownloadBodyAnyway, true);

	m_requester.reset(request, this, &FloatingImageWidget::onLoadingDone);
	m_requester->start();
}

void FloatingImageWidget::invalidateUrl()
{
	if (m_url.isValid())
	{
		m_url = QUrl();
		m_requester.reset();
	}
	
	if (isVisible())
	{
		hide();
	}
}

void FloatingImageWidget::onLoadingDone(CrawlerEngine::Requester* requester, const CrawlerEngine::DownloadResponse& response)
{
	Q_UNUSED(requester);
	QPixmap pixmap;
	pixmap.loadFromData(response.hopsChain.lastHop().body());
	const int actualWidth = pixmap.width();
	const int maxWidth = Common::Helpers::pointsToPixels(widthPoints);

	if (actualWidth == 0)
	{
		// broken image
		return;
	}

	if (actualWidth > maxWidth)
	{
		const int scaledHeight = pixmap.height() * (static_cast<double>(maxWidth) / actualWidth);
		pixmap = pixmap.scaled(maxWidth, scaledHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	
	setFixedWidth(pixmap.width());
	setFixedHeight(pixmap.height());
	setPixmap(pixmap);
	show();

	m_requester.reset();
	m_loadingDone = true;
}

}
