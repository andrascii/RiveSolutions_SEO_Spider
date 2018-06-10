#pragma once

namespace SeoSpider
{

class Dialog;

class DialogContainer final
{
public:
	DialogContainer(const DialogContainer&) = delete;
	DialogContainer(DialogContainer&&) = delete;

	static DialogContainer& instance();

	int openDialog(Dialog* dialog) const;

private:
	DialogContainer() = default;
};

}