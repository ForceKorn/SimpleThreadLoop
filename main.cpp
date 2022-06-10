#include <iostream>
#include <string_view>
#include <string>
#include <ctime>

#include <windows.h>


using buffer_t = std::string;
using buffer_view_t = std::string_view;

static constexpr size_t MILLISECONDS_PER_SECOND = 1000;
static constexpr size_t REQUIRED_SECONDS_FOR_SLEEP = 5;
static constexpr size_t MINIMAL_TIME_BUFFER_SIZE = 10;

static constexpr buffer_view_t TIME_FORMAT = "%X";
static constexpr buffer_view_t ENV_VAR_NAME = "ThreadLiveTime";


buffer_t updateFormatedTime(buffer_t& timeBufferToUpdate)
{
    tm          curDate{ 0 };
    std::time_t timeNow{ time(0) };

    localtime_s(&curDate, &timeNow);
    strftime(&timeBufferToUpdate[0], timeBufferToUpdate.size(), TIME_FORMAT.data(), &curDate);

    return timeBufferToUpdate;
}


DWORD getTimeFromEnviromentVar(const char* environmentVariableName)
{
    const DWORD BUFFER_SIZE = GetEnvironmentVariableA(environmentVariableName, nullptr, 0);
    if (!BUFFER_SIZE)
    {
        std::cout << "Error: can`t get size of environment variable " << GetLastError() << '\n';
        return 0;
    }

    buffer_t buffer(BUFFER_SIZE, '\0');
    DWORD result = GetEnvironmentVariableA(environmentVariableName, &buffer[0], BUFFER_SIZE);
    if (!result)
    {
        std::cout << "Error: can`t get value of environment variable " << GetLastError() << '\n';
        return 0;
    }

    return static_cast<DWORD>(std::stoul(buffer));
}

DWORD WINAPI threadCall(LPVOID argument)
{
    std::string formatedTime(MINIMAL_TIME_BUFFER_SIZE, '\0');

    while (true)
    {
        std::cout << "Current system time: " << updateFormatedTime(formatedTime) << '\n';
        Sleep(REQUIRED_SECONDS_FOR_SLEEP * MILLISECONDS_PER_SECOND);
    }

    return 0;
}


int main()
{
    DWORD threadLiveTime = getTimeFromEnviromentVar(ENV_VAR_NAME.data());

    std::cout << "Thread Live Time: " << threadLiveTime << '\n';

    HANDLE h = CreateThread(0, 0, threadCall, 0, 0, 0);
    if (h)
    {
        WaitForSingleObject(h, threadLiveTime * MILLISECONDS_PER_SECOND);
        CloseHandle(h);
    }

    return 0;
}