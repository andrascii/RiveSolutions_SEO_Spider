#pragma once

#include "storage_type.h"

namespace CrawlerEngine
{

class ErrorCategory
{
public:
	enum ErrorCategoryLevel
	{
		LevelInfo,
		LevelNotError,
		LevelWarning,
		LevelError
	};

	static ErrorCategoryLevel level(StorageType type) noexcept;
	static int infoCount() noexcept;
	static int warningCount() noexcept;
	static int errorCount() noexcept;
};

}