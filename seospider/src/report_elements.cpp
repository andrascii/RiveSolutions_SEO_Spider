#include "report_elements.h"

namespace SeoSpider
{
	
void ReportElementBase::setRelativePosition(int leftMargin, int topMargin, IReportElement* prevLineWidget, IReportElement* leftWidget)
{
	m_topMargin = topMargin;
	m_leftMargin = leftMargin;

	m_prevLineWidget = prevLineWidget;
	m_leftWidget = leftWidget;
}

void ReportElementBase::moveToPosition()
{
	QWidget* parentWidget = widget()->parentWidget();
	ASSERT(parentWidget != nullptr);

	const int top = m_prevLineWidget ? m_prevLineWidget->bottom() + m_topMargin : m_topMargin;
	const int left = m_leftWidget ? m_leftWidget->right() + m_leftMargin : m_leftMargin;
	widget()->move(left, top);
}

void ReportElementBase::setSize(int width, int height)
{
	m_width = width;
	m_height = height;

	ASSERT(widget() != nullptr);

	if (width > 0)
	{
		widget()->setFixedWidth(width);
	}
	else
	{
		m_calculatedWidth = true;
	}
	widget()->setFixedHeight(height);
}

int ReportElementBase::width() const
{
	if (!m_calculatedWidth)
	{
		return m_width;
	}

	widget()->adjustSize();
	return widget()->size().width();
}

int ReportElementBase::height() const
{
	return m_height;
}

int ReportElementBase::bottom() const
{
	if (!m_prevLineWidget)
	{
		return height() + m_topMargin;
	}

	return m_prevLineWidget->bottom() + height() + m_topMargin;
}

int ReportElementBase::right() const
{
	if (!m_leftWidget)
	{
		return width() + m_leftMargin;
	}

	return m_leftWidget->right() + width() + m_leftMargin;
}

void ReportElementBase::setCalculatedWidth(bool value)
{
	m_calculatedWidth = value;
}

LabelReportElement::LabelReportElement(const QByteArray& elementId, QWidget* parentWidget, Qt::Alignment alignment)
	: m_label(new QLabel(parentWidget))
{
	m_label->setAlignment(alignment);
	m_label->setObjectName(elementId);
}

void LabelReportElement::setValue(const QVariant& value)
{
	if (value.type() == QVariant::Url)
	{
		m_label->setText(qvariant_cast<QUrl>(value).toDisplayString());
	}
	else
	{
		m_label->setText(value.toString());
	}
}

QWidget* LabelReportElement::widget() const
{
	return m_label;
}

ImageReportElement::ImageReportElement(const QByteArray& elementId, QWidget* parentWidget)
	: m_label(new QLabel(parentWidget))
{
	m_label->setAlignment(Qt::AlignCenter);
	m_label->setObjectName(elementId);
}

void ImageReportElement::setValue(const QVariant& value)
{
	if (value.type() == QVariant::Pixmap)
	{
		m_label->setPixmap(qvariant_cast<QPixmap>(value));
	}
}

QWidget* ImageReportElement::widget() const
{
	return m_label;
}

}