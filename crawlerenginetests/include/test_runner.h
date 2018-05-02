#pragma once

namespace CrawlerEngineTests
{

Q_DECLARE_METATYPE(std::function<void()>)
	
class TestRunner : public QObject
{
	Q_OBJECT

public:
	TestRunner(QObject* parent = nullptr);

public slots:
	void runTest(const std::function<void()>& testFunction);

private slots:
	void onTestFinished();
};

}