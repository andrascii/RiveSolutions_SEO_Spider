#pragma once

namespace QuickieWebBot
{
	
class TitledWidget : public QFrame
{
	Q_OBJECT

public:
	enum Position
	{
		LeftPosition,
		RightPosition,
		CenterPosition
	};

	TitledWidget(QWidget* parent = nullptr);

	void addTitleWidget(QWidget* widget, Qt::AlignmentFlag align);
	void setContentWidget(QWidget* widget);

private:
	void init();

private:
	QFrame* m_titleFrame;
	QHBoxLayout* m_titleLayout;
	QVBoxLayout* m_layout;
};

}
