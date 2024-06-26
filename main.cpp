/* This program is a command-line to-do-list application that helps users
 * manage their tasks efficiently. It allows users to add new tasks, view 
 * existing tasks, mark tasks as completed, edit tasks and delete tasks. 
 * It also provides functionality to save tasks to a CSV (Comma Separated
 * Values) file and retrieve them upon restarting the application. This 
 * program features the use of regular expressions for data extraction 
 * and validation, along with utilisation of the CSV file format for 
 * data storage to ensure the reusability of program. Additionally, this 
 * program is designed to be cross-platform, with the ability to run on both 
 * Windows and Unix-like systems (Linux and MacOS).
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>
#include <cstdio>
#include <vector>
#include <regex>

using namespace std;

// Platform specific commands
#ifdef __MINGW32__ // Windows
    #define CLEAR_SCREEN "cls"
#else // Linux or MacOS
    #define CLEAR_SCREEN "clear"
#endif

// Path to save file
#define DATA_PATH "./save.csv"

// Single item unit
struct TodoItem {
    string title;
    string description;
    string due_date;
    bool completed = false;
};

// List containing item units
typedef vector<TodoItem> TodoItems;

// Function declarations
// Main command functions

/**
 *  @brief Adds a new task to the to-do list.
 *  
 *  This function prompts the user to enter details for a new task,
 *  including the title, description, and due date. If the user
 *  provides valid input, the task is added to the `todo_items` vector.
 */
void add();

/**
 *  @brief Displays all tasks in the to-do list.
 * 
 *  This function iterates through the `todo_items` vector and prints
 *  the detail of each task, including the title, description, due date,
 *  status of completion.
 */
void view();

/**
 *  @brief Marks a task as completed.
 *  
 *  This function prompts the user to select a task by its position in
 *  the `todo_items` vector. If the task is not already marked as completed,
 *  it sets the task's `completed` attribute to true.
 */
void mark();

/**
 *  @brief Edits an existing task in the to-do list.
 * 
 *  This function prompts the user to select a task by its position in the
 *  `todo_items` vector and then allows the user to modify the task's title, 
 *  description, and due date. The task is updated with the new details.
 */
void edit();

/**
 *  @brief Removes a task from the to-do list.
 *  
 *  This function prompts the user to select a task by its position in
 *  the `todo_items` vector and confirms the deletion. If confirmed, the task
 *  is removed from the vector.
 */
void remove();

// Tools

/**
 *  @brief Gets the position of a task in the to-do list.
 * 
 *  This function prompts the user to enter the number of task they want to
 *  interact with. It validates the input and returns the zero-based index of
 *  the task in the `todo_items` vector.
 * 
 *  @param action A string indicating the action being performed (e.g., "edit").
 *  @return The zero-based index of the task in the vector, or -1 if the
 *  operation is aborted.
 */
int get_item_position(string action);

/**
 *  @brief Checks if a year is a leap year.
 * 
 *  This function determines if the given year is a leap year based on the
 *  standard leap year rules.
 * 
 *  @param year The year to check.
 *  @return `true` if the year is a leap year, `false` if otherwise.
 */
bool is_leap_year(int year);

/**
 *  @brief Validates a date string.
 * 
 *  This function checks if the given date string is in the format DD/MM/YYYY
 *  and represents a valid date.
 * 
 *  @param date_str The date string to validate.
 *  @return `true` if the date is valid, `false` if otherwise.
 */
bool is_valid_date(string);

/**
 *  @brief Prompts the user for a valid date input.
 * 
 *  This function continuously prompts the user to enter a date until a valid
 *  date in the format DD/MM/YYYY is provided.
 * 
 *  @return A valid date string in the format DD/MM/YYYY.
 */
string get_date_input();

// File IO functions

/**
 *  @brief Saves the current tasks to save file.
 * 
 *  This function writes all tasks in the `todo_items` vector to a CSV file
 *  specified by `DATA_PATH` constant. Each task is saved in a single line
 *  with fields enclosed in quotes.
 */
void save_data();

/**
 *  @brief Retrieves tasks from a file.
 * 
 *  This function reads tasks from the program save file specified by 
 *  `DATA_PATH` constant and loads them into the `todo_items` vector. Each 
 *  line in the file is parsed to extract task details.
 *  
 *  @return A vector of `TodoItem` containing all tasks read from the file.
 */
TodoItems retrieve_data();

// Global storage object
TodoItems todo_items;

// Main program loop
int main() 
{
    char command;
    // Retrieve saved data from previous run, if any
    todo_items = retrieve_data();

    // Clear screen on first run
    system(CLEAR_SCREEN);

    while (true)
    {
        // Show instructions
        cout << "-To Do List-" << endl;
        cout << "1. Add Task" << endl;
        cout << "2. View Tasks" << endl;
        cout << "3. Mark Task as Completed" << endl;
        cout << "4. Edit Task" << endl;
        cout << "5. Delete Task" << endl;
        cout << "6. Exit" << endl << endl;

        cout << "Enter a number 1-6: ";

        while (true)
        {
            cin >> command;
            // Flush stdin stream to remove input "residue".
            // Only first character will be read, the rest 
            // of the line will be discarded.
            cin.ignore(INT_MAX, '\n');
            if (command >= '1' && command <= '6')
                break;
            cout << "Invalid input. Please enter a number within range 1-6: ";
        }

        cout << endl;

        system(CLEAR_SCREEN);

        // Map commands to main function calls
        switch (command)
        {
            case '1':
                add();
                break;
            case '2':
                view();
                break;
            case '3':
                mark();
                break;
            case '4':
                edit();
                break;
            case '5':
                remove();
                break;
            case '6':
                // Save all data before program termination
                save_data();

                // Closure of application
                cout << "Thanks for using the application, have a nice day!" << endl;
                cout << "Press enter to exit ...";
                cin.ignore(INT_MAX, '\n');

                // Cleanup
                system(CLEAR_SCREEN);

                // Terminate program
                return 0;
        }

        // Allow user to read the output before the program continues
        cout << "\nPress enter to continue ...";
        cin.ignore(INT_MAX, '\n');

        // Remove clutter from previous output to improve readability
        // and to focus on the next instructions
        system(CLEAR_SCREEN);
    }
}

void add()
{
    // Create object to store new task details
    TodoItem task;

    cout << "Enter task details (Empty to abort operation): " << endl;

    // Title of new task
    cout << "Title: ";
    getline(cin, task.title);

    // Allow user to back out of operation if they no longer 
    // wish to continue
    if (task.title.empty())
    {
        cout << "Abort task." << endl;
        return;
    }

    // Description of new task
    cout << "Description: ";
    getline(cin, task.description);

    // Due date of new task
    cout << "Due Date (DD/MM/YYYY): ";
    task.due_date = get_date_input();

    // Add new task to todo_items vector
    todo_items.push_back(task);

    cout << "Task added successfully" << endl;
}

void view()
{
    // Set counter
    // Will be printed onto screen beside the task titles
    int count = 1;

    // Print title of output
    cout << "All Tasks" << endl;

    // Iterate over all items in todo_items vector
    // and print their details onto screen
    for (const auto &item : todo_items)
    {
        cout << endl;
        printf("%-3d", count++);
        cout << "Title: " << item.title << endl;
        cout << "   Desc: " << item.description << endl;
        cout << "   Due Date: " << item.due_date << endl;
        cout << "   Completed: " << (item.completed ? "Yes" : "No") << endl;
    }
}

void mark()
{
    // Prompt user for position of task in todo_item
    // vector to be marked
    int item_position = get_item_position("mark");

    // Status code for operation abortion
    if (item_position == -1)
        return;

    // Check if item is already marked before
    if (todo_items[item_position].completed)
        cout << "Task is already marked as completed." << endl;
    else
    {
        // Change the state of .completed attribute
        todo_items[item_position].completed = true;
        cout << "Task marked as completed." << endl;
    }
}

void edit()
{
    // Prompt user for position of task in todo_item
    // vector to be edited
    int item_position = get_item_position("edit");

    // Status code for operation abortion
    if (item_position == -1)
        return;
        
    // Get selected task from todo_items vector
    auto task = todo_items[item_position];
    
    cout << "Enter task details (Empty to abort operation): " << endl;

    // Print initial title of task for user reference
    cout << "Title " << "(was " << task.title <<  "): ";
    getline(cin, task.title);

    // Allow user to back out of operation if they no longer 
    // wish to continue
    if (task.title.empty())
    {
        cout << "Abort task." << endl;
        return;
    }

    // Get updated decsription for selected task
    cout << "Description: " << "(was " << task.description << "): ";
    getline(cin, task.description);

    // Get updated due date for selected task
    cout << "Due Date (DD/MM/YYYY, was " << task.due_date << "): ";

    // Get and validate date input
    task.due_date = get_date_input();

    // Replace original task details with updated task details
    todo_items[item_position] = task;

    cout << "Task edited successfully" << endl;
}

void remove()
{
    // Prompt user for position of task in todo_item
    // vector to be removed
    int item_position = get_item_position("remove");

    // Status code for operation abortion
    if (item_position == -1)
        return;

    // Character to hold user's decision ([y]es or [n]o)
    char choice;

    // Prompts user to enter choice
    cout << "Confirm to delete \"" << todo_items[item_position].title << "\"? [y/n]: ";
    cin >> choice;

    // Consider both lower and upper case inputs
    // Proceed as long as the choice starts with
    // the 'y' character
    if (choice == 'y' || choice == 'Y')
    {
        // Delete the selected task from todo_items vector
        todo_items.erase(todo_items.begin() + item_position);
        cout << "Task deleted successfully." << endl;
    }
    // Confirmation failed
    else
    {
        cout << "Delete operation cancelled." << endl;
    }

    // Clear buffer for next input to prevent input issues
    // (Ignore leftover input from this point)
    cin.ignore(INT_MAX, '\n');
}

int get_item_position(string action)
{
    // Hold user input for task position in todo_items vector
    int input_num;

    while (true)
    {
        // Print customised prompt
        cout << "Enter task number to " << action << " (0 to abort operation): ";

        cin >> input_num;
        // Clear buffer to prevent input issues
        cin.ignore(INT_MAX, '\n');

        // If read successfully
        if (cin)
        {
            // User chose to abort operation
            if (input_num == 0)
            {
                cout << "Abort task." << endl;

                // Return status code for operation abortion
                return -1;
            }

            // Input number is in valid range
            if (input_num >= 1 && input_num <= todo_items.size())
                break; // Move on to next step
    
            cout << "Task number is out of range." << endl;

            // Continue prompting for input
            continue;
        }
        // User entered non-numeric values
        else 
        {
            // Clear error flag of cin
            // to enable next input
            cin.clear();

            // Clear buffer
            cin.ignore(INT_MAX, '\n');

            // Inform user about incorrect input
            cout << "What you've entered is not a number." << endl;

            // Continue prompting for input
            continue;
        }
    }

    // Adjust offset by 1 before returning
    // (User input starts at 1 while
    // position in vectors starts at 0)
    return input_num - 1;
}

bool is_valid_date(string date_str)
{
    int day, month, year;

    // Extract day, month, year from date_str
    int matches = sscanf(date_str.c_str(), "%d /%d /%d", &day, &month, &year);

    // Proceed only if there are exactly three matches
    if (matches != 3)
        return false;

    // Conditions: 
    // 1. Year should not be a negative number
    // 2. Month should be in range [1, 12]
    // 3. Date should be over 0
    // 4. Date should not exceed maximum days in that particular month
    // 5. Rule 3, with an exception for second month if it is a leap year (28 -> 29)

    if (year < 1) {
        return false;
    }

    if (month < 1 || month > 12) {
        return false;
    }

    // Array to match month with respective days
    const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (day < 1)
        return false;

    if (month == 2 && is_leap_year(year))
    {
        if (day > 29)
            return false;
    }
    else
    {
        if (day > days_in_month[month - 1])
            return false;
    }

    // Valid if all conditions are fulfilled
    return true;
}


bool is_leap_year(int year)
{
    // Condition 1:
    // Leap year if perfectly divisible by 400
    // OR
    // Condition 2:
    // Leap year if not divisible by 100, AND
    // Divisible by 4

    if (year % 400 == 0)
        return true;

    else if (year % 100 != 0 && year % 4 == 0)
        return true;

    else
        return false;
}

string get_date_input()
{
    string date_str;
    int day, month, year;
    stringstream result;

    // Prompt until receives valid input
    while (true)
    {
        // Get the whole line to parse
        getline(cin, date_str);

        // Input has passed validation checking
        if (is_valid_date(date_str))
            break; // Continue onto next step

        // Prompt for reenter
        cout << "Please enter a valid date: ";
    }

    // Arguments breakdown for sscanf function call
    // 1. Convert date_str from std::string to char* (C style string).
    // 2. C style conversion specification: %d, stands for digits/numbers.
    //    Second argument stands for "Match a digit, followed by zero or
    //    more spaces, a slash character, space character(s), a single
    //    slash, and a digit". This allows extraction of wanted information
    //    from a string.
    // 3. 3 pointers to 3 variables, holding the extracted day, month, and
    //    year. Extracted digits will be assigned to these variables by
    //    sscanf function.
    //
    sscanf(date_str.c_str(), "%d /%d /%d", &day, &month, &year);

    // Perform string concatenation for final result
    result << day << "/" << month << "/" << year;

    // Convert to C++ style std::string and return
    return result.str();
}

void save_data()
{
    // Create object for file output
    ofstream fout;

    // Open file specified by DATA_PATH constant for output,
    // in overwrite mode
    fout.open(DATA_PATH, ios::out);

    // Loop through each item in todo_items vector
    for (const auto &item : todo_items)
    {
        // Write item details to the file in CSV format
        // with each field enclosed in quotes
        fout << "\"" << item.title << "\","
            << "\"" << item.description << "\","
            << "\"" << item.due_date << "\","
            << "\"" << item.completed << "\""
            << endl;  // Indicates end of line/single entry
    }

    // Close the file to ensure all data is written safely
    // and release the resources
    fout.close();
}

TodoItems retrieve_data()
{
    // Create object to hold the data of each task
    TodoItem item;
    // Create vector object to store all tasks as a list
    TodoItems items;

    // Create an ifstream object for file input
    // and open file specified by DATA_PATH to read
    ifstream file(DATA_PATH);
    // String variable to hold each line read from the file
    string line;

    // Define a regular expression pattern to match the CSV format fields
    // Detailed explanation:
    // - The pattern first matches a quote character (\"), zero or more
    //   characters that are not quotes (([^\"]*)), a quote character (\"),
    //   followed by a single comma character (,)
    // - The pattern is repeated four times to match the four fields:
    //   title, description, due_date, completed
    regex pattern("\"([^\"]*)\",\"([^\"]*)\",\"([^\"]*)\",\"([^\"]*)\"");

    // Create smatch object to hold the matched results
    smatch matches;

    // Loop through each line in the file
    // Until it reaches the bottom (EOF, end of file)
    while (getline(file, line))
    {
        // Use regex search to match the line against the defined pattern
        regex_search(line, matches, pattern);

        // Extract the matched substrings and assign them
        // to the corresponding fields in item struct
        item.title = matches.str(1);
        item.description = matches.str(2);
        item.due_date = matches.str(3);
        
        // Convert the string "1" or "0" to a boolean type in C++
        // and assign to .completed field/attribute
        item.completed = matches.str(4) == "1" ? true : false;

        // Add item to the end of items list
        items.push_back(item);
    }

    // Close the file and release resources
    file.close();

    // Return the list of retrieved items
    return items;
}