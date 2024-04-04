#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

// Enum for assignment status
enum class AssignmentStatus {
    PENDING,
    APPROVED,
    NEEDS_ITERATION,
    RECHECK_SUBMITTED
};

// Class for Assignment
class Assignment {
public:
    string name;
    AssignmentStatus status;
    int grade;
    int iterations;

    Assignment(string _name) : name(_name), status(AssignmentStatus::PENDING), grade(0), iterations(0) {}
};

// Base class for both students and reviewers
class IMG_Member {
protected:
    string name;
    int id;
public:
    IMG_Member(string _name, int _id) : name(_name), id(_id) {}

    // Virtual function to get profile information
    virtual void getProfile() const = 0;
};

// Student class inheriting from IMG_Member
class Student : public IMG_Member {
private:
    vector<Assignment> assignments;
public:
    Student(string _name, int _id) : IMG_Member(_name, _id) {}

    // Method to get profile information for students
    void getProfile() const override {
        cout << "Student Name: " << name << endl;
        cout << "Student ID(enrollment number): " << id << endl;
        cout << "Assignments: ";
        for (const auto& assignment : assignments) {
            cout << assignment.name << " (Status: " << static_cast<int>(assignment.status) 
                 << ", Grade: " << assignment.grade 
                 << ", Iterations: " << assignment.iterations << "), ";
        }
        cout << endl;
    }

    // Method to get the ID of the student
    int getId() const {
        return id;
    }


    // Method to add assignment
    void addAssignment(Assignment assignment) {
        assignments.push_back(assignment);
        cout << "Assignment '" << assignment.name << "' added successfully!" << endl;
    }

    // Method to get the status of an assignment
    AssignmentStatus getAssignmentStatus(string assignmentName) const {
        for (const auto& assignment : assignments) {
            if (assignment.name == assignmentName) {
                return assignment.status;
            }
        }
        throw invalid_argument("Assignment not found!");
    }

    // Method to set the grade of an assignment
    void setAssignmentGrade(string assignmentName, int grade) {
        for (auto& assignment : assignments) {
            if (assignment.name == assignmentName) {
                assignment.grade = grade;
                cout << "Grade for assignment '" << assignmentName << "' set to " << grade << "!" << endl;
                return;
            }
        }
        throw invalid_argument("Assignment not found!");
    }

    // Method to submit recheck request
    void submitRecheck(string assignmentName) {
        for (auto& assignment : assignments) {
            if (assignment.name == assignmentName) {
                assignment.status = AssignmentStatus::RECHECK_SUBMITTED;
                cout << "Recheck request for assignment '" << assignmentName << "' submitted successfully!" << endl;
                return;
            }
        }
        throw invalid_argument("Assignment not found!");
    }
};

// Reviewer class inheriting from IMG_Member
class Reviewer : public IMG_Member {
    private:
    vector<Student*> students;
public:
    Reviewer(string _name, int _id) : IMG_Member(_name, _id) {}
  
    // Method to get profile information for reviewers
    void getProfile() const override {
        cout << "Reviewer Name: " << name << endl;
        cout << "Reviewer ID: " << id << endl;
    }
    
     // Method to add a student to the reviewer's list
    void addStudent(Student* student) {
        students.push_back(student);
    }

    // Method to assign assignment to a specific student
    void addAssignmentToStudent(int studentId, string assignmentName) {
        for (auto& student : students) {
            if (student->getId() == studentId) {
                Assignment assignment(assignmentName);
                student->addAssignment(assignment);
                cout << "Assignment '" << assignmentName << "' added to student with ID " << studentId << "!" << endl;
                return;
            }
        }
        cout << "Student with ID " << studentId << " not found!" << endl;
    }

    // Method to assign assignment to a batch of students with IDs in a certain range
    void addAssignmentToBatch(int startId, int endId, string assignmentName) {
        for (auto& student : students) {
            if (student->getId() >= startId && student->getId() <= endId) {
                Assignment assignment(assignmentName);
                student->addAssignment(assignment);
                cout << "Assignment '" << assignmentName << "' added to student with ID " << student->getId() << "!" << endl;
            }
        }
    }

    // Method to assign assignment
    void assignAssignment(Student& student, string assignmentName) {
        Assignment assignment(assignmentName);
        student.addAssignment(assignment);
    }

    // Method to review assignment
    void reviewAssignment(Student& student, string assignmentName, bool approve, int grade) {
        try {
            AssignmentStatus status = student.getAssignmentStatus(assignmentName);
            if (status == AssignmentStatus::PENDING || status == AssignmentStatus::RECHECK_SUBMITTED) {
                if (approve) {
                    cout << "Assignment '" << assignmentName << "' reviewed and approved!" << endl;
                    student.setAssignmentGrade(assignmentName, grade);
                } else {
                    cout << "Assignment '" << assignmentName << "' needs iterations." << endl;
                    suggestIteration(student, assignmentName);
                }
            } else {
                throw logic_error("Assignment is not pending for review!");
            }
        } catch (const invalid_argument& e) {
            cerr << "Error: " << e.what() << endl;
        } catch (const logic_error& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }

    // Method to suggest iteration
    void suggestIteration(Student& student, string assignmentName) {
        cout << "Suggesting iteration for assignment '" << assignmentName << "'." << endl;
        // Additional logic for suggesting iterations
    }
};

int main() {
    // Creating instances of students and reviewer
    Student student1("John Doe", 1);
    Student student2("Jane Doe", 2);
    Reviewer reviewer("Alice Smith", 101);

    // Adding students to the reviewer's list
    reviewer.addStudent(&student1);
    reviewer.addStudent(&student2);

    // Reviewer assigns assignment to a batch of students
    reviewer.addAssignmentToBatch(1, 2, "Assignment 1");

    // Student profiles after assignment is assigned
    cout << "--------------------" << endl;
    student1.getProfile();
    cout << "--------------------" << endl;
    student2.getProfile();
    cout << "--------------------" << endl;
    // Reviewer reviews assignment for student1 and disapproves it, suggesting iteration
    reviewer.reviewAssignment(student1, "Assignment 1", false,3);
    cout << "--------------------" << endl;
    // Student1 submits recheck request
    student1.submitRecheck("Assignment 1");
    cout << "--------------------" << endl;
    // Reviewer reviews assignment for student1 again and approves it
    reviewer.reviewAssignment(student1, "Assignment 1", true,4);
    cout << "--------------------" << endl;
    // Reviewer reviews assignment for student2 and disapproves it, suggesting iteration
    reviewer.reviewAssignment(student2, "Assignment 1", false,2);
    cout << "--------------------" << endl;
    // Student2 submits recheck request
    student2.submitRecheck("Assignment 1");
    cout << "--------------------" << endl;
    // Reviewer reviews assignment for student2 again and disapproves it, suggesting another iteration
    reviewer.reviewAssignment(student2, "Assignment 1", false,3);
    cout << "--------------------" << endl;
    // Student2 submits recheck request again
    student2.submitRecheck("Assignment 1");
    cout << "--------------------" << endl;
    // Reviewer reviews assignment for student2 again and approves it
    reviewer.reviewAssignment(student2, "Assignment 1", true,5);
    cout << "--------------------" << endl;
    // Student profiles after assignment is reviewed and iterations are done
    student1.getProfile();
    cout << "--------------------" << endl;
    student2.getProfile();

    return 0;
}
