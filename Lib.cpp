#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <map>
#include<regex>
using namespace std;

//Utility Functions
tm convertToDate(const string& dateStr) {
    tm date = {};
    istringstream ss(dateStr);
    int day, month, year;
    char slash;

    ss >> day >> slash >> month >> slash >> year;
    
    date.tm_mday = day;
    date.tm_mon = month - 1;
    date.tm_year = year - 1900;

    return date;
}

int daysBetweenDates(const string& date1, const string& date2) {
    tm tm1 = convertToDate(date1);
    tm tm2 = convertToDate(date2);

    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);

    return abs((int)difftime(time2, time1) / (60 * 60 * 24));
}
bool isValidDateFormat(const string& date) {
    regex datePattern(R"(^\d{2}/\d{2}/\d{4}$)");

    if (!regex_match(date, datePattern))
        return false;

    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    if (month < 1 || month > 12)
        return false;

    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        daysInMonth[1] = 29;

    if (day < 1 || day > daysInMonth[month - 1])
        return false;

    return true;
}

//Book Class
class Book {
private:
    string title;
    string author;
    string publisher;
    int year;
    string isbn;
    string status;
    string borrowedby;
    string borroweddate;
    string reservedby;
public:
    Book() : title(""), author(""), publisher(""), year(0), isbn(""), status("Available"),borrowedby("None"),borroweddate("None"),reservedby("None"){}
    Book(const string &t, const string &a, const string &p, int y, const string &i, const string &s = "Available",const string &b ="None",const string &d ="None",const string &r="None")
        : title(t), author(a), publisher(p), year(y), isbn(i), status(s),borrowedby(b),borroweddate(d),reservedby(r) {}

    string getTitle() const { return title; }
    void setTitle(const string &t) { title = t; }
    string getAuthor() const { return author; }
    void setAuthor(const string &a) { author = a; }
    string getPublisher() const { return publisher; }
    void setPublisher(const string &p) { publisher = p; }
    int getYear() const { return year; }
    void setYear(int y) { year = y; }
    string getISBN() const { return isbn; }
    void setISBN(const string &i) { isbn = i; }
    string getStatus() const { return status; }
    void setStatus(const string &s) { status = s; }
    void setborrower(const string&s){ borrowedby=s;}
    string getborrower(){ return borrowedby;}
    void setdate(const string&s) { borroweddate=s;}
    string getdate(){ return borroweddate;}
    void setreservedby(const string &s) { reservedby = s;}
    string getreservedby(){return reservedby;}
    string serialize() const {
        ostringstream oss;
        oss << title << "," << author << "," << publisher << "," << year << "," << isbn << "," << status << ","<<borrowedby<<","<<borroweddate<<","<<reservedby;
        return oss.str();
    }

    
    static Book deserialize(const string &data) {
        istringstream iss(data);
        string token;
        vector<string> tokens;
        while(getline(iss, token, ',')) {
            tokens.push_back(token);
        }
        if(tokens.size() < 6)
            return Book();
        //cout<<tokens[3]<<"\n";
        return Book(tokens[0], tokens[1], tokens[2], stoi(tokens[3]), tokens[4], tokens[5],tokens[6],tokens[7],tokens[8]);
    }
};

//Account class unique for each user
class Account {
private:
    vector<string> borrowedBookISBNs;
    double fine;
    vector<tuple<string,string,string>> borrowingHistory;
public:
    Account(){
        fine = 0.0;
    }

    void addBorrowedBook(const string &isbn) {
        borrowedBookISBNs.push_back(isbn);
    }

    void removeBorrowedBook(const string &isbn) {
        auto it = find(borrowedBookISBNs.begin(), borrowedBookISBNs.end(), isbn);
        if(it != borrowedBookISBNs.end())
            borrowedBookISBNs.erase(it);
    }

    void addHistory(const string &isbn,const string&bdate,const string&rdate)
    {
        borrowingHistory.push_back({isbn,bdate,rdate});
    }
    vector<string> getBorrowedBooks() const { return borrowedBookISBNs; }
    void setFine(double f) { fine = f; }
    double getFine() const { return fine; }
    vector<tuple<string,string,string>> getBorrowingHistory() const { return borrowingHistory;}

    string serialize() const {
        ostringstream oss;
        
        oss << fine << "|";
        
        for (size_t i = 0; i < borrowedBookISBNs.size(); ++i) {
            oss << borrowedBookISBNs[i];
            if (i != borrowedBookISBNs.size() - 1)
                oss << ";";
        }
        
        oss << "|";
        
        for (size_t i = 0; i < borrowingHistory.size(); ++i) {
            oss << get<0>(borrowingHistory[i]) << "+" 
                << get<1>(borrowingHistory[i]) << "+" 
                << get<2>(borrowingHistory[i]);
            if (i != borrowingHistory.size() - 1)
                oss << ";";
        }
        
        return oss.str();
    }
    
    
    void deserialize(const string &data) {
        borrowedBookISBNs.clear();
        borrowingHistory.clear();
        
        size_t firstPipe = data.find("|");
        size_t secondPipe = data.find("|", firstPipe + 1);
        
        if (firstPipe != string::npos) {
            // Parse the fine value
            fine = stod(data.substr(0, firstPipe));
        
            if (secondPipe != string::npos) {
                string borrowedPart = data.substr(firstPipe + 1, secondPipe - firstPipe - 1);
                istringstream borrowedStream(borrowedPart);
                string token;
                while (getline(borrowedStream, token, ';')) {
                    if (!token.empty())
                        borrowedBookISBNs.push_back(token);
                }
   
                string historyPart = data.substr(secondPipe + 1);
                istringstream historyStream(historyPart);
                string record;
                while (getline(historyStream, record, ';')) {
                    if (!record.empty()) {
                        istringstream recordStream(record);
                        string isbn, borrowDate, returnDate;
                        if (getline(recordStream, isbn, '+') &&
                            getline(recordStream, borrowDate, '+') &&
                            getline(recordStream, returnDate, '+')) {
                            borrowingHistory.push_back(make_tuple(isbn, borrowDate, returnDate));
                        }
                    }
                }
            }
        }
    }
    
    
};

//User base class
class User {
protected:
    string uniqueID;
    string username;
    string password;  
    string role;     
    Account account;
public:
    User(){
        uniqueID="";
        username ="";
        password ="";
        role = "";
    }
    User(const string &uid,const string &uname, const string &pwd, const string &r){
        uniqueID=uid;
        username = uname;
        password = pwd;
        role = r;
    }

    string getUniqueID() const{return uniqueID;}
    void setUniqueId(const string& uid){uniqueID=uid;}
    string getUsername() const { return username; }
    void setUsername(const string &uname) { username = uname; }
    string getPassword() const { return password; }
    void setPassword(const string &pwd) { password = pwd; }
    string getRole() const { return role; }
    void setRole(const string &r) { role = r; }
    Account &getAccount() { return account; }
    void setfine(double f)
    {
        account.setFine(f);
    }
    double getfine()
    {
        return account.getFine();
    }

    virtual bool borrowBook(Book *book,const string& date) {
        if(book->getborrower() != "None") {
            cout << "Book is currently borrowed by someone else." << endl;
            return false;
        }
        book->setborrower(uniqueID);
        book->setdate(date);
        book->setStatus("Borrowed");
        book->setreservedby("None");
        account.addBorrowedBook(book->getISBN());
        cout <<"\n"<< username << " borrowed \"" << book->getTitle() <<"\" on "<<date<< endl;
        return true;
    }
    virtual bool returnBook(Book *book,string date) {
        // if(book->getStatus() != "Borrowed") {
        //     cout << "Book not borrowed." << endl;
        //     return false;
        // }
        account.addHistory(book->getISBN(),book->getdate(),date);
        book->setdate("None");
        book->setborrower("None");
        if(book->getStatus()!="Reserved")
        book->setStatus("Available");
        account.removeBorrowedBook(book->getISBN());
        cout << username << " returned \"" << book->getTitle() << "\" on "<<date<<".\n\n";
        return true;
    }

    virtual string serialize() const {
        ostringstream oss;
        oss << uniqueID <<","<< username << "," << password << "," << role << "," << account.serialize();
        return oss.str();
    }


    virtual void deserialize(const string &data) {
        istringstream iss(data);
        string token;
        vector<string> tokens;
        while(getline(iss, token, ',')) {
            tokens.push_back(token);
        }
        if(tokens.size() < 4)
            return;
        uniqueID = tokens[0];
        username = tokens[1];
        password = tokens[2];
        role = tokens[3];
        account.deserialize(tokens[4]);
    }

    virtual ~User() {}
};

//Derived classes (student,librarian,faculty)
class Student : public User {
private:
    int borrowLimit;
    int maxBorrowDays;
    double finerate;
public:
    Student() : User(), borrowLimit(3), maxBorrowDays(15) ,finerate(10.00){
        role = "Student";
    }
    Student(const string &uid,const string &uname, const string &pwd)
        : User(uid,uname, pwd, "Student"), borrowLimit(3), maxBorrowDays(15) ,finerate(10.00){}

    void calcfine(int days)
    {
        if(days<=maxBorrowDays)
        return;

        double f = (days-maxBorrowDays)*finerate;
        if(days-maxBorrowDays==1)
        cout<<"You are "<<days-maxBorrowDays<<" day late! Fine of Rs."<<f<<" has been incurred.\n";
        else
        cout<<"You are "<<days-maxBorrowDays<<" days late! Fine of Rs."<<f<<" has been incurred.\n";
        User::setfine(f);
    }
    bool borrowBook(Book *book,const string&date) override {
        if(account.getBorrowedBooks().size() >= (size_t)borrowLimit) {
            cout << "Student " << username << " reached the borrow limit." << endl;
            return false;
        }
        return User::borrowBook(book,date);
    }
};

class Faculty : public User {
private:
    int borrowLimit;
    int maxBorrowDays;
public:
    Faculty() : User(), borrowLimit(5), maxBorrowDays(30) {
        role = "Faculty";
    }
    Faculty(const string&uid,const string &uname, const string &pwd)
        : User(uid,uname, pwd, "Faculty"), borrowLimit(5), maxBorrowDays(30) {}

    void display(int days)
    {
        if(days<=maxBorrowDays)
        return;

        if(days-maxBorrowDays==1)
        cout<<"You are "<<days-maxBorrowDays<<" day late!\n";
        else
        cout<<"You are "<<days-maxBorrowDays<<" days late!\n";
    }
    bool borrowBook(Book *book,const string&date) override {
        if(account.getBorrowedBooks().size() >= (size_t)borrowLimit) {
            cout << "Faculty " << username << " reached the borrow limit." << endl;
            return false;
        }
        return User::borrowBook(book,date);
    }
};

class Librarian : public User {
public:
    Librarian() : User() {
        role = "Librarian";
    }
    Librarian(const string &uid,const string &uname, const string &pwd)
        : User(uid,uname, pwd, "Librarian") {}

    // Librarians are not allowed to borrow books.
    bool borrowBook(Book *book,const string&date) override {
        cout << "Librarians cannot borrow books." << endl;
        return false;
    }
};

//Library class
class Library {
private:
    vector<Book*> books;
    vector<Student*> students;
    vector<Faculty*> faculties;
    vector<Librarian*> librarians;
    // File names.
    const string booksFile = "books.txt";
    const string studentsFile = "students.txt";
    const string facultiesFile = "faculty.txt";
    const string librariansFile = "librarians.txt";
    const string historyFile = "borrowinghistory.txt";

public:
    Library() {
        loadBooks();
        loadStudents();
        loadFaculties();
        loadLibrarians();
    }
    ~Library() {
        saveBooks();
        saveStudents();
        saveFaculties();
        saveLibrarians();

        for(auto b : books) delete b;
        for(auto s : students) delete s;
        for(auto f : faculties) delete f;
        for(auto l : librarians) delete l;
    }

    void addBook(Book *book) { books.push_back(book); }
    Book* findBook(const string &isbn) {
        for(auto b : books)
            if(b->getISBN() == isbn)
                return b;
        return nullptr;
    }
    void print_available_books()
    {
        for(auto b:books)
        {
            if(b->getStatus()=="Available")
            {
                cout<<endl;
                cout<<"ISBN: "<<b->getISBN()<<"\n";
                cout<<"Title: "<<b->getTitle()<<"\n";
                cout<<"Author: "<<b->getAuthor()<<"\n";
                cout<<"Publisher: "<<b->getPublisher()<<"\n";
                cout<<"\n";
            }
        }
    }
    void print_reserved_books(string uid)
    {
        bool check=false;
        for(auto b:books)
        {
            if(b->getStatus()=="Reserved" && b->getreservedby()==uid)
            {
                check=true;
                cout<<endl;
                cout<<"ISBN: "<<b->getISBN()<<"\n";
                cout<<"Title: "<<b->getTitle()<<"\n";
                cout<<"Author: "<<b->getAuthor()<<"\n";
                cout<<"Publisher: "<<b->getPublisher()<<"\n";
                cout<<"Borrowed by: "<<b->getborrower()<<"\n";
                cout<<"\n";
            }
        }
        if(!check)
        cout<<"\nYou didn't reserve any book.\n\n";
        cout<<"=======================================================================================\n";
    }
    bool print_borrowed_books(string uid)
    {
        bool check=false;
        for(auto b:books)
        {
            if(b->getStatus()=="Borrowed" && b->getborrower()!=uid)
            {
                check=true;
                cout<<endl;
                cout<<"ISBN: "<<b->getISBN()<<"\n";
                cout<<"Title: "<<b->getTitle()<<"\n";
                cout<<"Author: "<<b->getAuthor()<<"\n";
                cout<<"Publisher: "<<b->getPublisher()<<"\n";
                cout<<"Borrowed by: "<<b->getborrower()<<"\n";
                cout<<"\n";
            }
        }
        if(!check)
        {
        cout<<"\n No borrowed books for you to reserve :(\n";
        return false;
        }
        return true;
    }
    void print_all_books()
    {
        for(auto b:books)
        {
                cout<<endl;
                cout<<"ISBN: "<<b->getISBN()<<"\n";
                cout<<"Title: "<<b->getTitle()<<"\n";
                cout<<"Author: "<<b->getAuthor()<<"\n";
                cout<<"Publisher: "<<b->getPublisher()<<"\n";
                cout<<"Status: "<<b->getStatus()<<"\n";
                cout<<"Borrowed By: "<<b->getborrower()<<"\n";
                cout<<"Reserved By: "<<b->getreservedby()<<"\n";
                cout<<"\n";
        }
    }
    bool returned(string isbn)
    {
        Book* b= findBook(isbn);
        if(b->getborrower()!="None")
        {
        cout<<"This book has not been returned by "<<b->getborrower()<<" yet!";
        return false;
        }
        return true;
    }
    bool isreserved(string isbn,string uid)
    {
        Book* b = findBook(isbn);
        if(b->getStatus()=="Reserved" && b->getreservedby()!=uid)
        {
            cout<<"This book was not reserved by you.\n";
            return false;
        }
        return true;
    }
    void remove_book(string isbn)
    {
        Book *b = findBook(isbn);
        for(auto s:students)
        {
            for(string i:(s->getAccount()).getBorrowedBooks())
            {
                if(i==isbn)
                {
                    cout<<"Cannot remove this book. It is currently borrowed by "<<s->getUsername()<<"\n";
                    return;
                }
            }
        }
        for(auto s:faculties)
        {
            for(string i:(s->getAccount()).getBorrowedBooks())
            {
                if(i==isbn)
                {
                    cout<<"Cannot remove this book. It is currently borrowed by "<<s->getUsername()<<"\n";
                    return;
                }
            }
        }
        auto it = find(books.begin(),books.end(),b);
        if(it==books.end())
        {
            cout<<"Book not found.\n";
            return;
        }
        books.erase(it);
        cout<<"\""<<b->getTitle()<<"\" removed successfully!";
    }
    void print_registered_students()
    {
        for(auto s:students)
        {
            cout<<"UniqueID: "<<s->getUniqueID()<<endl;
            cout<<"Username: "<<s->getUsername()<<endl;
            cout<<endl;
        }
    }
    void print_registered_faculties()
    {
        for(auto f:faculties)
        {
            cout<<"UniqueID: "<<f->getUniqueID()<<endl;
            cout<<"Username: "<<f->getUsername()<<endl;
            cout<<endl;
        }
    }
    void print_registered_librarians()
    {
        for(auto l:librarians)
        {
            cout<<"UniqueID: "<<l->getUniqueID()<<endl;
            cout<<"Username: "<<l->getUsername()<<endl;
            cout<<endl;
        }
    }
    void print_borrowing_history(User* user)
    {
        vector<tuple<string,string,string>> borrowingHistory = (user->getAccount()).getBorrowingHistory();
        if(borrowingHistory.size()==0)
        {
            cout<<"You don't have any books in your borrowing history.\n";
            return;
        }
        for(auto p:borrowingHistory)
        {
            Book* b = findBook(get<0>(p));
            cout<<"\nISBN :"<<b->getISBN()<<"\nTitle : \""<<b->getTitle()<<"\"\nBorrowed on :"<<get<1>(p)<<"\nReturned on :"<<get<2>(p)<<"\n\n";
        }
    }
    void update_book()
    {
        cout<<"Enter ISBN of the book you want to update: ";
        string isbn;
        cin.ignore();
        getline(cin,isbn);
        Book* book = findBook(isbn);
        if(book==nullptr)
        {
            cout<<"Invalid ISBN.\n";
            return;
        }
        cout<<"Book found!\nWhat do you want to update?\n";
        while(1)
        {
        cout<<"1. Title\n2. Author\n3. Publisher\n4. Leave\n";
        string title,author,publisher;
        int choice;
        cin>>choice;
        if (cin.fail()) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number.\n";
            continue;
        }
        bool exit=false;
        switch(choice)
        {
            case 1:
            {
            cin.ignore();
            cout<<"Enter Title of the Book: \n";
            getline(cin,title);
            book->setTitle(title);
            cout<<"Title Updated.\n";
            }
            break;
            case 2:
            {
                cin.ignore();
                cout<<"Enter Author of the Book: \n";
                getline(cin,author);
                book->setAuthor(author);
                cout<<"Author Updated\n";
            }
            break;
            case 3:
            {
                cin.ignore();
                cout<<"Enter Publisher of the Book: \n";
                getline(cin,publisher);
                book->setPublisher(publisher);
                cout<<"Publisher Updated\n";
            }
            break;
            case 4:
            {
                exit=true;
            }
            break;
            default:
            {
                cout<<"Invalid Input.\n";
            }
            break;
        }
        if(exit)
        return;
        }
    }
    bool faculty_overdue(Faculty *faculty,string date)
    {
        for(string s:(faculty->getAccount()).getBorrowedBooks())
        {
            Book *b = findBook(s);
            int days = daysBetweenDates(date,b->getdate());
            if(days>60)
            {
                cout<<"You have one or more books overdue. Kindly return them first.\n";
                return true;
            }
        }
        return false;
    }
    bool student_overdue(Student* student,string date)
    {
        for(string s:(student->getAccount()).getBorrowedBooks())
        {
            Book* b =findBook(s);
            int days = daysBetweenDates(date,b->getdate());
            if(days>15)
            {
                cout<<"You have one or more books overdue. Kindly return them first.\n";
                return true;
            }
        }
        return false;
    }
    bool isunique(string uid)
    {
        for(auto s:students)
        {
            if(s->getUniqueID()==uid)
            return false;
        }
        for(auto f:faculties)
        {
            if(f->getUniqueID()==uid)
            return false;
        }
        for(auto l:librarians)
        {
            if(l->getUniqueID()==uid)
            return false;
        }
        return true;
    }
    bool isbn_is_unique(string isbn)
    {
        for(auto b:books)
        {
            if(b->getISBN()==isbn)
            return false;
        }
        return true;
    }
    void addStudent(Student* student){students.push_back(student);}
    void addnewStudent() { 
        cout<<"Enter details of the student: \n";
        string uid,username,pass;
        cin.ignore();
        while(1)
        {
        cout<<"Enter UniqueID: ";
        getline(cin,uid);
        if(!isunique(uid))
        {
            cout<<"ID is not unique.\n";
            continue;
        }
        break;
        }
        cout<<"Enter username: ";
        getline(cin,username);
        cout<<"Enter password: ";
        getline(cin,pass);
        addStudent(new Student(uid,username,pass));
        cout<<"Student "<<username<<" added successfully!\n";
    }
    Student* findStudent(const string &uid) {
        for(auto s : students)
            if(s->getUniqueID() == uid)
                return s;
        return nullptr;
    }
    void remove_student(string uid)
    {
        Student *s = findStudent(uid);
        auto it = find(students.begin(),students.end(),s);
        if(it==students.end())
        {
            cout<<"Student not found.\n";
            return;
        }
        else if((s->getAccount()).getBorrowedBooks().size()!=0)
        {
            cout<<"This student has a book borrowed. Cannot remove this user.\n";
            return;
        }   
        bool flag = false;
        for(auto b : books)
        {
            if(b->getreservedby()==s->getUniqueID())
            {
                flag=true;
                break;
            }
        }
        if(flag)
        {
            cout<<"This student has placed a reservation on a book. Cannot remove this user.\n";
            return;
        }
        students.erase(it);
        cout<<"\""<<s->getUsername()<<"\" removed successfully!";
    }
    void addFaculty(Faculty *faculty) { faculties.push_back(faculty); }
    void addnewFaculty() { 
        cout<<"Enter details of the faculty member: \n";
        string uid,username,pass;
        cin.ignore();
        while(1)
        {
        cout<<"Enter UniqueID: ";

        getline(cin,uid);
        if(!isunique(uid))
        {
            cout<<"ID is not unique.\n";
            continue;
        }
        break;
        }
        cout<<"Enter username: ";
        getline(cin,username);
        cout<<"Enter password: ";
        getline(cin,pass);
        addFaculty(new Faculty(uid,username,pass));
        cout<<"Faculty member "<<username<<" added successfully!\n";
    }
    Faculty* findFaculty(const string &uid) {
        for(auto f : faculties)
            if(f->getUniqueID() == uid)
                return f;
        return nullptr;
    }
    void remove_faculty(string uid)
    {
        Faculty *f = findFaculty(uid);
        auto it = find(faculties.begin(),faculties.end(),f);
        if(it==faculties.end())
        {
            cout<<"Faculty member not found.\n";
            return;
        }
        else if((f->getAccount()).getBorrowedBooks().size()!=0)
        {
            cout<<"This faculty member has a book borrowed. Cannot remove this user.\n";
            return;
        }
        for(auto b:books)
        {
            if(b->getreservedby()==f->getUniqueID())
            {
                cout<<"This faculty member has placed a reservation on a book. Cannot remove them.\n";
                return;
            }
        }
        faculties.erase(it);
        cout<<"\""<<f->getUsername()<<"\" removed successfully!";
    }
    void addLibrarian(Librarian *librarian) { librarians.push_back(librarian); }
    void addnewLibrarian() { 
        cout<<"Enter details of the librarian: \n";
        string uid,username,pass;
        cin.ignore();
        while(1)
        {
        cout<<"Enter UniqueID: ";
  
        getline(cin,uid);
        if(!isunique(uid))
        {
            cout<<"ID is not unique.\n";
            continue;
        }
        break;
        }
        cout<<"Enter username: ";
        getline(cin,username);
        cout<<"Enter password: ";
        getline(cin,pass);
        addLibrarian(new Librarian(uid,username,pass));
        cout<<"Librarian "<<username<<" added successfully!\n";
    }
    Librarian* findLibrarian(const string &uid) {
        for(auto l : librarians)
            if(l->getUniqueID() == uid)
                return l;
        return nullptr;
    }
    void remove_librarian(string uid)
    {
        Librarian *l = findLibrarian(uid);
        auto it = find(librarians.begin(),librarians.end(),l);
        if(it==librarians.end())
        {
            cout<<"Librarian not found.\n";
            return;
        }
        librarians.erase(it);
        cout<<"\""<<l->getUsername()<<"\" removed successfully!";
    }

    void loadBooks() {
        ifstream in(booksFile);
        if(!in) {
            // If file not found, add some default books.
            addBook(new Book("The Great Gatsby", "F. Scott Fitzgerald", "Scribner", 1925, "ISBN001"));
            addBook(new Book("1984", "George Orwell", "Secker & Warburg", 1949, "ISBN002"));
            addBook(new Book("To Kill a Mockingbird", "Harper Lee", "J.B. Lippincott & Co.", 1960, "ISBN003"));
            addBook(new Book("Moby-Dick", "Herman Melville", "Harper & Brothers", 1851, "ISBN004"));
            addBook(new Book("Pride and Prejudice", "Jane Austen", "T. Egerton", 1813, "ISBN005"));
            addBook(new Book("The Catcher in the Rye", "J.D. Salinger", "Brown and Company", 1951, "ISBN006"));
            addBook(new Book("Brave New World", "Aldous Huxley", "Chatto & Windus", 1932, "ISBN007"));
            addBook(new Book("The Lord of the Rings", "J.R.R. Tolkien", "Allen & Unwin", 1954, "ISBN008"));
            addBook(new Book("The Hobbit", "J.R.R. Tolkien", "George Allen & Unwin", 1937, "ISBN009"));
            addBook(new Book("Crime and Punishment", "Fyodor Dostoevsky", "The Russian Messenger", 1866, "ISBN010"));
            return;
        }
        string line;
        while(getline(in, line)) {
            if(line.empty()) continue;
            Book b = Book::deserialize(line);
            addBook(new Book(b));
        }
        in.close();
    }

    void saveBooks() {
        ofstream out(booksFile);
        for(auto b : books) {
            out << b->serialize() << "\n";
        }
        out.close();
    }

    void loadStudents() {
        ifstream in(studentsFile);
        if(!in) {
            // Create default students if file not found.
            addStudent(new Student("df1","Rudra", "ntl72"));
            addStudent(new Student("df2","Sinha", "hello"));
            addStudent(new Student("df3","Ronnie Coleman","thegoat"));
            addStudent(new Student("df4","Cutler","goat2"));
            addStudent(new Student("df5","Sulek","futuregoat"));
            return;
        }
        string line;
        while(getline(in, line)) {
            if(line.empty()) continue;
            Student* s = new Student();
            s->deserialize(line);
            addStudent(s);
        }
        in.close();
    }

    void saveStudents() {
        ofstream out(studentsFile);
        for(auto s : students) {
            out << s->serialize() << "\n";
        }
        out.close();
    }

    void loadFaculties() {
        ifstream in(facultiesFile);
        if(!in) {
            addFaculty(new Faculty("Default1","faculty1", "pass1"));
            addFaculty(new Faculty("Default2","faculty2","pass2"));
            addFaculty(new Faculty("Default3","faculty3","pass3"));
            return;
        }
        string line;
        while(getline(in, line)) {
            if(line.empty()) continue;
            Faculty* f = new Faculty();
            f->deserialize(line);
            addFaculty(f);
        }
        in.close();
    }

    void saveFaculties() {
        ofstream out(facultiesFile);
        for(auto f : faculties) {
            out << f->serialize() << "\n";
        }
        out.close();
    }

    void loadLibrarians() {
        ifstream in(librariansFile);
        if(!in) {
            addLibrarian(new Librarian("Epic","Walter White", "adminpass"));
            return;
        }
        string line;
        while(getline(in, line)) {
            if(line.empty()) continue;
            Librarian* l = new Librarian();
            l->deserialize(line);
            addLibrarian(l);
        }
        in.close();
    }

    void saveLibrarians() {
        ofstream out(librariansFile);
        for(auto l : librarians) {
            out << l->serialize() << "\n";
        }
        out.close();
    }

    void registerStudent(){
        string uid;
        cout<<"Enter your details.\n";
        while(1)
        {
        cout<<"Enter uniqueID (could be alphanumeric) {without spaces}: ";
        cin>>uid;
        if(!isunique(uid))
        {
            cout<<"This uniqueID is taken by someone else.\n";
            continue;
        }
        break;
        }
               cin.ignore();
        cout<<"Enter username: ";
        string usn;
        getline(cin,usn);
        string pass;
        string pass2;
        do{
        cout<<"Enter password: ";
        getline(cin,pass);
        cout<<"Confirm password: ";
        getline(cin,pass2);

        if(pass!=pass2)
        {
            cout<<"Password didn't match. Re-enter password\n";
        }
        }while(pass!=pass2);
        Student* newStudent = new Student(uid,usn,pass);
        students.push_back(newStudent);
        saveStudents();

        cout<<"Student registered successfully!\n";
    }

    void registerFaculty(){
        string uid;
        cout<<"Enter your details.\n";
        while(1)
        {
        cout<<"Enter uniqueID (could be alphanumeric) {without spaces}: ";
        cin>>uid;
        if(!isunique(uid))
        {
            cout<<"This uniqueID is taken by someone else.\n";
            continue;
        }
        break;
        }
               cin.ignore();
        cout<<"Enter username: ";
        string usn;
        getline(cin,usn);
        string pass;
        string pass2;
        do{
        cout<<"Enter password: ";
        getline(cin,pass);
        cout<<"Confirm password: ";
        getline(cin,pass2);

        if(pass!=pass2)
        {
            cout<<"Password didn't match. Re-enter password\n";
        }
        }while(pass!=pass2);
        Faculty* newFaculty = new Faculty(uid,usn,pass);
        faculties.push_back(newFaculty);
        saveFaculties();

        cout<<"Faculty registered successfully!\n";
    }

    void registerLibrarian(){
        string uid;
        cout<<"Enter your details.\n";
        while(1)
        {
        cout<<"Enter uniqueID (could be alphanumeric) {without spaces}: ";
        cin>>uid;
        if(!isunique(uid))
        {
            cout<<"This uniqueID is taken by someone else.\n";
            continue;
        }
        break;
        }
        cin.ignore();
        cout<<"Enter username: ";
        string usn;
        getline(cin,usn);
        string pass;
        string pass2;
        do{
        cout<<"Enter password: ";
        getline(cin,pass);
        cout<<"Confirm password: ";
        getline(cin,pass2);

        if(pass!=pass2)
        {
            cout<<"Password didn't match. Re-enter password\n";
        }
        }while(pass!=pass2);
        Librarian* newLibrarian = new Librarian(uid,usn,pass);
        librarians.push_back(newLibrarian);
        saveLibrarians();

        cout<<"Librarian registered successfully!\n";
    }
    User* loginStudent()
    {
          cin.ignore();
        cout<<"Enter uniqueID: ";
        string uid;
  
        getline(cin,uid);
        cout<<"Enter Username: ";
        string usn,pass;
        getline(cin,usn);
        cout<<"Enter Password: ";
        getline(cin,pass);

        for(auto s: students)
        {
            if(s->getUniqueID()==uid && s->getUsername()==usn && s->getPassword()==pass)
            {
                cout<<"\nLogin Successful!\nWelcome "<<usn<<"!\n";
                return s;
            }
        }
        cout<<"Invalid username or password or uniqueID\n";
        return nullptr;
    }
    User* loginFaculty()
    {
        cin.ignore();
        cout<<"Enter UniqueID: ";
        string uid;
        
        getline(cin,uid);
        cout<<"Enter Username: ";
        string usn,pass;
        getline(cin,usn);
        cout<<"Enter Password: ";
        getline(cin,pass);

        for(auto s: faculties)
        {
            if(s->getUniqueID()==uid && s->getUsername()==usn && s->getPassword()==pass)
            {
                cout<<"Login Successful!\nWelcome "<<usn<<"!\n";
                return s;
            }
        }
        cout<<"Invalid username or password or uniqueID\n";
        return nullptr;
    }
    User* loginLibrarian()
    {
        cin.ignore();
        cout<<"Enter UniqueID: ";
        string uid;
        
        getline(cin,uid);
        cout<<"Enter Username: ";
        string usn,pass;
        getline(cin,usn); 
        cout<<"Enter Password: ";
        getline(cin,pass);

        for(auto s: librarians)
        {
            if(s->getUniqueID()==uid && s->getUsername()==usn && s->getPassword()==pass)
            {
                cout<<"Login Successful!\nWelcome "<<usn<<"!\n";
                return s;
            }
        }
        cout<<"Invalid username or password or uniqueID\n";
        return nullptr;
    }
};
int checkUserType(User* user) {
    if (dynamic_cast<Student*>(user)) {
        return 1;
    } 
    else if (dynamic_cast<Faculty*>(user)) {
        return 2;
    } 
    else if (dynamic_cast<Librarian*>(user)) {
        return 3;
    } 
    else {
        return -1;
    }
}

int main() {
    Library lib;
    cout << "=============================\n";
    cout << "    WELCOME TO MY LIBRARY    \n";
    cout << "=============================\n";
    while(1)
    {
        cout << "========================" << endl;
        cout << "    Choose Your Role    " << endl;
        cout << "========================" << endl;
        cout<<"\n1. Student\n2. Faculty Member\n3. Librarian\n4. Exit\nEnter your choice(index no.): ";
        int c;
        cin>>c;
        if (cin.fail()) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number.\n";
            continue;
        }
        User* user=nullptr;
        bool exit=false;
        switch(c)
        {
            case 1:
            {
                while(1)
                {
                cout<<"Are you alreay registered?[Yes/No]\n";
                string ans;
                cin>>ans;
                transform(ans.begin(), ans.end(), ans.begin(), ::tolower);
                if(ans=="no")
                {
                    lib.registerStudent();
                    //lib.saveStudents();
                }
                else if(ans=="yes")
                {
                    user = lib.loginStudent();
                    if(user==nullptr)
                    continue;
                    break;
                }
                else
                cout<<"Invalid Inputs\n";
            }
            }
            break;
            case 2:
            {
                while(1)
                {
                cout<<"Are you alreay registered?[Yes/No]\n";
                string ans;
                cin>>ans;
                transform(ans.begin(), ans.end(), ans.begin(), ::tolower);
                if(ans=="no")
                {
                    lib.registerFaculty();
     
                }
                else if(ans=="yes")
                {
                    user = lib.loginFaculty();
                    if(user==nullptr)
                    continue;
                    break;
                }
                else
                cout<<"Invalid Inputs\n";
            }
            }
            break;
            case 3:
            while(1)
                {
                cout<<"Are you alreay registered?[Yes/No]\n";
                string ans;
                cin>>ans;
                transform(ans.begin(), ans.end(), ans.begin(), ::tolower);
                if(ans=="no")
                {
                    lib.registerLibrarian();
     
                }
                else if(ans=="yes")
                {
                    user = lib.loginLibrarian();
                    if(user==nullptr)
                    continue;
                    break;
                }
                else
                cout<<"Invalid Inputs\n";
            }
            break;
            case 4:
            exit=true;
            break;
            default:
            {
                cout<<"Invalid Inputs\n";
                continue;
            }
        }
        if(exit)
        {
            cout<<"Exiting...\n";
            cout << "=================\n";
            cout << "    GOOD BYE!     \n";
            cout << "=================\n";
        break;
        }
        switch(c)
        {
        case 1:
        {
            Student* student = dynamic_cast<Student*>(user);
            while(1)
            {
            cout<<"\n=======================================================================================\n\n";
            cout<<"1. Borrow a book\n2. View currently borrowed books\n3. Return a book\n4. Reserve a Book\n5. View Borrowing History\n6. Clear Fine\n7. Logout\n";
            cout<<"Enter your choice: ";
            int pp;
            cin>>pp;
            bool exit=false;
            if (cin.fail()) {
                cin.clear(); 
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input! Please enter a number.\n";
                continue;
            }
            cout<<"\n=======================================================================================\n\n";
            switch(pp)
            {
                case 1:
                {
                    if(user->getfine()!=0.0)
                    {
                        cout<<"You have unpaid dues. Clear them first\n";
                        break;
                    }
                    Book* book=nullptr;
                    cout << "\n=====================\n";
                    cout << "  AVAILABLE BOOKS   \n";
                    cout << "=====================\n";
                    lib.print_available_books();
                    cout << "=====================\n";
                    cout << "  RESERVED BOOKS    \n";
                    cout << "=====================\n";
                    lib.print_reserved_books(user->getUniqueID());
                    cout<<"Enter ISBN of the book you want to borrow: ";
                    while(1)
                    {
                    string isbn;
                    cin>>isbn;
                    book = lib.findBook(isbn);
                    if(book==nullptr)
                    {
                        cout<<"Enter valid ISBN\n";
                        continue;
                    }
                    if(!lib.returned(isbn))
                    {
                        break;
                    }
                    if(!lib.isreserved(isbn,student->getUniqueID()))
                    break;
                    cout<<"Enter today's date (dd/mm/yyyy) {with slashes}: ";
                    string date;
                    while(1)
                    {
                    cin>>date;
                    if(!isValidDateFormat(date))
                    {
                        cout<<"Enter valid date.\n";
                        continue;
                    }
                    break;
                    }
                    if(lib.student_overdue(student,date))
                    break;
                    user->borrowBook(book,date);
                    break;
                    }
                }
                break;
                case 2:
                {
                    Account acc=user->getAccount();
                    vector<string> ISBNs = acc.getBorrowedBooks();
                    if(ISBNs.size()==0)
                    {
                        cout<<"You don't have any borrowed books.\n";
                        break;
                    }
                    for(string s :ISBNs)
                    {
                        Book* b = lib.findBook(s);
                        cout<<"ISBN :"<<b->getISBN()<<"\nTitle : \""<<b->getTitle()<<"\"\nBorrowed on :"<<b->getdate()<<"\n\n";
                    }
                }
                break;
                case 3:
                {
                    if((user->getAccount()).getBorrowedBooks().size()==0)
                    {
                        cout<<"You don't have any borrowed books.\n";
                        break;
                    }
                    Account acc=user->getAccount();
                    vector<string> ISBNs = acc.getBorrowedBooks();
                    cout<<"The following books were borrowed by you: \n";
                    for(string s :ISBNs)
                    {
                        Book* b = lib.findBook(s);
                        cout<<"ISBN :"<<b->getISBN()<<"\nTitle : \""<<b->getTitle()<<"\"\nBorrowed on :"<<b->getdate()<<"\n\n";
                    }
                    cout<<"Enter ISBN of the book you want to return: ";
                    Book* book = nullptr;
                    while(1)
                    {
                    string isbn;
                    cin>>isbn;
                    book = lib.findBook(isbn);
                    if(book==nullptr)
                    {
                        cout<<"Enter valid ISBN\n";
                        continue;
                    }
                    else if(book->getborrower()!=user->getUniqueID())
                    {
                        cout<<"\nThis book was not borrowed by you.\n";
                        break;
                    }
                    cout<<"Enter today's date (dd/mm/yyyy) {with slashes}: ";
                    string date;
                    while(1)
                    {
                    cin>>date;
                    if(!isValidDateFormat(date))
                    {
                        cout<<"Enter valid date.\n";
                        continue;
                    }
                    break;
                    }
                    int days = daysBetweenDates(date,book->getdate());
                    student->calcfine(days);
                    user->returnBook(book,date);
                    break;
                    }
                }
                break;
                case 4:
                {
                    if(user->getfine()!=0.0)
                    {
                        cout<<"You have unpaid dues. Clear them first\n";
                        break;
                    }
                    cout<<"Here is the list of all currently borrowed books by other users.\n";
                    if(!lib.print_borrowed_books(student->getUniqueID()))
                    break;
                    cout<<"Enter the ISBN of the book you want to reserve: ";
                    Book* book=nullptr;
                    while(1)
                    {
                        string isbn;
                        cin>>isbn;
                        book = lib.findBook(isbn);
                        if(book==nullptr)
                        {
                            cout<<"Invalid ISBN.\nEnter valid ISBN: ";
                            continue;
                        }
                        break;
                    }
                    book->setStatus("Reserved");
                    book->setreservedby(user->getUniqueID());
                    cout<<"\""<<book->getTitle()<<"\""<<" has been reserved by "<<user->getUsername()<<"!\n";
                }
                break;
                case 5:
                {
                    cout << "=================\n";
                    cout << "     HISTORY     \n";
                    cout << "=================\n";
                    lib.print_borrowing_history(user);
                }
                break;
                case 6:
                {
                    if(user->getfine()==0.0)
                    {
                        cout<<"You have no dues!\n";
                        break;
                    }
                    cout<<"Current dues: "<<user->getfine()<<endl;
                    cout<<"How much do you want to pay?\n";
                    double ans;
                    cin>>ans;
                    if (cin.fail()) {
                        cin.clear(); 
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid input! Please enter a decimal number.\n";
                        break;;
                    }
                    if(ans<0)
                    {
                        cout<<"Please enter a positive value.\n";
                    }
                    else if(ans>=user->getfine())
                    {
                        cout<<"Fine has been cleared!\n";
                        user->setfine(0.0);
                    }
                    else
                    {
                        double f=user->getfine()-ans;
                        user->setfine(f);
                        cout<<"Dues left: "<<user->getfine()<<endl;
                    }
                }
                break;
                case 7:
                exit=true;
                break;
                default:
                cout<<"Invalid Input.\n";
                break;
            }
            if(exit)
            {
                cout<<"Logging out...\n\n";
                break;
            }
        }
    }
    break;
    case 2:
    {
        Faculty* faculty = dynamic_cast<Faculty*>(user);
            while(1)
            {
            cout<<"\n=======================================================================================\n\n";
            cout<<"1. Borrow a book\n2. View currently borrowed books\n3. Return a book\n4. Reserve a Book\n5. View Borrowing History\n6. Logout\n";
            cout<<"Enter your choice: ";
            int pp;
            cin>>pp;
            if (cin.fail()) {
                cin.clear(); 
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input! Please enter a number.\n";
                continue;
            }
            cout<<"\n=======================================================================================\n\n";
            bool exit=false;
            switch(pp)
            {
                case 1:
                {
                    Book* book=nullptr;
                    cout << "=====================\n";
                    cout << "  AVAILABLE BOOKS   \n";
                    cout << "=====================\n";
                    lib.print_available_books();
                    cout << "=====================\n";
                    cout << "  RESERVED BOOKS    \n";
                    cout << "=====================\n";
                    lib.print_reserved_books(user->getUniqueID());
                    cout<<"Enter ISBN of the book you want to borrow: ";
                    while(1)
                    {
                    string isbn;
                    cin>>isbn;
                    book = lib.findBook(isbn);
                    if(book==nullptr)
                    {
                        cout<<"Enter valid ISBN\n";
                        continue;
                    }
                    if(!lib.returned(isbn))
                    {
                        break;
                    }
                    if(!lib.isreserved(isbn,user->getUniqueID()))
                    break;
                    cout<<"Enter today's date (dd/mm/yyyy) {with slashes}: ";
                    string date;
                    while(1)
                    {
                    cin>>date;
                    if(!isValidDateFormat(date))
                    {
                        cout<<"Enter valid date.\n";
                        continue;
                    }
                    break;
                    }
                    if(lib.faculty_overdue(faculty,date))
                    break;
                    user->borrowBook(book,date);
                    break;
                    }
                }
                break;
                case 2:
                {
                    Account &acc= user->getAccount();
                    vector<string> ISBNs = acc.getBorrowedBooks();
                    if(ISBNs.size()==0)
                    {
                        cout<<"You don't have any borrowed books.\n";
                        break;
                    }
                    for(string s :ISBNs)
                    {
                        Book* b = lib.findBook(s);
                        cout<<"ISBN :"<<b->getISBN()<<"\nTitle : \""<<b->getTitle()<<"\"\nBorrowed on :"<<b->getdate()<<"\n\n";
                    }
                }
                break;
                case 3:
                {
                    if((user->getAccount()).getBorrowedBooks().size()==0)
                    {
                        cout<<"You don't have any borrowed books.\n";
                        break;
                    }
                    Account acc=user->getAccount();
                    vector<string> ISBNs = acc.getBorrowedBooks();
                    cout<<"The following books were borrowed by you: \n";
                    for(string s :ISBNs)
                    {
                        Book* b = lib.findBook(s);
                        cout<<"ISBN :"<<b->getISBN()<<"\nTitle : \""<<b->getTitle()<<"\"\nBorrowed on :"<<b->getdate()<<"\n\n";
                    }
                    cout<<"Enter ISBN of the book you want to return: ";
                    Book* book = nullptr;
                    while(1)
                    {
                    string isbn;
                    cin>>isbn;
                    book = lib.findBook(isbn);
                    if(book==nullptr)
                    {
                        cout<<"Enter valid ISBN\n";
                        continue;
                    }
                    else if(book->getborrower()!=user->getUniqueID())
                    {
                        cout<<"\nThis book was not borrowed by you.\n";
                        break;
                    }
                    cout<<"Enter today's date (dd/mm/yyyy) {with slashes}: ";
                    string date;
                    while(1)
                    {
                    cin>>date;
                    if(!isValidDateFormat(date))
                    {
                        cout<<"Enter valid date.\n";
                        continue;
                    }
                    break;
                    }
                    int days = daysBetweenDates(date,book->getdate());
                    faculty->display(days);
                    user->returnBook(book,date);
                    break;
                    }
                }
                break;
                case 4:
                {
                    cout<<"\nHere is the list of all currently borrowed books by other users.\n";
                    if(!lib.print_borrowed_books(user->getUniqueID()))
                    break;
                    cout<<"Enter the ISBN of the book you want to reserve: ";
                    Book* book=nullptr;
                    while(1)
                    {
                        string isbn;
                        cin>>isbn;
                        book = lib.findBook(isbn);
                        if(book==nullptr)
                        {
                            cout<<"Invalid ISBN.\nEnter valid ISBN: ";
                            continue;
                        }
                        break;
                    }
                    book->setStatus("Reserved");
                    book->setreservedby(user->getUniqueID());
                    cout<<"\""<<book->getTitle()<<"\""<<" has been reserved by "<<user->getUsername()<<"!\n";
                }
                break;
                case 5:
                {
                    cout << "=================\n";
                    cout << "     HISTORY     \n";
                    cout << "=================\n";
                    lib.print_borrowing_history(user);
                }
                break;
                case 6:
                exit=true;
                break;

                default:
                cout<<"Invalid Inputs\n";
                break;
            }
            if(exit)
            {
                cout<<"Logging out...\n\n";
                break;
            }
        }
    }
    break;
    case 3:
    {
        Librarian* librarian = dynamic_cast<Librarian*>(user);
        Account acc= user->getAccount();
            while(1)
            {
            cout<<"\n=======================================================================================\n\n";
            cout<<"1. See all books in the library\n2. Add a Book\n3. Remove a Book\n4. Update a Book\n5. See all registered students\n6. Add a student\n7. Remove a student\n";
            cout<<"8. See all registered faculties\n9. Add a faculty\n10. Remove a faculty\n";
            cout<<"11. See all registered Librarians\n12. Logout\n";
            cout<<"Enter your choice: ";
            int pp;
            cin>>pp;
            if (cin.fail()) {
                cin.clear(); 
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input! Please enter a number.\n";
                continue;
            }
            cout<<"\n=======================================================================================\n\n";
            bool exit=false;
            switch(pp)
            {
                case 1:
                {
                    lib.print_all_books();
                }
                break;
                case 2:
                {
                    string title, author, publisher, isbn;
                    int year;
                    cout << "Enter Details of the Book.\n";
                    cin.ignore();
                    cout << "Title: ";
                    getline(cin, title);

                    cout << "Author: ";
                    getline(cin, author);

                    cout << "Publisher: ";
                    getline(cin, publisher);

                    cout << "ISBN: ";
                    getline(cin, isbn);
                    if(!lib.isbn_is_unique(isbn))
                    {
                        cout<<"Another book with same ISBN exists. Choose a different ISBN.\n";
                        break;
                    }

                    cout << "Year: ";
                    cin >> year;

                    lib.addBook(new Book(title, author, publisher, year, isbn));

                }
                break;
                case 3:
                {
                    cout<<"\nThese are the available books in the library:\n";
                    lib.print_available_books();
                    cout<<"Enter ISBN of the Book you want to remove: ";
                    string isbn;
                    cin>>isbn;
                    lib.remove_book(isbn);
                }
                break;
                case 4:
                {
                    cin.ignore();
                    cout<<"Enter ISBN of the book you want to update: ";
                    string isbn;
                    
                    getline(cin,isbn);
                    Book* book = lib.findBook(isbn);
                    if(book==nullptr)
                    {
                        cout<<"Invalid ISBN.\n";
                        break;
                    }
                    cout<<"Book found!\nWhat do you want to update?\n";
                    while(1)
                    {
                    cout<<"1. Title\n2. Author\n3. Publisher\n4. Leave\n";
                    string title,author,publisher;
                    int choice;
                    cin>>c;
                    if (cin.fail()) {
                        cin.clear(); 
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid input! Please enter a number.\n";
                        continue;
                    }
                    bool exit=false;
                    switch(c)
                    {
                        case 1:
                        {
                        cout<<"Enter Title of the Book: \n";
                        cin.ignore();
                        getline(cin,title);
                        book->setTitle(title);
                        }
                        break;
                        case 2:
                        {
                            cout<<"Enter Author of the Book: \n";
                            cin.ignore();
                            getline(cin,author);
                            book->setAuthor(author);
                        }
                        break;
                        case 3:
                        {
                            cout<<"Enter Publisher of the Book: \n";
                            cin.ignore();
                            getline(cin,publisher);
                            book->setPublisher(publisher);
                        }
                        break;
                        case 4:
                        {
                            exit=true;
                        }
                        break;
                        default:
                        {
                            cout<<"Invalid Input.\n";
                        }
                        break;
                        }
                        if(exit)
                        break;
                        }
                    }
                break;
                case 5:
                {
                    lib.print_registered_students();
                }
                break;
                case 6:
                {
                    lib.addnewStudent();
                }
                break;
                case 7:
                {
                    cout<<"Enter uniqueID of the student you want to remove: ";
                    string uid;
                    cin.ignore();
                    getline(cin,uid);
                    lib.remove_student(uid);
                }
                break;
                case 8:
                {
                    lib.print_registered_faculties();
                }
                break;
                case 9:
                {
                    lib.addnewFaculty();
                }
                break;
                case 10:
                {
                    cout<<"Enter uniqueID of the faculty you want to remove: ";
                    string uid;
                    cin.ignore();
                    getline(cin,uid);
                    lib.remove_faculty(uid);
                }
                break;
                case 11:
                {
                    lib.print_registered_librarians();
                }
                break;
                case 12:
                exit=true;
                break;
                default:
                cout<<"Invalid Inputs\n";
                break;
            }
            if(exit)
            {
                cout<<"Logging out...\n\n";
                break;
            }
        }
    }
    }
    }
}
