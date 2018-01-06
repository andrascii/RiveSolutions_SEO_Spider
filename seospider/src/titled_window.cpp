#include "titled_window.h"
#include "titlebar.h"

namespace SeoSpider
{

TitledWindow::TitledWindow(QWidget* content, QWidget* parent)
	: QFrame(parent)
{
	m_content = content;

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(0);

	TitleBar* titleBar = new TitleBar(this);

	layout->addWidget(titleBar);
	layout->addWidget(m_content);

	m_content->setParent(this);

	setWindowFlags(Qt::FramelessWindowHint);

	VERIFY(connect(titleBar, &TitleBar::closeButtonClicked, this, &TitledWindow::onCloseButtonClicked));
	VERIFY(connect(titleBar, &TitleBar::maximizeRestoreButtonClicked, this, &TitledWindow::onMaximizeRestoreButtonClicked));
	VERIFY(connect(titleBar, &TitleBar::minimizeButtonClicked, this, &TitledWindow::onMinimizeButtonClicked));
}

QWidget* TitledWindow::content() const noexcept
{
	return m_content;
}

void TitledWindow::onCloseButtonClicked()
{
	close();
}

void TitledWindow::onMaximizeRestoreButtonClicked()
{
	if (isMaximized())
	{
		showNormal();
		return;
	}

	showMaximized();
}

void TitledWindow::onMinimizeButtonClicked()
{
	showMinimized();
}

}