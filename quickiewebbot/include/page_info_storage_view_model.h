#pragma once

#include "iview_model.h"

namespace QuickieWebBot
{

class DefaultColumnResizePolicy;
class PageInfoStorageModel;
class TextRenderer;
class UrlRenderer;
class SelectionBackgroundRenderer;
class BackgroundRenderer;

class PageInfoStorageViewModel
	: public QObject
	, public IViewModel
{
	Q_OBJECT

public:
	PageInfoStorageViewModel(PageInfoStorageModel* model, QObject* parent = nullptr);

	virtual void resetRenderersCache() const noexcept override;
	virtual QList<IRenderer*> renderers(const QModelIndex& index) const noexcept override;

private slots:
	void onAttachedModelInternalDataChanged();

private:
	PageInfoStorageModel* m_model;

	std::unique_ptr<TextRenderer> m_textRenderer;
	std::unique_ptr<UrlRenderer> m_urlRenderer;
	std::unique_ptr<SelectionBackgroundRenderer> m_selectionBackgroundRenderer;
	std::unique_ptr<BackgroundRenderer> m_backgroundRenderer;
};

}