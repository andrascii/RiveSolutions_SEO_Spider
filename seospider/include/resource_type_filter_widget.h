#pragma once

namespace SeoSpider
{

class ResourceTypeFilterWidget: public QFrame
{
	Q_OBJECT

public:
	ResourceTypeFilterWidget(QWidget* parent = nullptr);
	void setFilter(int filter);

	Q_SIGNAL void filterChanged(int filter);

private:
	void createPushButton(const QString& text, int flag, QHBoxLayout* layout, const char* prop = nullptr);

private:
	int m_filter;
	QMap<int, QPushButton*> m_buttons;
};

}