#include "url_line_edit.h"

namespace SeoSpider
{

UrlLineEdit::UrlLineEdit(QWidget* parent) 
	: QLineEdit(parent)
	, m_isUrlCorrect(false)
{
	VERIFY(connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(checkUrlCorrectness())));

	QRegularExpression urlRegExp(
			"^(?:(?:https?):\\/\\/)?(?:\\S+(?::\\S*)?@|\\d{1,3}(?:\\.\\d{1,3}){3}|(?:(?:[a-z\\d\\x{00a1}-\\x{ffff}]+-?)*"
			"[a-z\\d\\x{00a1}-\\x{ffff}]+)(?:\\.(?:[a-z\\d\\x{00a1}-\\x{ffff}]+-?)*[a-z\\d\\x{00a1}-\\x{ffff}]+)*"
			"(?:\\.[a-z\\x{00a1}-\\x{ffff}]{2,6}))(?::\\d+)?(?:[^\\s]*)?$"
			);
	
	urlRegExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
	setValidator(new QRegularExpressionValidator(urlRegExp, this));
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