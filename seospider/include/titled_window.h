#pragma once

namespace SeoSpider
{

class TitleBar;

class TitledWindow : public QFrame
{
	Q_OBJECT

public:
	TitledWindow(QWidget* content, QWidget* parent = nullptr);

	QWidget* content() const noexcept;

	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);

	Q_SLOT void onCloseButtonClicked();
	Q_SLOT void onMaximizeRestoreButtonClicked();
	Q_SLOT void onMinimizeButtonClicked();

private:
	QWidget* m_content;
	TitleBar* m_titleBar;

	QPoint m_oldPosition;

	bool m_leftButtonDown;
	bool left, right, bottom;
};

}