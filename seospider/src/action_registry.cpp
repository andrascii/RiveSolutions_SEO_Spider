#include "action_registry.h"

namespace SeoSpider
{

ActionRegistry& ActionRegistry::instance()
{
	static ActionRegistry actionRegistry;

	return actionRegistry;
}

QAction* ActionRegistry::globalAction(const QByteArray& actionKey) const
{
	auto globalActionIterator = m_globalActions.find(actionKey);

	DEBUG_ASSERT(globalActionIterator != m_globalActions.end());

	return globalActionIterator->second;
}

QActionGroup* ActionRegistry::actionGroup(const QByteArray& actionGroupKey) const
{
	auto actionGroupIterator = m_actionGroups.find(actionGroupKey);

	DEBUG_ASSERT(actionGroupIterator != m_actionGroups.end());

	return actionGroupIterator->second.get();
}

QAction* ActionRegistry::addGlobalAction(const QByteArray& actionKey, const QIcon& icon, const QString& text)
{
	DEBUG_ASSERT(m_globalActions.find(actionKey) == std::end(m_globalActions));

	QAction* action = new QAction(icon, text);
	
	return addGlobalAction(actionKey, action);
}

QAction* ActionRegistry::addGlobalAction(const QByteArray& actionKey, const QString& text)
{
	return addGlobalAction(actionKey, QIcon(), text);
}

QAction* ActionRegistry::addGlobalAction(const QByteArray& actionKey, QAction* action)
{
	DEBUG_ASSERT(m_globalActions.find(actionKey) == std::end(m_globalActions));

	m_globalActions[actionKey] = action;
	m_actionGroups[s_globalActionGroup]->addAction(action);

	return action;
}

QActionGroup* ActionRegistry::addActionGroup(const QByteArray& actionGroupKey)
{
	DEBUG_ASSERT(m_actionGroups.find(actionGroupKey) == std::end(m_actionGroups));

	m_actionGroups[actionGroupKey] = std::make_shared<QActionGroup>(nullptr);

	return m_actionGroups[actionGroupKey].get();
}

QAction* ActionRegistry::addActionToActionGroup(const QByteArray& actionGroupKey, const QByteArray& actionKey, const QIcon& icon, const QString& text)
{
	DEBUG_ASSERT(m_actionGroups.find(actionGroupKey) != std::end(m_actionGroups));

	QActionGroup* actionGroup = m_actionGroups[actionGroupKey].get();
	QAction* action = addGlobalAction(actionKey, icon, text);

	actionGroup->addAction(action);

	return action;
}

QAction* ActionRegistry::addActionToActionGroup(const QByteArray& actionGroupKey, const QByteArray& actionKey, const QString& text)
{
	return addActionToActionGroup(actionGroupKey, actionKey, QIcon(), text);
}

QAction* ActionRegistry::addActionToActionGroup(const QByteArray& actionGroupKey, const QByteArray& actionKey, QAction* action)
{
	DEBUG_ASSERT(m_actionGroups.find(actionGroupKey) != std::end(m_actionGroups));

	QActionGroup* actionGroup = m_actionGroups[actionGroupKey].get();
	QAction* act = addGlobalAction(actionKey, action);
	
	return actionGroup->addAction(act);
}

ActionRegistry::ActionRegistry()
{
	addActionGroup(s_globalActionGroup);
}

}