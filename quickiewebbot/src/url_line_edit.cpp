#include "url_line_edit.h"
#include "application.h"
#include "application_settings.h"

namespace QuickieWebBot
{

UrlLineEdit::UrlLineEdit(QWidget* parent) 
	: QLineEdit(parent)
	, m_isUrlCorrect(false)
{
	VERIFY(connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(checkUrlCorrectness())));

	QRegularExpression urlRegExp(
		"(https?://|//)?(www\\.)?(?:((?:[\\.\\w-]+?\\.)*))?([\\w-]+)\\."
		"((?:[a-z]{3}\\.[a-z]{2})|(?:[a-z]{2}\\.[a-z]{3})|(?:[a-z]{2}\\.[a-z]{2})|[a-z]{2,6})(:\\d+)?(.*)?");
	
	urlRegExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

	QRegularExpressionValidator* urlValidator = new QRegularExpressionValidator(urlRegExp, this);
	setValidator(urlValidator);
}

void UrlLineEdit::checkUrlCorrectness()
{
	QString line = text();
	int pos = 0;

	const QValidator* urlValidator = validator();
	m_isUrlCorrect = urlValidator->validate(line, pos) == QValidator::Acceptable;

	setProperty("isValidUrl", m_isUrlCorrect);

 	m_isUrlCorrect ? 
		DEBUGLOG << text() << "Correct url" : 
		DEBUGLOG << text() << "Incorrect url";

	style()->unpolish(this);
	style()->polish(this);
}

bool UrlLineEdit::isUrlCorrect() const noexcept
{
	return m_isUrlCorrect;
}

}