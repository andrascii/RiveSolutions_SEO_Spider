#pragma once

namespace SeoSpider
{
	
class HeaderDecorationWidget : public QFrame
{
	Q_OBJECT

public:
	HeaderDecorationWidget(QWidget* parent = nullptr);

	void addWidgetToHeader(QWidget* widget, Qt::AlignmentFlag align = Qt::AlignLeft, bool last = false) const;
	void setContentWidget(QWidget* widget);

protected:
	virtual void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
	void onAnimationFinished();

private:
	QFrame* m_titleFrame;
	QHBoxLayout* m_titleLayout;
	QVBoxLayout* m_layout;
	QWidget* m_contentWidget;
	QPropertyAnimation* m_titleFrameAnimation;
	QPropertyAnimation* m_contentFrameAnimation;
	bool m_animationFinished;
	bool m_titleFrameCollapsed;
};

}
