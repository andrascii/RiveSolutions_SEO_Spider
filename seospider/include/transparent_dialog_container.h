#pragma once

namespace SeoSpider
{

class Dialog;
class FloatingDialog;

class TransparentDialogContainer : public QDialog
{
	Q_OBJECT

public:
	TransparentDialogContainer(Dialog* dialog);
	TransparentDialogContainer(FloatingDialog* dialog);

protected:
	virtual void showEvent(QShowEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;

private:
	TransparentDialogContainer(QWidget* dialog);

private:
	QWidget* m_widget;
	bool m_isFloating;
	bool m_mousePressed;
	QPoint m_mousePos;
	QPoint m_wndPos;
};

}