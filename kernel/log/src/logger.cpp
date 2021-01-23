#include <logger.hpp>

#include <string.hpp>

#include "buf_builder.hpp"

using namespace lib;



lib::Logger::Logger(const char* logger_name, LogLevel level):
	_log_level{level},
	_streambufs{logger_name, _log_level}
{}

Logger::~Logger() = default;

Logger::SbHolder::SbHolder(const char* p, lib::LogLevel config)
{
	info = log::build_global_buf(LogLevel::INFO, p, config);
	notice = log::build_global_buf(LogLevel::NOTICE, p, config);
	warning = log::build_global_buf(LogLevel::WARNING, p, config);
	error = log::build_global_buf(LogLevel::ERROR, p, config);
	critical = log::build_global_buf(LogLevel::CRITICAL, p, config);
}
