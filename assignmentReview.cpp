#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <map>
#include <sstream>
using namespace std;

// User class
class User
{
protected:
    string username;
    string password;

public:
    User() : username(""), password("") {}
    User(string username, string password) : username(username), password(password) {}
    string getUsername() { return username; }
    bool checkPassword(string inputPassword) { return password == inputPassword; }
    string getPassword() const { return password; }
    virtual string getName() const = 0; 
    virtual int getId() const = 0;      
    virtual bool isReviewer() const = 0;
};

// Enum for assignment status
enum class AssignmentStatus
{
    PENDING,
    APPROVED,
    NEEDS_ITERATION,
    RECHECK_SUBMITTED
};

// Class for Assignment
class Assignment
{
public:
    string name;
    AssignmentStatus status;
    int grade;
    int iterations;

    Assignment(string _name) : name(_name), status(AssignmentStatus::PENDING), grade(0), iterations(0) {}

    void incrementIterations()
    {
        iterations++;
    }
};

// Base class for both students and reviewers
class IMG_Member : virtual public User
{
protected:
    string name;
    int id;

public:
    // Default constructor
    IMG_Member() : User("", "") {}

    IMG_Member(string username, string password, string _name, int _id)
        : User(username, password), name(_name), id(_id) {}

    // Virtual function to get profile information
    virtual void getProfile() const = 0;
};

// Student class inheriting from IMG_Member
class Student : public IMG_Member
{
private:
    vector<Assignment> assignments;

public:
// Default constructor
    Student() : IMG_Member() {}

    // Constructor 
    Student(string username, string password, string name, int id, Assignment assignment)
    : User(username, password)
{
    // Initialize the remaining member variables
    this->name = name;
    this->id = id;
    assignments.push_back(assignment);
}

    vector<Assignment> getIterations() const
    {
        return assignments;
    }

    string getName() const override { return name; }
    bool isReviewer() const override { return false; }
    // Method to get profile information for students
    Student(string username, string password, string _name, int _id)
        : User(username, password)
    {
        this->name = _name;
        this->id = _id;
    }
    void getProfile() const override
    {
        cout << "Student Name: " << name << endl;
        cout << "Student ID(enrollment number): " << id << endl;
        cout << "Assignments: ";
        for (const auto &assignment : assignments)
        {
            cout << assignment.name << " (Status: " << static_cast<int>(assignment.status)
                 << ", Grade: " << assignment.grade
                 << ", Iterations: " << assignment.iterations << "), ";
        }
        cout << endl;
    }

    // Method to get the ID of the student
    int getId() const
    {
        return id;
    }

    // Method to add assignment
    void addAssignment(Assignment assignment)
    {
        assignments.push_back(assignment);
        cout << "Assignment '" << assignment.name << "' added successfully!" << endl;
    }

    void submitAssignment(const std::string &assignmentName, const std::string &link)
    {
        std::fstream file("users.txt", std::ios::in | std::ios::out);
        std::string line;

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> fields;
            std::string field;

            while (std::getline(iss, field, ',')) // Change ',' to the actual delimiter in your file
            {
                fields.push_back(field);
            }

            // Assuming the ID is the fourth field
            if (std::stoi(fields[3]) == this->getId())
            {
                // Assuming assignments start from the sixth field
                for (size_t i = 5; i < fields.size(); i += 2)
                {
                    if (fields[i] == assignmentName)
                    {
                        // Append the link to the line
                        line += "," + link;

                        // Write the updated line back to the file
                        file.seekp(-line.length(), std::ios::cur);
                        file << line;
                        break;
                    }
                }
                break;
            }
        }

        file.close();
    }

    // Method to get the status of an assignment
    AssignmentStatus getAssignmentStatus(string assignmentName) const
    {
        for (const auto &assignment : assignments)
        {
            if (assignment.name == assignmentName)
            {
                return assignment.status;
            }
        }
        throw invalid_argument("Assignment not found!");
    }

    // Method to set the grade of an assignment
    void setAssignmentGrade(string assignmentName, int grade)
    {
        for (auto &assignment : assignments)
        {
            if (assignment.name == assignmentName)
            {
                assignment.grade = grade;
                cout << "Grade for assignment '" << assignmentName << "' set to " << grade << "!" << endl;
                return;
            }
        }
        throw invalid_argument("Assignment not found!");
    }

    // Method to submit recheck request
    void submitRecheck(string assignmentName)
    {
        for (auto &assignment : assignments)
        {
            if (assignment.name == assignmentName)
            {
                assignment.status = AssignmentStatus::RECHECK_SUBMITTED;
                cout << "Recheck request for assignment '" << assignmentName << "' submitted successfully!" << endl;
                return;
            }
        }
        throw invalid_argument("Assignment not found!");
    }

    void incrementIterations(string assignmentName)
    {
        for (auto &assignment : assignments)
        {
            if (assignment.name == assignmentName)
            {
                assignment.incrementIterations();
                return;
            }
        }
        throw invalid_argument("Assignment not found!");
    }

    void viewAssignments()
    {
        std::ifstream file("users.txt");
        std::string line;

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> fields;
            std::string field;

            while (std::getline(iss, field, ' ')) // Change ' ' to the actual delimiter in your file
            {
                fields.push_back(field);
            }

            // Assuming the ID is the fourth field
            if (std::stoi(fields[3]) == this->getId())
            {
                // Assuming assignments start from the sixth field
                for (size_t i = 5; i < fields.size(); i += 2)
                {
                    std::cout << "Assignment Name: " << fields[i]
                              << ", Status: " << (fields[i + 1] == "1" ? "Completed" : "Pending") << std::endl;
                }
                break;
            }
        }

        file.close();
    }
};

// Reviewer class inheriting from IMG_Member
class Reviewer : public IMG_Member
{
private:
    vector<Student *> students;

public:
    string getName() const override { return name; }
    int getId() const override { return id; }
    bool isReviewer() const override { return true; }
    Reviewer(string username, string password, string name, int id) : User(username, password)
    {
        // Initialize the remaining member variables
        this->name = name;
        this->id = id;
    }
    void loadStudents()
    {
        std::ifstream file("users.txt");
        std::string line;

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> fields;
            std::string field;

            while (std::getline(iss, field, ' ')) // Change ' ' to the actual delimiter in your file
            {
                fields.push_back(field);
            }

            if (fields.size() >= 5 && fields[4] == "0")
            {
                // Assuming the fields are: username, password, name, id, isStudent
                students.push_back(new Student(fields[0], fields[1], fields[2], std::stoi(fields[3])));
            }
        }
    }

    void viewAvailableStudents()
    {
        loadStudents();

        for (const auto &student : students)
        {
            cout << "Student ID: " << student->getId() << ", Name: " << student->getName() << endl;
        }
    }
    // Method to get profile information for reviewers
    void getProfile() const override
    {
        cout << "Reviewer Name: " << name << endl;
        cout << "Reviewer ID: " << id << endl;
    }

    // Method to add a student to the reviewer's list
    void addStudent(Student *student)
    {
        students.push_back(student);
    }

    // Method to assign assignment to a specific student
    void addAssignmentToStudent(int studentId, const std::string &assignmentDetails)
{
    std::ifstream fileIn("users.txt");

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(fileIn, line))
    {
        std::istringstream iss(line);
        std::vector<std::string> fields;
        std::string field;

        while (std::getline(iss, field, ' ')) // Change ' ' to the actual delimiter in your file
        {
            fields.push_back(field);
        }

        // Assuming the ID is the fourth field
        if (std::stoi(fields[3]) == studentId)
        {
            // Append the assignment details to the line
            line += " " + assignmentDetails;
        }

        lines.push_back(line);
    }

    fileIn.close();

    // Overwrite the original file with the modified data
    std::ofstream fileOut("users.txt");
    for (const auto& line : lines)
    {
        fileOut << line << "\n";
    }
}

    // Method to assign assignment to a batch of students with IDs in a certain range
    void addAssignmentToBatch(int startId, int endId, string assignmentName)
{
    for (auto &student : students)
    {
        if (student->getId() >= startId && student->getId() <= endId)
        {
            Assignment assignment(assignmentName);
            student->addAssignment(assignment);
            cout << "Assignment '" << assignmentName << "' added to student with ID " << student->getId() << "!" << endl;

            // Write the change to users.txt
            std::ofstream fileOut("users.txt", std::ios::app);
            fileOut << student->getId() << " " << assignmentName << "\n";
        }
    }
}

    // Modify the reviewAssignment method in the Reviewer class
    void reviewAssignment(Student &student, const std::string &assignmentName, bool approve, int grade)
    {
        // Open the file in read-write mode
        std::fstream file("users.txt", std::ios::in | std::ios::out);

        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> fields;
            std::string field;

            while (std::getline(iss, field, ' ')) // Change ' ' to the actual delimiter in your file
            {
                fields.push_back(field);
            }

            // Assuming the ID is the fourth field
            if (std::stoi(fields[3]) == student.getId())
            {
                // Append the assignment review status and grade to the line
                line += " " + assignmentName + " " + (approve ? "1" : "0") + " " + std::to_string(grade);

                // Write the updated line back to the file
                file.seekp(-line.length(), std::ios::cur);
                file << line;
                break;
            }
        }

        file.close();
    }

    // Method to suggest iteration
    void suggestIteration(Student &student, string assignmentName)
    {
        cout << "Suggesting iteration for assignment '" << assignmentName << "'." << endl;
        // Additional logic for suggesting iterations
    }

    std::string getAssignmentLink(int id, const std::string& assignmentName)
{
    std::ifstream file("users.txt");
    std::string line;
    std::string link;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::vector<std::string> fields;
        std::string field;

        while (std::getline(iss, field, ',')) // Change ',' to the actual delimiter in your file
        {
            fields.push_back(field);
        }

        // Assuming the ID is the fourth field
        if (std::stoi(fields[3]) == id)
        {
            // Assuming assignments start from the sixth field
            for (size_t i = 5; i < fields.size(); i += 3)
            {
                if (fields[i] == assignmentName)
                {
                    // Retrieve the link
                    link = fields[i+2];
                    break;
                }
            }
            break;
        }
    }

    file.close();

    return link;
}
};

// UserManager class
class UserManager
{
    map<string, User *> users;
    map<int, Student *> studentsById;

public:
    UserManager()
    {
        loadUsersFromFile("users.txt");
    }

    void loadUsersFromFile(const string &filename)
    {
        ifstream file(filename);
        string line;
        while (getline(file, line))
        {
            istringstream iss(line);
            string username, password, name;
            int id;
            bool isReviewer;
            // Assuming each line in the file is in the format "username,password,name,id,isReviewer"
            if (!(iss >> username >> password >> name >> id >> isReviewer))
            {
                break; // Error
            }
            registerUser(username, password, name, id, isReviewer);
        }
    }
void registerUser(string username, string password, string name, int id, bool isReviewer, string assignment = "")
{
    User* user;
    if (isReviewer)
    {
        user = new Reviewer(username, password, name, id);
    }
    else
    {
        Student *student = new Student(username, password, name, id, assignment);
        user = student;
        studentsById[id] = student;
    
    }
    users[username] = user;
    saveData(user);
}

    Student &getStudentById(int id)
    {
        if (studentsById.count(id) > 0)
        {
            return *studentsById[id];
        }
        throw std::invalid_argument("Student with given ID not found");
    }

    User *loginUser(string username, string password)
    {
        if (users.find(username) != users.end() && users[username]->checkPassword(password))
        {
            return users[username];
        }
        return nullptr;
    }

    void saveData(User* user)
    {
        ofstream file("users.txt", ios::app);
        file << user->getUsername() << " "
             << user->getPassword() << " "
             << user->getName() << " "
             << user->getId() << " "
             << user->isReviewer();
        if (!user->isReviewer())
        {
            Student *student = dynamic_cast<Student *>(user);
            if (student)
            {
                for (auto &assignment : student->getIterations())
                {
                    file << " " << assignment.iterations;
                }
            }
        }
        file << "\n";
        file.close();
    }
void loadData()
{
    ifstream file("users.txt");
    string username, password, name, assignment;
    int id, iteration;
    bool isReviewer;

    while (file >> username >> password >> name >> id >> isReviewer >> assignment)
    {
        vector<int> iterations;
        char nextChar;
        while (file >> nextChar && nextChar != '\n')
        {
            file.putback(nextChar);  // put the character back, because it's not a newline
            if (file >> iteration)
            {
                iterations.push_back(iteration);
            }
        }
        registerUser(username, password, name, id, isReviewer, assignment);
    }
    file.close();
}
//     void loadData()
// {
//     ifstream file("users.txt");
//     string username, password, name, assignment;
//     int id, iteration;
//     bool isReviewer;

//     while (file >> username >> password >> name >> id >> isReviewer >> assignment)
//     {
//         vector<int> iterations;
//         while (file >> iteration)
//         {
//             iterations.push_back(iteration);
//         }
//         registerUser(username, password, name, id, isReviewer, assignment);
//     }
//     file.close();
// }
};

int main()

{
    UserManager userManager;
    userManager.loadData(); // Load existing users from file

    string username, password, name;
    int id, choice;
    User *currentUser = nullptr;

    while (true)
    {
        if (currentUser == nullptr)
        {
            cout << "1. Register as student\n2. Register as reviewer\n3. Login\n4. Exit\n";
            cin >> choice;

            switch (choice)
            {
            case 1: // Register as student
                cout << "Enter username, password, name, id:\n";
                cin >> username >> password >> name >> id;
                userManager.registerUser(username, password, name, id, false);
                break;
            case 2: // Register as reviewer
                cout << "Enter username, password, name, id:\n";
                cin >> username >> password >> name >> id;
                userManager.registerUser(username, password, name, id, true);
                break;
            case 3: // Login
                cout << "Enter username and password:\n";
                cin >> username >> password;
                currentUser = userManager.loginUser(username, password);
                if (!currentUser)
                {
                    cout << "Invalid username or password.\n";
                }
                break;
            case 4:      // Logout
                exit(0); // Exit the program
                break;
            default:
                cout << "Invalid option.\n";
            }
        }
        else
        {
            // User is logged in, you can now perform actions based on the user type
            if (currentUser->isReviewer())
            {
                // Reviewer options
                cout << "1. View available students\n2. Assign assignment\n3. Review assignment\n4. Logout\n";
                cin >> choice;
                string assignmentDetails;
                int assignmentId;
                string studentIdInput;
                string assignmentName;
                switch (choice)
                {
                case 1: // View available students
                    dynamic_cast<Reviewer *>(currentUser)->viewAvailableStudents();
                    break;
                case 2: // Assign assignment
                    cout << "Enter assignment details and ID:\n";
                    cin >> assignmentDetails >> assignmentId;
                    cout << "Enter student ID or range of IDs (startId-endId):\n";
                    cin >> studentIdInput;
                    if (studentIdInput.find('-') != string::npos) // If input contains a dash, it's a range
                    {
                        int startId = stoi(studentIdInput.substr(0, studentIdInput.find('-')));
                        int endId = stoi(studentIdInput.substr(studentIdInput.find('-') + 1));
                        dynamic_cast<Reviewer *>(currentUser)->addAssignmentToBatch(startId, endId, assignmentDetails);
                    }
                    else // If input doesn't contain a dash, it's a single ID
                    {
                        dynamic_cast<Reviewer *>(currentUser)->addAssignmentToStudent(stoi(studentIdInput), assignmentDetails);
                    }
                    break;
                case 3: // Review assignment
                {
                    int id;
                    cout << "Enter student ID:\n";
                    cin >> id;
                    Student &student = userManager.getStudentById(id); // Retrieve the Student object
                    cout << "Enter assignment name:\n";
                    cin >> assignmentName;

                    // Retrieve the link
                    std::string link = dynamic_cast<Reviewer *>(currentUser)->getAssignmentLink(id, assignmentName);

                    // Display the link
                    cout << "Link: " << link << "\n";

                    bool approve;
                    int grade;
                    cout << "Enter approval status (1 for approve, 0 for disapprove) and grade:\n";
                    cin >> approve >> grade;
                    dynamic_cast<Reviewer *>(currentUser)->reviewAssignment(student, assignmentName, approve, grade);
                    break;
                }
                case 4:      // Logout
                    exit(0); // Exit the program
                    break;
                default:
                    cout << "Invalid option.\n";
                }
            }
            else
            {
                // Student options
                cout << "1. View assignments\n2. Submit assignment\n3. Logout\n";
                cin >> choice;
                string assignmentName, link;
                switch (choice)
                {
                case 1: // View assignments
                    dynamic_cast<Student *>(currentUser)->viewAssignments();
                    break;
                case 2: // Submit assignment

                    cout << "Enter assignment name:\n";
                    cin >> assignmentName;
                    cout << "Enter assignment link:\n";
                    cin >> link;
                    dynamic_cast<Student *>(currentUser)->submitAssignment(assignmentName, link);
                    break;
                case 3:      // Logout
                    exit(0); // Exit the program
                    break;

                default:
                    cout << "Invalid option.\n";
                }
            }
        }
    }

    return 0;
}
