#pragma once

namespace QuickieWebBot
{

class StyleLoader : public QObject
{
public:
	static void attachStyleLoader(QString const& filename, QKeySequence const& keySequenceCustomStyleSheet);

protected:
	virtual bool eventFilter(QObject* obj, QEvent* event) override;

private:
	StyleLoader(QObject* parent, QString const& filename, QKeySequence const& keySequenceCustomStyleSheet);

	void loadCustomStyleSheet();
	void loadStandardStyleSheet();

private:
	QString m_filename;
	QKeySequence m_keySequenceCustomStyleSheet;
	QKeySequence m_keySequenceStandardStyleSheet;
};

}