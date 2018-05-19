#include "wait_operation_frame.h"
#include "application.h"
#include "widget_helpers.h"
#include "shadow_decoration_frame.h"
#include "main_window.h"

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
		s_instance = new WaitOperationFrame(nullptr);
	}

	return s_instance;
}

WaitOperationFrame::WaitOperationFrame(QWidget* parent)
	: QFrame(parent)
	, m_ui(new Ui_WaitOperationFrame)
{
	theApp->mainWindow()->setEnabled(false);

	setAttribute(Qt::WA_TranslucentBackground);
	setWindowModality(Qt::ApplicationModal);
	setWindowFlags(Qt::Dialog | Qt::Tool | Qt::FramelessWindowHint);

	QFrame* internalFrame = new QFrame;
	m_ui->setupUi(internalFrame);

	QMovie* movie = new QMovie(":/gifs/loader.gif");
	m_ui->movieLabel->setMovie(movie);
	movie->start();

	ShadowDecorationFrame* shadowFrame = new ShadowDecorationFrame(internalFrame, this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(shadowFrame);
}

WaitOperationFrame::~WaitOperationFrame()
{
	theApp->mainWindow()->setEnabled(true);
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

}