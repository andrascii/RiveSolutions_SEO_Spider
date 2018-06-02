#include "wait_operation_frame.h"
#include "application.h"
#include "widget_helpers.h"
#include "shadow_decoration_frame.h"
#include "main_window.h"
#include "cursor_factory.h"

namespace SeoSpider
{

QPointer<WaitOperationFrame> WaitOperationFrame::s_instance = nullptr;

void WaitOperationFrame::show()
{
	static_cast<QWidget*>(instance())->show();

	theApp->processEvents();
}

void WaitOperationFrame::finish()
{
	instance()->hide();
	instance()->deleteLater();

	theApp->processEvents();
}

void WaitOperationFrame::showMessage(const QString& message)
{
	instance()->setMessage(message);
	show();
}

WaitOperationFrame* WaitOperationFrame::instance()
{
	if (!s_instance)
	{
		s_instance = new WaitOperationFrame(theApp->mainWindow());
	}

	return s_instance;
}

WaitOperationFrame::WaitOperationFrame(QWidget* parent)
	: QFrame(parent)
	, m_ui(new Ui_WaitOperationFrame)
{
	applyExternalVisualSettings();
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowModality(Qt::ApplicationModal);
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

	QFrame* internalFrame = new QFrame;
	m_ui->setupUi(internalFrame);

	QMovie* movie = new QMovie(":/gifs/loader.gif");
	m_ui->movieLabel->setMovie(movie);
	movie->start();

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(new ShadowDecorationFrame(internalFrame, this));
}

WaitOperationFrame::~WaitOperationFrame()
{
	restoreExternalVisualSettings();
	s_instance = nullptr;
}

void WaitOperationFrame::setMessage(const QString& message)
{
	m_ui->messageLabel->setText(message);
}

void WaitOperationFrame::showEvent(QShowEvent* event)
{
	WidgetHelpers::moveWidgetToHostCenter(this);
	QFrame::showEvent(event);
}

void WaitOperationFrame::hideEvent(QHideEvent* event)
{
	restoreExternalVisualSettings();
	QFrame::hideEvent(event);
}

void WaitOperationFrame::applyExternalVisualSettings()
{
	QApplication::setOverrideCursor(CursorFactory::createCursor(Qt::WaitCursor));
	theApp->mainWindow()->showShadedOverlay();
	theApp->mainWindow()->setEnabled(false);
}

void WaitOperationFrame::restoreExternalVisualSettings()
{
	QApplication::restoreOverrideCursor();
	theApp->mainWindow()->hideShadedOverlay();
	theApp->mainWindow()->setEnabled(true);
}

}