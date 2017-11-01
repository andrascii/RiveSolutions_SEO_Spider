#include "seo_spider_service_app.h"

int main(int argc, char** argv)
{
	SeoSpiderService::SeoSpiderServiceApp app(argc, argv);
	return app.exec();
}