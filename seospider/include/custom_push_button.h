#pragma once

namespace SeoSpider
{

class CustomPushButton : public QPushButton
{
	Q_OBJECT
	
public:
	CustomPushButton(QWidget* parent = nullptr);
	CustomPushButton(const QString& text, QWidget* parent = nullptr);
	CustomPushButton(const QIcon& icon, const QString& text, QWidget* parent = nullptr);

	void setActive(bool active);

protected:
	virtual void leaveEvent(QEvent*) override;
	virtual void enterEvent(QEvent*) override;
	virtual void paintEvent(QPaintEvent*) override;

private:
	bool m_isInFocus;
	bool m_isActive;
};

}