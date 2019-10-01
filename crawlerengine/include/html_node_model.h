#pragma once

#include "ihtml_node.h"

namespace CrawlerEngine
{

class IHtmlParser;
// class IHtmlNodeCountedPtr;

class HtmlNodeModel : public QAbstractXmlNodeModel
{
public:
	HtmlNodeModel(QXmlNamePool pool, const IHtmlParser* parser);
	virtual ~HtmlNodeModel() = default;

	virtual QUrl baseUri(const QXmlNodeModelIndex& index) const override;
	virtual QXmlNodeModelIndex::DocumentOrder compareOrder(const QXmlNodeModelIndex& ni1, const QXmlNodeModelIndex& ni2) const override;
	virtual QUrl documentUri(const QXmlNodeModelIndex& n) const override;
	virtual QXmlNodeModelIndex elementById(const QXmlName& id) const override;
	virtual QXmlNodeModelIndex::NodeKind kind(const QXmlNodeModelIndex& ni) const override;
	virtual QXmlName name(const QXmlNodeModelIndex& ni) const override;
	virtual QVector<QXmlName> namespaceBindings(const QXmlNodeModelIndex& n) const override;
	virtual QVector<QXmlNodeModelIndex> nodesByIdref(const QXmlName& idref) const override;
	virtual QXmlNodeModelIndex root(const QXmlNodeModelIndex& n) const override;
	virtual QString stringValue(const QXmlNodeModelIndex& n) const override;
	virtual QVariant typedValue(const QXmlNodeModelIndex& node) const override;

	QXmlNodeModelIndex rootIndex() const;

protected:
	virtual QVector<QXmlNodeModelIndex> attributes(const QXmlNodeModelIndex& element) const override;
	virtual QXmlNodeModelIndex nextFromSimpleAxis(QAbstractXmlNodeModel::SimpleAxis axis, const QXmlNodeModelIndex &origin) const override;

private:
	void toHtmlNode(const QXmlNodeModelIndex& index, IHtmlNodeCountedPtr& node) const;
	IHtmlAttributeCountedPtr toHtmlAttribute(const QXmlNodeModelIndex& index) const;
	QXmlNodeModelIndex fromHtmlNode(const IHtmlNodeCountedPtr& node) const;
	QXmlNodeModelIndex fromHtmlAttribute(const IHtmlAttributeCountedPtr& attribute) const;

private:
	const IHtmlParser* m_parser;
	mutable QXmlNamePool m_pool;
	mutable std::vector<void*> m_path1;
	mutable std::vector<void*> m_path2;

	mutable IHtmlNodeCountedPtr m_cachedNode1;
	mutable IHtmlNodeCountedPtr m_cachedNode2;
};

}
