#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <windows.h>
#include <shlwapi.h>


void printError(const std::string & function)
{
    DWORD dw = GetLastError();
    char * lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<char *>(&lpMsgBuf), 0, nullptr);
    std::cerr << function << "() failed. " << lpMsgBuf;
    LocalFree(lpMsgBuf);
}

bool doesFileExist(const std::string & filename)
{
    return PathFileExists(filename.data()) == TRUE;
}

std::string getEnvironmentVariable(const std::string & variable)
{
    char buffer[32767] = { 0 };
    DWORD result = GetEnvironmentVariable(variable.data(), buffer, sizeof(buffer));
    if (result == 0)
    {
        printError("");
        return {};
    }
    else if (result > sizeof(buffer))
    {
        return {};
    }
    else
    {
        return buffer;
    }
}

std::vector<std::string> getPaths()
{
    std::vector<std::string> result;
    std::string path_variable = getEnvironmentVariable("PATH");
    std::istringstream path_stream(path_variable);
    std::string single_path;
    while (std::getline(path_stream, single_path, ';'))
    {
        result.emplace_back(std::move(single_path));
    }
    return result;
}

std::string searchExecutable(const std::string & executable)
{
    auto paths = getPaths();
    for (auto & path : paths)
    {
        std::string candidate = path + '\\' + executable;
        if (doesFileExist(candidate))
        {
            return candidate;
        }
    }
    return executable;
}

std::string getArguments(const std::string & command)
{
    bool in_quote = false;
    for (size_t i = 0; i < command.size(); ++i)
    {
        char c = command[i];
        if (c == '"')
        {
            in_quote = !in_quote;
        }
        else if (c == ' ')
        {
            if (!in_quote)
            {
                return command.substr(i + 1);
            }
        }
        else if (c == '\\')
        {
            ++i;
        }
    }
    return {};
}

std::string trimLeft(const std::string & argument)
{
    auto pos = argument.find_first_not_of(' ');
    if (pos == std::string::npos)
    {
        return {};
    }
    else
    {
        return argument.substr(pos);
    }
}

std::string trimRight(const std::string & argument)
{
    auto pos = argument.find_last_not_of(' ');
    if (pos == std::string::npos)
    {
        return {};
    }
    else
    {
        return argument.substr(0, pos + 1);
    }
}

int main()
{
    std::string arguments = trimLeft(getArguments(GetCommandLine())); /// We need trimming because extra space is being added before an argument if running as "cmd /c smth".
    if (arguments.empty())
    {
        std::cerr << "Usage:\n";
        std::cerr << "noextrun program [arguments...]\n";
        return 0;
    }

    std::string sub_arguments = getArguments(arguments);
    std::string executable = trimRight(arguments.substr(0, arguments.size() - sub_arguments.size()));

    if (!doesFileExist(executable))
    {
        arguments = searchExecutable(executable) + ' ' + sub_arguments; /// Manually search in PATH to give first priority to files without extension.
    }

    PROCESS_INFORMATION pi;
    STARTUPINFO si = { 0 };
    si.cb = sizeof(si);
    std::vector<char> arguments_data(arguments.begin(), arguments.end());
    arguments_data.push_back(0);
    bool res = CreateProcess(nullptr, &arguments_data[0], nullptr, nullptr, false, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi);
    if (!res)
    {
        printError("CreateProcess");
        return 1;
    }
    else
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD code;
        res = GetExitCodeProcess(pi.hProcess, &code);
        if (!res)
        {
            printError("GetExitCodeProcess");
            return 2;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return code;
    }
}
