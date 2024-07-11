#include <iostream>
#include <filesystem>

using std::cout;
using std::string;

string available_commands[] = {"exit", "echo", "pwd", "cd", "chex", "append", "run", "runl", "{file_name}", "dir", "help"}; // Add more commands here

namespace commands
{
    // Phase 1 commands
    void echoCommand(string text)
    {
        if (text == "echo")
        {
            cout << "Echo requires an input\n";
        }
        else
        {
            cout << text << "\n";
        }
    }

    void pwdCommand()
    {
        string current_dir = std::filesystem::current_path();
        for (const auto &entry : std::filesystem::directory_iterator(current_dir))
        {
            cout << entry.path().filename() << "\n";
        }
    }

    void cdCommand(string path)
    {
        try
        {
            std::filesystem::current_path(path);
        }
        catch (const std::exception &e)
        {
            cout << "Cannot set current path: No such file or directory\n";
        }
    }

    // Phase 2 commands
    void checkExecutionCommand(std::filesystem::perms p, string fileName)
    {
        if (fileName == "chex")
        {
            cout << "Plese enter a filename\n";
            return;
        }

        using std::filesystem::perms;
        auto show = [=](char op, perms perm)
        {
            if ((perms::none == (perm & p) ? '-' : op) == '-')
            {
                cout << fileName << ": Not executable\n";
            }
            else if ((perms::none == (perm & p) ? '-' : op) == op)
            {
                cout << fileName << ": Is executable\n";
            }
        };

        show('x', perms::owner_exec);
    }

    void appendCommand(string path)
    {
        if (path == "append")
        {
            cout << "Please enter a path to the file\n";
            return;
        }

        string xdg_open = "xdg-open ";
        xdg_open.append(path);
        system(xdg_open.c_str());
    }

    void runCommand(string file_name)
    {
        if (file_name == "run")
        {
            cout << "Please enter a filename";
            return;
        }

        string runFile = "./";
        runFile.append(file_name);
        system(runFile.c_str());
    }

    void runLocalCommand(string path)
    {
        if (path == "runl")
        {
            cout << "Please enter a correct path\n";
            return;
        }

        system(path.c_str());
    }

    void curlyBracesCommand(string input)
    {
        string current_dir = std::filesystem::current_path();

        string::size_type input_begin = input.find("{") + 1;
        string::size_type input_end = input.find("}", input_begin);
        input = input.substr(input_begin, input_end - input_begin);
        int input_length = input.length();

        // Check if there is input in curly braces
        if (input_length == 0)
        {
            cout << "Please enter a file name\n";
            return;
        }

        // If file is inside of directory
        bool file_found = false;
        for (const auto &entry : std::filesystem::directory_iterator(current_dir))
        {

            string token = entry.path().filename();
            if (token == input)
            {
                cout << token << ": inside current directory\n";
                file_found = true;
                break;
            }
        }

        if (file_found == false)
        {
            cout << input << ": Not inside current directory\n";
            return;
        }

        // If file is executable
        checkExecutionCommand(std::filesystem::status(input).permissions(), input);
    }

    // Custom Commands
    void dirCommand()
    {
        cout << "Current Direcotry: \n";
        cout << std::filesystem::current_path() << "\n";
    }

    void helpCommand()
    {
        cout << "Available Commands: \n";
        int commands_length = std::end(available_commands) - std::begin(available_commands);
        for (int i = 1; i < commands_length; i++)
        {
            cout << i << ". " << available_commands[i] << "\n";
        }
    }
}

void checkCommand(string user_input)
{
    string command = user_input.substr(0, user_input.find(' '));
    bool command_is_available = false;
    int available_commands_len = std::end(available_commands) - std::begin(available_commands);

    for (int i = 0; i < available_commands_len; i++)
    {

        if (command == available_commands[i])
        {
            command_is_available = true;
            break;
        }
    }
    if (command_is_available == false)
    {
        cout << command << ": command not found\n";
    }
}

string commandExecution(string user_input, bool curly_braces = false)
{
    using namespace commands;

    string command = user_input.substr(0, user_input.find(' '));
    string command_input = user_input.substr(user_input.find_first_of(" ") + 1);

    if (command == "exit")
    {
        return "exit";
    }

    // Phase 1 commands
    if (command == "echo")
    {
        echoCommand(command_input);
    }
    else if (command == "pwd")
    {
        pwdCommand();
    }
    else if (command == "cd")
    {
        cdCommand(command_input);
    }

    // Phase 2 commands
    else if (command == "chex")
    {
        const auto file_permission = std::filesystem::status(command_input).permissions();
        string file_name = command_input;
        checkExecutionCommand(file_permission, file_name);
    }
    else if (command == "append")
    {
        appendCommand(command_input);
    }
    else if (command == "run")
    {
        runCommand(command_input);
    }
    else if (command == "runl")
    {
        runLocalCommand(command_input);
    }
    else if (curly_braces == true)
    {
        curlyBracesCommand(command_input);
    }

    // Custom Commands
    else if (command == "dir")
    {
        dirCommand();
    }
    else if (command == "help")
    {
        helpCommand();
    }

    return "successful";
}

int main()
{
    bool active = true;
    string exitCode = "exit";

    // Main Shell
    while (active == true)
    {
        string user_input;
        cout << "$ ";
        getline(std::cin, user_input);

        bool has_curly_braces = false;
        if (user_input.find('{') != string::npos && user_input.find('}') != string::npos)
        { // Checks if it has curlyBraces
            has_curly_braces = true;
            // No check command because this is an exception
            commandExecution(user_input, has_curly_braces);
        }
        else
        {
            // checks the command exists
            checkCommand(user_input);
        }

        // Executes the command
        string commExec_output = commandExecution(user_input);
        if (commExec_output == "exit")
        {
            cout << "\nExiting...";
            break;
        }
    }

    return 0;
}