#include "message_box_dialog.h"
#include "helpers.h"
#include "application.h"
#include "main_window.h"
#include "widget_helpers.h"
#include "shadow_decoration_frame.h"
#include "cursor_factory.h"

namespace SeoSpider
{

MessageBoxDialog::MessageBoxDialog()
	: FloatingFrame(nullptr)
	, m_ui(new Ui_MessageBox)
	, m_clickedButtonRole(QDialogButtonBox::InvalidRole)
{
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowModality(Qt::ApplicationModal);
	setAttribute(Qt::WA_DeleteOnClose, true);

	QFrame* internalFrame = new QFrame;
	m_ui->setupUi(internalFrame);

	internalFrame->installEventFilter(this);

	VERIFY(connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject())));
	VERIFY(connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept())));
	VERIFY(connect(m_ui->buttonBox, &QDialogButtonBox::clicked, this, &MessageBoxDialog::onButtonClicked));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(new ShadowDecorationFrame(internalFrame, this));

	m_ui->buttonBox->setCursor(CursorFactory::createCursor(Qt::PointingHandCursor));
	m_ui->messageLabel->setWordWrap(true);
}

void MessageBoxDialog::setMessage(const QString& message)
{
	m_ui->messageLabel->setText(message);

	adjustSize();

	setMinimumSize(size());
	setMaximumSize(size());
}

void MessageBoxDialog::setStandardButtons(QDialogButtonBox::StandardButtons buttons)
{
	m_ui->buttonBox->setStandardButtons(buttons);
}

int MessageBoxDialog::result() const
{
	return m_dialogCode;
}

void MessageBoxDialog::accept()
{
	done(QDialog::Accepted);
}

void MessageBoxDialog::reject()
{
	done(QDialog::Rejected);
}

void MessageBoxDialog::done(int r)
{
	completeLocalEventLoop();

	m_dialogCode = static_cast<QDialog::DialogCode>(r);

	hide();
}

void MessageBoxDialog::exec()
{
	show();

	m_eventLoop.exec();

	completeLocalEventLoop();
}

void MessageBoxDialog::onButtonClicked(QAbstractButton* button)
{
	m_clickedButtonRole = m_ui->buttonBox->buttonRole(button);
}

void MessageBoxDialog::showEvent(QShowEvent* event)
{
	WidgetHelpers::moveWidgetToHostCenter(this);

	theApp->mainWindow()->setDisabled(true);

	QFrame::showEvent(event);
}

void MessageBoxDialog::hideEvent(QHideEvent* event)
{
	theApp->mainWindow()->setDisabled(false);

	QFrame::hideEvent(event);

	emit dialogClosed(m_clickedButtonRole);
}

bool MessageBoxDialog::eventFilter(QObject*, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		FloatingFrame::mousePressEvent(static_cast<QMouseEvent*>(event));
	}

	if (event->type() == QEvent::MouseButtonRelease)
	{
		FloatingFrame::mouseReleaseEvent(static_cast<QMouseEvent*>(event));
	}

	if (event->type() == QEvent::MouseMove)
	{
		FloatingFrame::mouseMoveEvent(static_cast<QMouseEvent*>(event));
	}

	return false;
}

void MessageBoxDialog::completeLocalEventLoop()
{
	if (!m_eventLoop.isRunning())
	{
		return;
	}

	m_eventLoop.processEvents();
	m_eventLoop.exit();
}

}