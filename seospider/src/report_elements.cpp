#include "stdafx.h"
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
	DEBUG_ASSERT(widget()->parentWidget() != nullptr);

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
	if (m_bottom >= 0)
	{
		return m_bottom;
	}

	if (!m_prevLineWidget)
	{
		m_bottom = height() + m_topMargin;
		return m_bottom;
	}

	m_bottom = m_prevLineWidget->bottom() + height() + m_topMargin;
	return m_bottom;
}

int ReportElementBase::right() const
{
	if (m_right >= 0)
	{
		return m_right;
	}

	if (!m_leftWidget)
	{
		m_right = width() + m_leftMargin;
		return m_right;
	}

	m_right = m_leftWidget->right() + width() + m_leftMargin;
	return m_right;
}

void ReportElementBase::setCalculatedWidth(bool value)
{
	m_calculatedWidth = value;
}

void ReportElementBase::invalidateRightBottom()
{
	m_right = -1;
	m_bottom = -1;
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

ImageReportElement::ImageReportElement(const QByteArray& elementId, QWidget* parentWidget, bool adjustWidth)
	: m_label(new QLabel(parentWidget))
	, m_adjustWidth(adjustWidth)
{
	m_label->setAlignment(Qt::AlignCenter);
	m_label->setObjectName(elementId);
}

void ImageReportElement::setValue(const QVariant& value)
{
	if (value.type() == QVariant::Pixmap)
	{
		QPixmap pixmap = qvariant_cast<QPixmap>(value);
		if (m_adjustWidth)
		{
			pixmap = pixmap.scaledToWidth(width(), Qt::SmoothTransformation);
		}
		m_label->setPixmap(pixmap);
	}
}

QWidget* ImageReportElement::widget() const
{
	return m_label;
}

}
