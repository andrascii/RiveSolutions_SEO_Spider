#pragma once

namespace QuickieWebBot
{

class UrlLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	UrlLineEdit(QWidget* parent = nullptr);
		
	bool isUrlCorrect() const noexcept;
		
	Q_SLOT void checkUrlCorrectness();

private:
	void redrawBorder();

private:
	bool m_isUrlCorrect;
};	

}