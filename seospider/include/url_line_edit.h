#pragma once

namespace SeoSpider
{

class UrlLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	UrlLineEdit(QWidget* parent = nullptr);
		
	bool isUrlCorrect() const noexcept;
	Q_SLOT void checkUrlCorrectness();

private:
	bool m_isUrlCorrect;
};	

}