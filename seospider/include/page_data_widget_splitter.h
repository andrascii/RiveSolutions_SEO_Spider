#pragma once

namespace SeoSpider
{

class PageDataWidget;

class PageDataWidgetSplitter : public QSplitter
{
	Q_OBJECT

	Q_PROPERTY(int secondWidgetHeight READ secondWidgetHeight WRITE setSecondWidgetHeight)

public:
	PageDataWidgetSplitter(QWidget* parent, QWidget* splitterFirstWidget, PageDataWidget* splitterSecondWidget);
	~PageDataWidgetSplitter();
	void addPageDataWidget(PageDataWidget* splitterSecondWidget);

	int secondWidgetHeight() const;
	void setSecondWidgetHeight(int heigth);
	void syncronize(const PageDataWidgetSplitter* other);

	virtual bool eventFilter(QObject* watched, QEvent* event) override;

protected:
	virtual void showEvent(QShowEvent*) override;

private:
	static int collapsedPageDataWidgetHeight();
	void setSecondWidgetHeightImpl(int height, bool setMaximumHeight);

private slots:
	void onPageWidgetTabClicked(int index, int prevIndex);

private:
	QWidget* m_splitterFirstWidget;
	QWidget* m_splitterSecondWidget;
	bool m_wasShown;
	bool m_pageDataWidgetCollapsed;
	double m_prevDatawidgetHeight;
	int m_animationEndHeight;
	int m_defaultTableViewHeight;
};

}