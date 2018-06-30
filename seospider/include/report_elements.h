#pragma once

namespace SeoSpider
{
	
class IReportElement
{
public:
	virtual ~IReportElement() = default;
	virtual void setValue(const QVariant& value) = 0;
	virtual QWidget* widget() const = 0;
	virtual void setRelativePosition(int topMargin, int leftMargin, IReportElement* prevLineWidget, IReportElement* leftWidget) = 0;
	virtual void moveToPosition() = 0;
	virtual void setSize(int width, int height) = 0;
	virtual int width() const = 0;
	virtual int height() const = 0;
	virtual int bottom() const = 0;
	virtual int right() const = 0;
};

class ReportElementBase: public IReportElement
{
public:
	virtual void setRelativePosition(int leftMargin, int topMargin, IReportElement* prevLineWidget, IReportElement* leftWidget) override;
	virtual void moveToPosition() override;
	virtual void setSize(int width, int height) override;
	virtual int width() const override;
	virtual int height() const override;
	virtual int bottom() const override;
	virtual int right() const override;
	virtual void setCalculatedWidth(bool value);

protected:
	QByteArray m_elementId;
	IReportElement* m_prevLineWidget = nullptr;
	IReportElement* m_leftWidget = nullptr;
	double m_topMargin = 0;
	double m_leftMargin = 0;
	int m_width = 0;
	int m_height = 0;
	bool m_calculatedWidth = false;
};

class LabelReportElement: public ReportElementBase
{
public:
	LabelReportElement(const QByteArray& elementId, QWidget* parentWidget, Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter);
	virtual void setValue(const QVariant& value) override;
	virtual QWidget* widget() const override;

private:
	QLabel* m_label;
};

class ImageReportElement : public ReportElementBase
{
public:
	ImageReportElement(const QByteArray& elementId, QWidget* parentWidget);
	virtual void setValue(const QVariant& value) override;
	virtual QWidget* widget() const override;

private:
	QLabel* m_label;
};

}