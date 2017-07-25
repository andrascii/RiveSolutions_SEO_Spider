#include "url_line_edit.h"
#include "application.h"
#include "application_properties.h"

namespace QuickieWebBot
{

UrlLineEdit::UrlLineEdit(QWidget* parent) 
	: QLineEdit(parent)
{
	VERIFY(connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(checkUrlCorrectness())));
}

void UrlLineEdit::checkUrlCorrectness()
{
	static boost::regex s_absoluteLinkRegexPattern(
		"^(https?://|//)?(www\\.)?(?:((?:[\\.\\w-]+?\\.)*))?([\\w-]+)\\."
		"((?:[a-z]{3}\\.[a-z]{2})|(?:[a-z]{2}\\.[a-z]{3})|(?:[a-z]{2}\\.[a-z]{2})|[a-z]{2,6})(:\\d+)?(.*)?$",
		boost::regex_constants::icase
	);

	m_isUrlCorrect = boost::regex_match(text().toStdString(), s_absoluteLinkRegexPattern);

	setProperty("isValidUrl", m_isUrlCorrect);

	m_isUrlCorrect ?
		DEBUGLOG << text() << "Correct url" :
		DEBUGLOG << text() << "Incorrect url";

	redrawBorder();
}

void UrlLineEdit::redrawBorder()
{
	style()->unpolish(this);
	style()->polish(this);
}

bool UrlLineEdit::isUrlCorrect() const noexcept
{
	return m_isUrlCorrect;
}

}