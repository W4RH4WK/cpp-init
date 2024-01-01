#pragma once

#include <example/example_logger.hpp>

namespace Example {

class FileLogger : public ILogger {
  public:
	static std::unique_ptr<FileLogger> create(std::string_view filename)
	{
		std::unique_ptr<FileLogger> logger(new FileLogger(filename));
		if (!logger->isValid()) {
			return nullptr;
		}
		return logger;
	}

	void log(std::string_view message) override { m_file << message << "\n"; }

	const std::string& filename() const { return m_filename; }

  private:
	FileLogger(std::string_view filename) : m_filename(filename), m_file(m_filename) {}

	bool isValid() const { return bool(m_file); }

	std::string m_filename;
	std::ofstream m_file;
};

} // namespace Example
