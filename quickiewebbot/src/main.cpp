#include "application.h"
#include "html_page_word_count_parser.h"
#include "gumbo.h"
#include "page_info.h"

int main(int argc, char *argv[])
{
	QuickieWebBot::Application app(argc, argv);
	return app.exec();
}
