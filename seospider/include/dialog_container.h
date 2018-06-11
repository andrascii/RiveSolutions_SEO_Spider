#pragma once

namespace SeoSpider
{

class Dialog;
class FloatingDialog;

class DialogContainer final : public QObject
{
	Q_OBJECT

public:
	DialogContainer(const DialogContainer&) = delete;
	DialogContainer(DialogContainer&&) = delete;

	static DialogContainer& instance();

	void registerDialog(Dialog* dialog);
	void registerDialog(FloatingDialog* dialog);
	void openDialog(QWidget* widget) const;

private:
	DialogContainer() = default;

private slots:
	void onDialogDestroyed(QObject* object);

private:
	std::map<QWidget*, QDialog*> m_dialogContainers;
};

}