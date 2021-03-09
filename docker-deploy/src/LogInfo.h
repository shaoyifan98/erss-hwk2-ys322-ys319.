#ifndef LOGINFO
#define LOGINFO

#include <string>
#include <mutex>
#include <fstream>


class LogInfo {
  private:
    std::string id;
    std::ofstream logfile;
  public:
    LogInfo(std::string id): id(id) {
      logfile.open("/var/log/erss/proxy.log", ios::out | ios::app);
    }

    // write normal log info
    void writeInfo(std::string logInfo) {
      std::mutex writeLock;
      writeLock.lock();
      if (logfile.is_open()){
        logfile << id << ": " << logInfo << std::endl;
      }
      writeLock.unlock();
    }

    // write error message like NOTE, WARNING, ERROR
    void writeError(std::string errMsg) {
      std::mutex writeLock;
      writeLock.lock();
      //std::ofstream logfile("/var/log/erss/proxy.log");
      if (logfile.is_open()){
        logfile << "(no-id): " << errMsg << std::endl;
      }
      writeLock.unlock();
    }

    ~LogInfo() {
      logfile.close();
    }

};

#endif