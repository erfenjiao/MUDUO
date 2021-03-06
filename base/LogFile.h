#ifndef MUDUO_BASE_LOGFILE_H
#define MUDUO_BASE_LOGFILE_H

#include "Mutex.h"
#include "Types.h"

#include <memory>

namespace muduo
{

    namespace FileUtil
    {
        class AppendFile;
    }

    class LogFile : noncopyable
    {
        public:
            LogFile(const string& basename,
                    off_t rollSize,
                    bool threadSafe = true,
                    int flushInterval = 3,
                    int checkEveryN = 1024);
            ~LogFile();

            void append(const char* logline, int len);
            void flush();
            bool rollFile();

        private:
            void append_unlocked(const char* logline, int len);
            //不加锁的append模式

            static string getLogFileName(const string& basename, time_t* now);
            //获取日志名

            const string basename_;
            const off_t rollSize_;
            const int flushInterval_;  //写入间隔时间
            const int checkEveryN_;

            int count_;

            std::unique_ptr<MutexLock> mutex_;
            time_t startOfPeriod_;
            time_t lastRoll_;
            time_t lastFlush_;
            std::unique_ptr<FileUtil::AppendFile> file_;

            const static int kRollPerSeconds_ = 60*60*24;
    };

}  // namespace muduo



#endif  // MUDUO_BASE_LOGFILE_H