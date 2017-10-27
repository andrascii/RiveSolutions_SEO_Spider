#pragma once

#include "irequest.h"

namespace Common
{

class LoadRequest : public IRequest
{
public:
	LoadRequest(QUrl url);

	const QUrl& url() const noexcept;
	virtual IRequest* clone() const override;
	virtual RequestType requestType() const noexcept override;

private:
	QUrl m_url;
};

}
