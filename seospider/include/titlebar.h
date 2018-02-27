#pragma once

namespace SeoSpider
{

class TitleBar : public QFrame
{
	Q_OBJECT

public:
	enum ButtonType
	{
		ButtonTypeMinimize,
		ButtonTypeMaximizeRestore,
		ButtonTypeClose
	};

	TitleBar(QWidget* parent = nullptr);

	void setButton(ButtonType buttonType);

signals:
	void closeButtonClicked();
	void maximizeRestoreButtonClicked();
	void minimizeButtonClicked();

private slots:
	void onMaximizeRestoreButtonClicked();

protected:
	void mousePressEvent(QMouseEvent *mouseEvent);
	void mouseMoveEvent(QMouseEvent *mouseEvent);

private:
	QPushButton* createMinimizeButton();
	QPushButton* createMaximizeRestoreButton();
	QPushButton* createCloseButton();

private:
	struct TitleBarControls
	{
		std::map<QPushButton*, QLabel*> buttons;
	};

	const int m_pixmapWidthHeight;
	const int m_pixmapLeftMargin;

	QPixmap m_minimizePixmap;
	QPixmap m_maximizePixmap;
	QPixmap m_restorePixmap;
	QPixmap m_closePixmap;

	QLabel* m_minimizeButton;
	QLabel* m_maximizeRestoreButton;
	QLabel* m_closeButton;

	QPoint m_startPosition;
	QPoint m_clickPosition;

	bool m_maximized;
};

}
