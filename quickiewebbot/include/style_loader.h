#pragma once

namespace QuickieWebBot
{

class StyleLoader : public QObject
{
public:
	static void attachStyleSheet(QString const& filename, QKeySequence const& keySequence);

protected:
	virtual bool eventFilter(QObject* obj, QEvent* event) override;

private:
	StyleLoader(QObject* parent, QString const& filename, QKeySequence const& keySequence);
	void updateStyleSheet();

private:
	QString m_filename;
	QKeySequence m_keySequence;
};

}