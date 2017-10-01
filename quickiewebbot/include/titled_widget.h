#pragma once

namespace QuickieWebBot
{
	
class TitledWidget : public QFrame
{
	Q_OBJECT

public:
	TitledWidget(QWidget* parent = nullptr);

	void addTitleWidget(QWidget* widget, Qt::AlignmentFlag align = Qt::AlignLeft, bool last = false);
	void setContentWidget(QWidget* widget);

private:
	QFrame* m_titleFrame;
	QHBoxLayout* m_titleLayout;
	QVBoxLayout* m_layout;
};

}
