#pragma once

namespace SeoSpider
{

class TitledWindow : public QFrame
{
	Q_OBJECT

public:
	TitledWindow(QWidget* content, QWidget* parent = nullptr);

	QWidget* content() const noexcept;

	Q_SLOT void onCloseButtonClicked();
	Q_SLOT void onMaximizeRestoreButtonClicked();
	Q_SLOT void onMinimizeButtonClicked();

private:
	QWidget* m_content;
};

}