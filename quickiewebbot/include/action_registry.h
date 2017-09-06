#pragma once

namespace QuickieWebBot
{

class ActionRegistry
{
public:
	static ActionRegistry& instance();

	QAction* globalAction(const QByteArray& actionKey) const;
	QActionGroup* actionGroup(const QByteArray& actionGroupKey) const;

	QAction* addGlobalAction(const QByteArray& actionKey, const QIcon& icon = QIcon(), const QString& text = QString());
	QAction* addGlobalAction(const QByteArray& actionKey, const QString& text);

	QActionGroup* addActionGroup(const QByteArray& actionGroupKey);
	QAction* addActionToActionGroup(const QByteArray& actionGroupKey, const QByteArray& actionKey, const QIcon& icon = QIcon(), const QString& text = QString());
	QAction* addActionToActionGroup(const QByteArray& actionGroupKey, const QByteArray& actionKey, const QString& text);

private:
	ActionRegistry();
	ActionRegistry(const ActionRegistry&) = delete;
	ActionRegistry(ActionRegistry&&) = delete;
	ActionRegistry& operator=(const ActionRegistry&) = delete;
	ActionRegistry& operator=(ActionRegistry&&) = delete;

private:
	static constexpr const char* s_globalActionGroup = "GlobalActionsGroup";

	std::map<QByteArray, QAction*> m_globalActions;
	std::map<QByteArray, std::shared_ptr<QActionGroup>> m_actionGroups;
};

}