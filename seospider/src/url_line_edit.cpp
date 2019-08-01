#include "stdafx.h"
#include "url_line_edit.h"
#include "page_parser_helpers.h"

namespace SeoSpider
{

class UrlValidator: public QValidator
{
public:
	explicit UrlValidator(QObject* parent)
		: QValidator(parent)
	{
	}

	virtual State validate(QString& input, int& pos) const override
	{
		Q_UNUSED(pos);

		static QRegularExpression s_urlRegExp(
			"^(?:(?:https?):\\/\\/)?(?:\\S+(?::\\S*)?@|\\d{1,3}(?:\\.\\d{1,3}){3}|(?:(?:[a-z\\d\\x{00a1}-\\x{ffff}]+-?)*"
			"[a-z\\d\\x{00a1}-\\x{ffff}]+)(?:\\.(?:[a-z\\d\\x{00a1}-\\x{ffff}]+-?)*[a-z\\d\\x{00a1}-\\x{ffff}]+)*"
			"(?:\\.[a-z\\x{00a1}-\\x{ffff}]{2,6}))(?::\\d+)?(?:[^\\s]*)?$"
		);

		if (input.isEmpty())
		{
			return QValidator::Intermediate;
		}

		CrawlerEngine::Url url(input);
		
		if (url.isValid() && CrawlerEngine::PageParserHelpers::isHttpOrHttpsScheme(url))
		{
			if (input.contains("."))
			{
				input = url.toDisplayString();
			}

			pos = qMin(pos, input.length());
			if (!s_urlRegExp.match(input).hasMatch())
			{
				return QValidator::Intermediate;
			}

			if (url.scheme().isEmpty() || url.host().isEmpty())
			{
				input = QString("http://") + input;
				pos += 7;
			}

			return QValidator::Acceptable;
		}

		return QValidator::Intermediate;
	}
};


UrlLineEdit::UrlLineEdit(QWidget* parent) 
	: QLineEdit(parent)
	, m_isUrlCorrect(false)
{
	VERIFY(connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(checkUrlCorrectness())));
	setValidator(new UrlValidator(this));
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
