#pragma once

namespace SeoSpider
{

class CollapseHeaderButton;

class HeaderDecorationWidget : public QFrame
{
	Q_OBJECT

public:
	HeaderDecorationWidget(QWidget* parent = nullptr);

	void addWidgetToHeader(QWidget* widget, Qt::AlignmentFlag align = Qt::AlignLeft, bool last = false) const;
	void addContentWidget(QWidget* widget);

protected:
	virtual void mouseReleaseEvent(QMouseEvent*) override;

private slots:
	void onAnimationFinished();
	void onCollapseButtonClicked();

private:
	QFrame* m_titleFrame;
	QHBoxLayout* m_titleLayout;
	QVBoxLayout* m_layout;
	QWidget* m_contentWidget;
	QHBoxLayout* m_contentLayout;
	CollapseHeaderButton* m_collapseButton;
	QParallelAnimationGroup* m_collapseAnimation;
	bool m_animationFinished;
	bool m_titleFrameCollapsed;
	QVector<QWidget*> m_hiddenWidgets;
};

}
