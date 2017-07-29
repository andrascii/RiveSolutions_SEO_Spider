#pragma once

namespace QuickieWebBot
{

class Ui::MainFrameClass;

class NavigationPanelController : public QObject
{

public:
	NavigationPanelController(QObject* parent, Ui::MainFrameClass* ui);

	virtual bool eventFilter(QObject* watched, QEvent* event) override;

private:
	QPushButton* m_prevSelectedButton;
	Ui::MainFrameClass* m_ui;

};

}