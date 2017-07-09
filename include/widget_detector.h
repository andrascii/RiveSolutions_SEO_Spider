#pragma once

namespace QuickieWebBot
{

class WidgetDetector : public QObject
{
public:
	static void attachWidgetDetector(QKeySequence const& keySequence);

protected:
	virtual bool eventFilter(QObject* obj, QEvent* event) override;

private:
	WidgetDetector(QObject* parent, QKeySequence const& keySequence);
	void showMessageBoxInfo();

private:
	QString m_filename;
	QKeySequence m_keySequence;
};

}