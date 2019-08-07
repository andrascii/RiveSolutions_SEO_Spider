#pragma once

namespace SeoSpider
{

class Dialog : public QDialog
{
	Q_OBJECT

public:
	Dialog(QWidget* parent = nullptr);

	static int showMessageBoxDialog(const QString& title, const QString& message,
		QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	static void showDeferredMessageBoxDialog(const QString& title, const QString& message,
		QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	static Dialog* shownDialog();

	virtual void open() override;
	virtual int exec() override;

signals:
	void dialogClosed();

private:
	struct DeferredDialogData
	{
		QString title;
		QString message;
		QDialogButtonBox::StandardButtons buttons;

		bool operator==(const DeferredDialogData& other) const
		{
			return title == other.title && message == other.message && buttons == other.buttons;
		}
	};

private:
	static Dialog* s_dialog;
	static std::deque<DeferredDialogData> s_defferedDialogs;
};

}