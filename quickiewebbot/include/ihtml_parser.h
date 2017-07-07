#pragma once

namespace QuickieWebBot
{

class IHTMLAttribute;

class IHTMLNode
{
public:
	typedef std::vector<IHTMLNode*> HTMLNodeVector;
	typedef std::vector<IHTMLAttribute*> HTMLAttributeVector;

	enum NodeType
	{
		DocumentNodeType,
		ElementNodeType,
		TextNodeType
	};

	virtual NodeType nodeType() const noexcept = 0;
};

class IHTMLDocument : public IHTMLNode
{
public:
	enum QuirksMode
	{
		DocTypeNoQuirksMode,
		DocTypeQuirksMode,
		DocTypeLimitedQuirksMode
	};

	virtual NodeType nodeType() const noexcept = 0;

	virtual bool hasDoctype() const noexcept = 0;

	virtual QString name() const noexcept = 0;
	virtual QString publicIdentifier() const noexcept = 0;
	virtual QString systemIdentifier() const noexcept = 0;

	virtual QuirksMode docTypeQuirksMode() const noexcept = 0;

	virtual HTMLNodeVector const& children() const noexcept = 0;
};

class IHTMLElement : public IHTMLNode
{
public:
	enum ElementType
	{
		#include "ihtml_parser_element_types_enum.h"
	};

	virtual NodeType type() const noexcept = 0;

	//! Returns element value as string
	virtual QString nativeValue() const noexcept = 0;

	virtual HTMLNodeVector const& children() const noexcept = 0;
	virtual HTMLAttributeVector const& attributes() const noexcept = 0;
};

class IHTMLText : public IHTMLNode
{
public:
	enum TextType
	{
		TextTextType,
		CDataTextType,
		CommentTextType,
		WhitespaceTextType
	};

	virtual NodeType type() const noexcept = 0;
};

class IHTMLAttribute
{
public:
};

class IHTMLParser
{
public:
	virtual void setHtmlPage(const QString& htmlPage) = 0;
	virtual IHTMLNode* node(IHTMLNode* node, IHTMLNode::NodeType nodeType) noexcept = 0;
	virtual unsigned countChildren(IHTMLNode* node, IHTMLNode::NodeType nodeType) noexcept = 0;
};

}