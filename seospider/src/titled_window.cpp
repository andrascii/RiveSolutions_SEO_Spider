#include "titled_window.h"
#include "titlebar.h"

namespace SeoSpider
{

TitledWindow::TitledWindow(QWidget* content, QWidget* parent)
	: QFrame(parent)
{
	m_leftButtonDown = false;
	m_content = content;

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(0);

	m_titleBar = new TitleBar(this);

	layout->addWidget(m_titleBar);
	layout->addWidget(m_content);

	m_content->setParent(this);

	setWindowFlags(Qt::CustomizeWindowHint);

	VERIFY(connect(m_titleBar, &TitleBar::closeButtonClicked, this, &TitledWindow::onCloseButtonClicked));
	VERIFY(connect(m_titleBar, &TitleBar::maximizeRestoreButtonClicked, this, &TitledWindow::onMaximizeRestoreButtonClicked));
	VERIFY(connect(m_titleBar, &TitleBar::minimizeButtonClicked, this, &TitledWindow::onMinimizeButtonClicked));
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


void TitledWindow::mousePressEvent(QMouseEvent *event)
{
	m_oldPosition = event->pos();
	m_leftButtonDown = event->button() == Qt::LeftButton;
}

void TitledWindow::mouseMoveEvent(QMouseEvent *e)
{
	int x = e->x();
	int y = e->y();

	if (m_leftButtonDown)
	{
		int dx = x - m_oldPosition.x();
		int dy = y - m_oldPosition.y();

		if(!y)
		{
			//QtWin::extendFrameIntoClientArea(windowHandle(), 0, 1, 0, 0);
			return;
		}

		QRect g = geometry();

		if (left)
		{
			g.setLeft(g.left() + dx);
		}
		if (right)
		{
			g.setRight(g.right() + dx);
		}
		if (bottom)
		{
			g.setBottom(g.bottom() + dy);
		}

		setGeometry(g);

		m_oldPosition = QPoint(!left ? e->x() : m_oldPosition.x(), e->y());
	}
	else 
	{
		QRect r = rect();

		left = qAbs(x - r.left()) <= 5;
		right = qAbs(x - r.right()) <= 5;
		bottom = qAbs(y - r.bottom()) <= 5;

		bool hor = left | right;

		if (hor && bottom) 
		{
			if (left)
			{
				setCursor(Qt::SizeBDiagCursor);
			}
			else
			{
				setCursor(Qt::SizeFDiagCursor);
			}
		}
		else if (hor) 
		{
			setCursor(Qt::SizeHorCursor);
		}
		else if (bottom) 
		{
			setCursor(Qt::SizeVerCursor);
		}
		else 
		{
			setCursor(Qt::ArrowCursor);
		}
	}
}

void TitledWindow::mouseReleaseEvent(QMouseEvent*) 
{
	m_leftButtonDown = false;
}

}