#pragma once

namespace SeoSpider
{
	
class HeaderDecorationWidget : public QFrame
{
	Q_OBJECT

public:
	HeaderDecorationWidget(QWidget* parent = nullptr);

	void addWidgetToHeader(QWidget* widget, Qt::AlignmentFlag align = Qt::AlignLeft, bool last = false) const;
	void setContentWidget(QWidget* widget) const;

private:
	QFrame* m_titleFrame;
	QHBoxLayout* m_titleLayout;
	QHBoxLayout* m_contentLayout;
	QVBoxLayout* m_layout;
};

}
