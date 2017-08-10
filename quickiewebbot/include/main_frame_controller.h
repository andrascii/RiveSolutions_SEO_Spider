#pragma once

namespace QuickieWebBot
{
	
class MainFrame;

class MainFrameController : public QObject
{
	Q_OBJECT

public:
	explicit MainFrameController(MainFrame* mainFrame, QObject* parent);

	virtual bool eventFilter(QObject* object, QEvent* event) override;

private:
	MainFrame* m_mainFrame;
};

}
