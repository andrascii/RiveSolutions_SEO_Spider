#pragma once

#include "abstract_page.h"

namespace SeoSpider
{

class PageDataWidget;

class AllPagesPage : public QFrame, public AbstractPage
{
	Q_OBJECT

public:
	AllPagesPage(QWidget* parent = nullptr);
	virtual QWidget* widget() const override;
	virtual QString name() const override;
	virtual QIcon icon() const override;
	virtual Type type() const override;

signals:
	virtual void controlsChanged(IPage::Type) override;

private slots:
	void pageViewSelectionChangedSlot(const QItemSelection& selected, const QItemSelection& deselected);

private:
	PageDataWidget* m_pageDataWidget;
	QSplitter* m_splitter;
};

}