#include <windows.h>
#include <fstream>
#include <string>

using namespace std;

// counter for the keystrokes
static unsigned long long counter = 0;

// length of the expression "SESSION="
constexpr short session_word_length = 8;

// length of the expression "TOTAL="
constexpr short total_word_length = 6;

// ___source___: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

// virtual key value of the first "pressable" character
constexpr short BACKSPACE_ASCII = 0x8;

// virtual key value of the last "pressable" character
constexpr short DOT_ASCII = 0xBE;

BOOL write_to_file()
{

    try
    {
        unsigned short session_number = 1;
        unsigned long long total_keys_pressed = 0;

        // try to open the data file
        fstream data_file;
        data_file.open("data.txt");

        // if it does not exist
        if (!data_file)
        {

            //create one
            data_file.open("data.txt", fstream::in | fstream::out | fstream::app);

            //add the starting data to the new file
            data_file << "SESSION=" << session_number << endl;
            data_file << "TOTAL=" << counter << endl;
        }
        else
        {
            // close to open with new parameters
            data_file.close();

            // open the file with flags: input file, output file, append to file
            data_file.open("data.txt", fstream::in | fstream::out | fstream::app);

            // read the session info line into a string
            string session_string;
            getline(data_file, session_string);

            // the session number is positioned right after the "SESSION=" expression
            // convert it from string to int
            session_number = stoi(session_string.substr(session_word_length, session_string.length() - 1));

            // increase it by one to represent the current session
            ++session_number;

            // read the total keys pressed info line into another string
            string total_string;
            getline(data_file, total_string);

            // the total amount of keys pressed is positioned right after the "TOTAL=" expression
            // convert it from string to int
            total_keys_pressed = stoi(total_string.substr(total_word_length, total_string.length() - 1));

            // add the newly pressed amount to it
            total_keys_pressed += counter;

            // close the file after we've gathered our info
            data_file.close();

            // reopen it with the trunc flag to clear any text inside it
            data_file.open("data.txt", fstream::in | fstream::out | fstream::trunc);

            //write the new data to the file
            data_file << "SESSION=" << session_number << endl;
            data_file << "TOTAL=" << total_keys_pressed << endl;

            // close the file
            data_file.close();
        }

        // try to open the log file
        fstream log_file;
        log_file.open("log.txt");

        // if it does not exist
        if (!log_file)
        {
            // create one
            log_file.open("log.txt", fstream::in | fstream::out | fstream::app);

            log_file << "Keys pressed in session #" << session_number << ": " << counter << endl;
        }
        else
        {
            // close in order to open with new parameters
            log_file.close();

            // open the file with flags: input file, output file, append to file
            log_file.open("log.txt", fstream::in | fstream::out | fstream::app);

            // append the new record
            log_file << "Keys pressed in session #" << session_number << ": " << counter << endl;

            // close the file
            log_file.close();
        }

        return TRUE;
    }
    catch (...)
    {
        return FALSE;
    }
}

// if the console window gets closed, the process gets terminated ------------- this doesn't work! or the PC shuts down
// call write_to_file which returns TRUE if it runs successfully
// and returns FALSE if it fails
BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_CLOSE_EVENT ||
        dwCtrlType == CTRL_SHUTDOWN_EVENT ||
        dwCtrlType == CTRL_BREAK_EVENT)
        return write_to_file();
}

int main()
{
    // kasnije dodaj key u registry da se pokrece na startupu, mozda ovo (?)https://stackoverflow.com/questions/15913202/add-application-to-startup-registry, provjeri za svaki slucaj

    //assign the console handler routine to the console window
    BOOL ret = SetConsoleCtrlHandler(HandlerRoutine, TRUE);

    //hide the console window
    ShowWindow(GetConsoleWindow(), SW_MINIMIZE);

    while (1)
    {

        // iterate over the virtual key set
        // starting at the first and finishing at the last pressable character
        for (int i = BACKSPACE_ASCII; i <= DOT_ASCII; i++)
        {
            // check if the i-th key was pressed using the bitwise & 0x01
            // more info: https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getasynckeystate
            if (GetAsyncKeyState(i) & 0x01)
            {

                // if a certain key gets pressed increase the counter and break out of the loop
                counter++;
                break;
            }
        }
    }

    return 0;
}