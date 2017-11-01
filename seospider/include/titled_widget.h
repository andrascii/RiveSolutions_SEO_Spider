#pragma once

namespace SeoSpider
{
	
class TitledWidget : public QFrame
{
	Q_OBJECT

public:
	TitledWidget(QWidget* parent = nullptr);

	void addTitleWidget(QWidget* widget, Qt::AlignmentFlag align = Qt::AlignLeft, bool last = false) const;
	void setContentWidget(QWidget* widget) const;

private:
	QFrame* m_titleFrame;
	QHBoxLayout* m_titleLayout;
	QVBoxLayout* m_layout;
};

}
