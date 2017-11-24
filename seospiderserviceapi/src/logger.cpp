#include "logger.h"

namespace SeoSpiderServiceApi
{

thread_local boost::log::sources::severity_logger<boost::log::trivial::severity_level> Logger::s_logger;

SeoSpiderServiceApi::ILogger* Logger::instance()
{
    static std::unique_ptr<Logger, DefaultLoggerDeleter<Logger>> s_instance;

    if (!s_instance)
    {
        s_instance.reset(new Logger);
    }

    return s_instance.get();
}

Logger::~Logger()
{
    using namespace std::literals::string_literals;

    if (!m_deleting)
    {
        // this already deleted
        terminate();
    }
}

void Logger::logMessage(const std::string& message, SeverityLevel severityLevel)
{
    boost::log::record record = s_logger.open_record(boost::log::keywords::severity = 
        static_cast<boost::log::trivial::severity_level>(severityLevel));

    if (record)
    {
        boost::log::record_ostream stream(record);
        stream << message;
        stream.flush();

        s_logger.push_record(boost::move(record));
    }
}

void Logger::flush()
{
    m_loggingCore->flush();
}

Logger::Logger()
    : m_loggingCore(boost::log::core::get())
    , m_deleting(false)
{
    boost::shared_ptr<boost::log::sinks::text_file_backend> backend =
        boost::make_shared<boost::log::sinks::text_file_backend>(
            boost::log::keywords::file_name = "logs.log",
            boost::log::keywords::rotation_size = 5 * 1024 * 1024,
            boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(12, 0, 0)
        );

    typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> sink_t;
    boost::shared_ptr<sink_t> sink = boost::make_shared<sink_t>(backend);

    m_loggingCore->add_sink(sink);

    m_loggingCore->set_filter(boost::log::trivial::severity >= ILogger::SeverityLevel::InfoLevel);
    m_loggingCore->add_global_attribute("time_stamp", boost::log::attributes::local_clock());
    m_loggingCore->add_global_attribute("current_thread_id", boost::log::attributes::current_thread_id());
}

}