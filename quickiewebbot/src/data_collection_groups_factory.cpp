#include "data_collection_groups_factory.h"

namespace QuickieWebBot
{

DCStorageDescriptionPtr DataCollectionGroupsFactory::create(WebCrawler::DataCollection* dataCollection, ProblemAuditGroups problemsGroup)
{
	std::shared_ptr<DCStorageGroupDescription> p =
		std::make_shared<DCStorageGroupDescription>();

	switch (problemsGroup)
	{
		case ProblemAuditGroups::LinkProblems:
		{
			p->name = "Links";
			p->descriptions.push_back({ WebCrawler::DataCollection::UpperCaseUrlStorageType, "Links With Uppercase Characters" });
			p->descriptions.push_back({ WebCrawler::DataCollection::NonAsciiCharacterUrlStorageType, "Links With Non-ASCII Characters" });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryLongUrlStorageType, "Too Long Links" });

			return p;
		}

		case ProblemAuditGroups::TitleProblems:
		{
			p->name = "Page's Titles";
			p->descriptions.push_back({ WebCrawler::DataCollection::EmptyTitleUrlStorageType, "Empty Titles" });
			p->descriptions.push_back({ WebCrawler::DataCollection::DuplicatedTitleUrlStorageType, "Duplicated Titles" });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryLongTitleUrlStorageType, "Too Long Titles" });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryShortTitleUrlStorageType, "Too Short Titles" });
			p->descriptions.push_back({ WebCrawler::DataCollection::DuplicatedH1TitleUrlStorageType, "Titles Duplicates H1" });
			p->descriptions.push_back({ WebCrawler::DataCollection::SeveralTitleUrlStorageType, "Several Title Tags On The Same Page" });

			return p;
		}

		case ProblemAuditGroups::MetaDescriptionProblems:
		{
			p->name = "Page's Meta Descriptions";
			p->descriptions.push_back({ WebCrawler::DataCollection::EmptyMetaDescriptionUrlStorageType, "Empty Meta Descriptions" });
			p->descriptions.push_back({ WebCrawler::DataCollection::DuplicatedMetaDescriptionUrlStorageType, "Duplicated Meta Descriptions" });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryLongMetaDescriptionUrlStorageType, "Too Long Meta Descriptions" });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryShortMetaDescriptionUrlStorageType, "Too Short Meta Descriptions" });
			p->descriptions.push_back({ WebCrawler::DataCollection::SeveralMetaDescriptionUrlStorageType, "Several Meta Descriptions" });

			return p;
		}

		case ProblemAuditGroups::MetaKeywordProblems:
		{
			p->name = "Page's Meta Keywords";
			p->descriptions.push_back({ WebCrawler::DataCollection::EmptyMetaKeywordsUrlStorageType, "Empty  Meta Keywords" });
			p->descriptions.push_back({ WebCrawler::DataCollection::DuplicatedMetaKeywordsUrlStorageType, "Duplicated  Meta Keywords" });
			p->descriptions.push_back({ WebCrawler::DataCollection::SeveralMetaKeywordsUrlStorageType, "Several Meta Keywords" });

			return p;
		}

		case ProblemAuditGroups::H1Problems:
		{
			p->name = "Page's H1";
			p->descriptions.push_back({ WebCrawler::DataCollection::MissingH1UrlStorageType, "Missing H1" });
			p->descriptions.push_back({ WebCrawler::DataCollection::DuplicatedH1UrlStorageType, "Duplicated H1" });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryLongH1UrlStorageType, "Too Long H1" });
			p->descriptions.push_back({ WebCrawler::DataCollection::SeveralH1UrlStorageType, "Several Equal H1" });

			return p;
		}

		case ProblemAuditGroups::H2Problems:
		{
			p->name = "Page's H2";
			p->descriptions.push_back({ WebCrawler::DataCollection::MissingH2UrlStorageType, "Missing H2" });
			p->descriptions.push_back({ WebCrawler::DataCollection::DuplicatedH2UrlStorageType, "Duplicated H2" });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryLongH2UrlStorageType, "Too Long H2" });
			p->descriptions.push_back({ WebCrawler::DataCollection::SeveralH2UrlStorageType, "Several Equal H2" });

			return p;
		}

		case ProblemAuditGroups::ImageProblems:
		{
			p->name = "Page's H2";
			p->descriptions.push_back({ WebCrawler::DataCollection::Over100kbImageStorageType, "Images Over 100 KB" });
			p->descriptions.push_back({ WebCrawler::DataCollection::MissingAltTextImageStorageType, "Images With Missing Alt Description" });
			p->descriptions.push_back({ WebCrawler::DataCollection::VeryLongAltTextImageStorageType, "Too Long Image Alt Description" });

			return p;
		}
	}

	return nullptr;
}

}