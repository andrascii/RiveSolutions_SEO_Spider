#include "titlebar.h"
#include "helpers.h"

namespace SeoSpider
{

TitleBar::TitleBar(QWidget* parent)
	: QFrame(parent)
	, m_pixmapWidthHeight(Common::Helpers::pointsToPixels(10))
	, m_pixmapLeftMargin(Common::Helpers::pointsToPixels(4))
	, m_minimizePixmap(":/images/minimize-window.png")
	, m_maximizePixmap(":/images/maximize-window.png")
	, m_restorePixmap(":/images/restore-window.png")
	, m_closePixmap(":/images/close-window.png")
{
	setWindowFlags(Qt::CustomizeWindowHint);

	m_maximized = false;

	QLabel* label = new QLabel;
	label->setPixmap(QPixmap(":/images/close-window.png"));

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(0);

	QSpacerItem* spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	QPushButton* minimizeButton = createMinimizeButton();
	QPushButton* maximizeRestoreButton = createMaximizeRestoreButton();
	QPushButton* closeButton = createCloseButton();

	layout->addItem(spacer);
	layout->addWidget(minimizeButton);
	layout->addWidget(maximizeRestoreButton);
	layout->addWidget(closeButton);

	VERIFY(connect(closeButton, &QPushButton::clicked, this, &TitleBar::closeButtonClicked));
	VERIFY(connect(maximizeRestoreButton, &QPushButton::clicked, this, &TitleBar::maximizeRestoreButtonClicked));
	VERIFY(connect(maximizeRestoreButton, &QPushButton::clicked, this, &TitleBar::onMaximizeRestoreButtonClicked));
	VERIFY(connect(minimizeButton, &QPushButton::clicked, this, &TitleBar::minimizeButtonClicked));
}

void TitleBar::setButton(ButtonType buttonType)
{
	switch (buttonType)
	{
		case ButtonTypeMinimize:
		{
			
			break;
		}
		case ButtonTypeMaximizeRestore:
		{
			
			break;
		}
		case ButtonTypeClose:
		{

			break;
		}
	}
}

void TitleBar::onMaximizeRestoreButtonClicked()
{
	if (parentWidget()->isMaximized())
	{
		m_maximized = !m_maximized;
	}
	else
	{

	}
}

QPushButton* TitleBar::createMinimizeButton()
{
	QLabel* minimizeButtonPixmapLabel = new QLabel;
	minimizeButtonPixmapLabel->setPixmap(m_minimizePixmap.scaled(m_pixmapWidthHeight, m_pixmapWidthHeight));

	QPushButton* minimizeButton = new QPushButton(this);
	QHBoxLayout* buttonLayout = new QHBoxLayout(minimizeButton);
	buttonLayout->addWidget(minimizeButtonPixmapLabel);

	buttonLayout->setSpacing(0);
	buttonLayout->setContentsMargins(m_pixmapLeftMargin, 0, 0, 0);

	return minimizeButton;
}

QPushButton* TitleBar::createMaximizeRestoreButton()
{
	QLabel* maximizeRestoreButtonPixmapLabel = new QLabel;
	maximizeRestoreButtonPixmapLabel->setPixmap(m_maximizePixmap.scaled(m_pixmapWidthHeight, m_pixmapWidthHeight));

	QPushButton* maximizeRestoreButton = new QPushButton(this);
	QHBoxLayout* buttonLayout = new QHBoxLayout(maximizeRestoreButton);
	buttonLayout->addWidget(maximizeRestoreButtonPixmapLabel);

	buttonLayout->setSpacing(0);
	buttonLayout->setContentsMargins(m_pixmapLeftMargin, 0, 0, 0);

	return maximizeRestoreButton;
}

QPushButton* TitleBar::createCloseButton()
{
	QLabel* closeButtonPixmapLabel = new QLabel;
	closeButtonPixmapLabel->setPixmap(m_closePixmap.scaled(m_pixmapWidthHeight, m_pixmapWidthHeight));

	QPushButton* closeButton = new QPushButton(this);
	QHBoxLayout* buttonLayout = new QHBoxLayout(closeButton);
	buttonLayout->addWidget(closeButtonPixmapLabel);

	buttonLayout->setSpacing(0);
	buttonLayout->setContentsMargins(m_pixmapLeftMargin, 0, 0, 0);

	return closeButton;
}

void TitleBar::mousePressEvent(QMouseEvent *mouseEvent) 
{
	m_startPosition = mouseEvent->globalPos();
	m_clickPosition = mapToParent(mouseEvent->pos());
}

void TitleBar::mouseMoveEvent(QMouseEvent *mouseEvent)
{
	if (m_maximized)
	{
		return;
	}

	parentWidget()->move(mouseEvent->globalPos() - m_clickPosition);

	if (!mouseEvent->globalPos().y())
	{
		//QtWin::extendFrameIntoClientArea(parentWidget()->windowHandle(), -1, -1, -1, -1);
		return;
	}

}

}