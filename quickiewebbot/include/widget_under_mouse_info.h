#pragma once

namespace QuickieWebBot
{

class WidgetUnderMouseInfoDialog;

class WidgetUnderMouseInfo : public QObject
{
	Q_OBJECT

public:
	static void attach(QKeySequence const& keySequence);

protected:
	virtual bool eventFilter(QObject* watched, QEvent* event) override;

private:
	WidgetUnderMouseInfo(QObject* parent, QKeySequence const& keySequence);

	Q_SLOT void deleteWidgetOnApplicationQuit();

private:
	QKeySequence m_keySequence;

	std::unique_ptr<WidgetUnderMouseInfoDialog> m_widgetUnderMouseInfoDialog;
};

}