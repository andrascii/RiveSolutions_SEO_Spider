#pragma once
#include "stdafx.h"

#include "ui_register_product_dialog.h"
#include "requester_wrapper.h"
#include "dialog.h"

namespace CrawlerEngine
{

class Requester;
class SetSerialNumberResponse;

}

namespace SeoSpider
{

class RegisterProductDialog : public Dialog
{
	Q_OBJECT

public:
	RegisterProductDialog(QWidget* parent = nullptr);

private slots:
	void onLicenseChanged();
	void onEnterSerialNumber();

private:
	void onActivationSerialNumberResult(CrawlerEngine::Requester* requester, const CrawlerEngine::SetSerialNumberResponse& response);

private:
	Ui_RegisterProductDialog* m_ui;
	CrawlerEngine::RequesterWrapper m_licenseActivationRequester;
};

}