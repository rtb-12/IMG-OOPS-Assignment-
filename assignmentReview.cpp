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

    // void incrementIterations()
    // {
    //     iterations++;
    // }
};

// Base class for both students and reviewers
class IMG_Member : virtual public User
{
protected:
    string name;
    int id;

public:
    IMG_Member() : User("", "") {}

    IMG_Member(string username, string password, string _name, int _id)
        : User(username, password), name(_name), id(_id) {}

    virtual void getProfile() const = 0;
};

// Student class inheriting from IMG_Member
class Student : public IMG_Member
{
private:
    vector<Assignment> assignments;

public:
    Student() : IMG_Member() {}

    Student(string username, string password, string name, int id, Assignment assignment)
        : User(username, password)
    {
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

    int getId() const
    {
        return id;
    }

    void addAssignment(Assignment assignment)
    {
        assignments.push_back(assignment);
        cout << "Assignment '" << assignment.name << "' added successfully!" << endl;
    }

    void submitAssignment(const std::string &assignmentName, const std::string &link)
    {
        std::fstream file("users.txt", std::ios::in | std::ios::out);
        std::string line;
        std::streampos prevPos;
        bool assignmentExists = false;

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> fields;
            std::string field;

            while (std::getline(iss, field, ' '))
            {
                fields.push_back(field);
            }

            if (std::stoi(fields[3]) == this->getId())
            {

                for (size_t i = 6; i < fields.size(); i += 2)
                {
                    if (fields[i] == assignmentName)
                    {
                        assignmentExists = true;

                        line += " " + link + " 0 0 ~";

                        file.seekg(prevPos);
                        file << line << std::endl;
                        break;
                    }
                }
                break;
            }
            prevPos = file.tellg();
        }

        if (!assignmentExists)
        {
            cout << "Assignment '" << assignmentName << "' not found!" << endl;
            return;
        }

        file.close();
    }

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

   
    void viewAssignments()
    {
        std::ifstream file("users.txt");
        std::string line;

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> fields;
            std::string field;

            while (std::getline(iss, field, ' '))
            {
                fields.push_back(field);
            }

            if (std::stoi(fields[3]) == this->getId())
            {

                for (size_t i = 6; i < fields.size(); i += 5)
                {
                    std::cout << "Assignment Name: " << fields[i]
                              << ", Status: " << (fields[i + 3] == "1" ? "Completed" : "Pending") << " Number of Iterations : " << fields[i + 3];

                    // Split the comments
                    std::string comments = fields[i + 4];
                    std::istringstream iss(comments);
                    std::string comment;
                    while (std::getline(iss, comment, '~'))
                    {
                        std::cout << "|" << comment;
                    }
                    std::cout << std::endl;
                }
                break;
            }
        }

        file.close();
    }
};

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

            while (std::getline(iss, field, ' '))
            {
                fields.push_back(field);
            }

            if (fields.size() >= 5 && fields[4] == "0")
            {

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

    void getProfile() const override
    {
        cout << "Reviewer Name: " << name << endl;
        cout << "Reviewer ID: " << id << endl;
    }


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

            while (std::getline(iss, field, ' '))
            {
                fields.push_back(field);
            }

            if (std::stoi(fields[3]) == studentId)
            {

                line += " " + assignmentDetails;
            }

            lines.push_back(line);
        }

        fileIn.close();

        std::ofstream fileOut("users.txt");
        for (const auto &line : lines)
        {
            fileOut << line << "\n";
        }
    }

    void addAssignmentToBatch(int startId, int endId, string assignmentName)
    {
        for (auto &student : students)
        {
            if (student->getId() >= startId && student->getId() <= endId)
            {
                Assignment assignment(assignmentName);
                student->addAssignment(assignment);
                cout << "Assignment '" << assignmentName << "' added to student with ID " << student->getId() << "!" << endl;

                std::ofstream fileOut("users.txt", std::ios::app);
                fileOut << student->getId() << " " << assignmentName << "\n";
            }
        }
    }

    void reviewAssignment(Student &student, const std::string &assignmentName, bool approve, string comment)
    {
        std::fstream file("users.txt", std::ios::in | std::ios::out);
        std::string line;
        std::streampos prevPos;
        bool assignmentExists = false;

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> fields;
            std::string field;

            while (std::getline(iss, field, ' '))
            {
                fields.push_back(field);
            }

            if (std::stoi(fields[3]) == student.getId())
            {
                for (size_t i = 6; i < fields.size(); i += 5)
                {
                    if (fields[i] == assignmentName)
                    {
                        assignmentExists = true;

                        fields[i + 2] = approve ? "1" : "0";

                        // If disapproved, increment the i+3 field
                        if (!approve)
                        {
                            fields[i + 3] = std::to_string(std::stoi(fields[i + 3]) + 1);
                        }

                        // Append the new comment to the existing comment at i+4
                        if (!fields[i + 4].empty())
                        {
                            fields[i + 4] += "~";
                        }
                        fields[i + 4] += comment;

                        // Reconstruct the line
                        line = "";
                        for (const auto &field : fields)
                        {
                            line += field + " ";
                        }
                        line = line.substr(0, line.length() - 1);

                        // Add a newline character at the end of the line
                        line += "\n";

                        // Write the updated line back to the file
                        file.clear();
                        file.seekp(prevPos);
                        file << line;
                        break;
                    }
                }
                break;
            }
            
            prevPos = file.tellg();
        }

        if (!assignmentExists)
        {
            cout << "Assignment '" << assignmentName << "' not found!" << endl;
            return;
        }

        file.close();
    }

    std::string getAssignmentLink(int id, const std::string &assignmentName)
    {
        std::ifstream file("users.txt");
        std::string line;
        std::string link;

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> fields;
            std::string field;

            while (std::getline(iss, field, ' '))
            {
                fields.push_back(field);
            }

            if (std::stoi(fields[3]) == id)
            {

                for (size_t i = 6; i < fields.size(); i += 5)
                {
                    if (fields[i] == assignmentName)
                    {

                        link = fields[i + 1];
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

            if (!(iss >> username >> password >> name >> id >> isReviewer))
            {
                break;
            }
            saveUser(username, password, name, id, isReviewer);
        }
    }
    void saveUser(string username, string password, string name, int id, bool isReviewer, string assignment = "")
    {
        User *user;
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
    }
    void registerUser(string username, string password, string name, int id, bool isReviewer, string assignment = "")
    {
        User *user;
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

    void saveData(User *user)
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
    
};

int main()

{
    UserManager userManager;
    // userManager.loadData();
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
            case 1:
                cout << "Enter username, password, name, id:\n";
                cin >> username >> password >> name >> id;
                userManager.registerUser(username, password, name, id, false);
                break;
            case 2:
                cout << "Enter username, password, name, id:\n";
                cin >> username >> password >> name >> id;
                userManager.registerUser(username, password, name, id, true);
                break;
            case 3:
                cout << "Enter username and password:\n";
                cin >> username >> password;
                currentUser = userManager.loginUser(username, password);
                if (!currentUser)
                {
                    cout << "Invalid username or password.\n";
                }
                break;
            case 4:
                exit(0);
                break;
            default:
                cout << "Invalid option.\n";
            }
        }
        else
        {
            // reviewer options
            if (currentUser->isReviewer())
            {

                cout << "1. View available students\n2. Assign assignment\n3. Review assignment\n4. Logout\n";
                cin >> choice;
                string assignmentDetails;
                int assignmentId;
                string studentIdInput;
                string assignmentName;
                string comment;
                switch (choice)
                {
                case 1:
                    dynamic_cast<Reviewer *>(currentUser)->viewAvailableStudents();
                    break;
                case 2:
                    cout << "Enter assignment details and ID:\n";
                    cin >> assignmentDetails >> assignmentId;
                    cout << "Enter student ID or range of IDs (startId-endId):\n";
                    cin >> studentIdInput;
                    if (studentIdInput.find('-') != string::npos)
                    {
                        int startId = stoi(studentIdInput.substr(0, studentIdInput.find('-')));
                        int endId = stoi(studentIdInput.substr(studentIdInput.find('-') + 1));
                        dynamic_cast<Reviewer *>(currentUser)->addAssignmentToBatch(startId, endId, assignmentDetails);
                    }
                    else
                    {
                        dynamic_cast<Reviewer *>(currentUser)->addAssignmentToStudent(stoi(studentIdInput), assignmentDetails);
                    }
                    break;
                case 3:
                {
                    int id;
                    cout << "Enter student ID:\n";
                    cin >> id;
                    Student &student = userManager.getStudentById(id);
                    cout << "Enter assignment name:\n";
                    cin >> assignmentName;

                    std::string link = dynamic_cast<Reviewer *>(currentUser)->getAssignmentLink(id, assignmentName);

                    cout << "Link: " << link << "\n";

                    bool approve;
                    cout << "Enter approval status (1 for approve, 0 for disapprove) \n";
                    cin >> approve;
                    if (approve == 1)
                    {
                        comment = "Approved";
                    }
                    else
                    {
                        std::cout << "Enter comment:\n";
                        std::cin.ignore(); // Ignore the newline character from the previous input
                        std::getline(cin, comment);
                        dynamic_cast<Reviewer *>(currentUser)->reviewAssignment(student, assignmentName, approve, comment);
                        break;
                    }
                    dynamic_cast<Reviewer *>(currentUser)->reviewAssignment(student, assignmentName, approve, comment);
                    break;
                }
                case 4:
                    exit(0);
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
                case 1:
                    dynamic_cast<Student *>(currentUser)->viewAssignments();
                    break;
                case 2:

                    cout << "Enter assignment name:\n";
                    cin >> assignmentName;
                    cout << "Enter assignment link:\n";
                    cin >> link;
                    dynamic_cast<Student *>(currentUser)->submitAssignment(assignmentName, link);
                    break;
                case 3:
                    exit(0);
                    break;

                default:
                    cout << "Invalid option.\n";
                }
            }
        }
    }

    return 0;
}