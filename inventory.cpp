#include <iostream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <map>
#include <string>
using namespace std;

// Book Class
class Book {
private:
    string title;      // Book title
    string author;     // Author name
    string publisher;  // Publisher name
    string edition;    // Edition of the book
    int ID;            // Unique book ID
    double price;      // Price of the book
    int stock;         // Number of books in stock
public:
    // Constructor to initialize Book details
    Book(string title, string author, string publisher, string edition, int ID, double price, int stock) {
        this->title = title;
        this->author = author;
        this->publisher = publisher;
        this->edition = edition;
        this->ID = ID;
        this->price = price;
        this->stock = stock;
    }
    friend class Bookshop; // Grant Bookshop access to private members
    friend class Sale;     // Grant Sale access to private members
};

// Sale Class
class Sale {
public:
    string title;       // Title of the book sold
    int quantity;       // Quantity sold
    double totalPrice;  // Total price of the sale

    // Constructor to initialize Sale details
    Sale(string t, int q, double tp) : title(t), quantity(q), totalPrice(tp) {}
};

// Customer Class
class Customer {
private:
    string name;                // Customer's name
    int customerID;             // Unique customer ID
    string contact;             // Customer's contact information
    vector<Sale> purchaseHistory; // Record of all purchases made by the customer
public:
    // Constructor to initialize Customer details
    Customer(string name, int customerID, string contact) {
        this->name = name;
        this->customerID = customerID;
        this->contact = contact;
    }

    // Add a new sale to the customer's purchase history
    void addPurchase(Sale sale) {
        purchaseHistory.push_back(sale);
    }

    // Display the customer's purchase history
    void viewPurchaseHistory() {
        if (purchaseHistory.empty()) {
            cout << "No purchases found for customer " << name << ".\n";
            return;
        }
        cout << "\n--- Purchase History for " << name << " ---\n";
        for (const auto& sale : purchaseHistory) {
            cout << "Title: " << sale.title << ", Quantity: " << sale.quantity 
                 << ", Total Price: $" << fixed << setprecision(2) << sale.totalPrice << endl;
        }
    }

    friend class Bookshop; // Grant Bookshop access to private members
};

// Bookshop Class
class Bookshop {
private:
    vector<Book> inventory;       // List of books in the inventory
    vector<Sale> sales;           // List of all sales
    vector<Customer> customers;   // List of all customers

    // Helper function to normalize a string for comparison (ignores case and spaces)
    string normalizeString(const string& input) {
        string normalized;
        for (char ch : input) {
            if (!isspace(ch)) { // Ignore spaces
                normalized += tolower(ch);
            }
        }
        return normalized;
    }

public:
    // Add a new book to the inventory
    void addBook(Book newBook) {
        inventory.push_back(newBook);
    }

    // Delete a book from the inventory based on its ID
    void deleteBook(int idtoDelete) {
        for (auto it = inventory.begin(); it != inventory.end();) {
            if (it->ID == idtoDelete) {
                inventory.erase(it);
                cout << "Book deleted successfully!\n";
                return;
            } else {
                ++it;
            }
        }
        cout << "Book not found in inventory.\n";
    }

    // Display all books in the inventory
    void viewBooks() {
        if (inventory.empty()) {
            cout << "No books in the inventory.\n";
            return;
        }

        cout << "\n--- Book Inventory ---\n";
        for (const auto &book : inventory) {
            cout << "Title: " << book.title << "\n";
            cout << "Author: " << book.author << "\n";
            cout << "Publisher: " << book.publisher << "\n";
            cout << "Edition: "  << book.edition << "\n";
            cout << "ID: " << book.ID << "\n";
            cout << "Price: $" << book.price << "\n";
            cout << "Stock: " << book.stock << "\n";
            cout << "-------------------------\n";
        }
    }

    // Add a new customer
    void addCustomer(Customer newCustomer) {
        customers.push_back(newCustomer);
        cout << "Customer added successfully!\n";
    }

    // Display all customers
    void viewCustomers() {
        if (customers.empty()) {
            cout << "No customers found.\n";
            return;
        }
        cout << "\n--- Customer List ---\n";
        for (const auto& customer : customers) {
            cout << "Name: " << customer.name << "\n";
            cout << "Customer ID: " << customer.customerID << "\n";
            cout << "Contact: " << customer.contact << "\n";
            cout << "-------------------------\n";
        }
    }

    // Purchase a book
    void purchaseBook(string title, int quantity, int customerID) {
        string normalizedTitle = normalizeString(title);
        for (auto& book : inventory) {
            if (normalizeString(book.title) == normalizedTitle) {
                if (book.stock >= quantity) {
                    double totalPrice = quantity * book.price;
                    book.stock -= quantity;
                    sales.push_back(Sale(book.title, quantity, totalPrice));

                    // Update customer's purchase history
                    for (auto& customer : customers) {
                        if (customer.customerID == customerID) {
                            customer.addPurchase(Sale(book.title, quantity, totalPrice));
                            cout << "\nPurchase successful! Total Price: $" << totalPrice << endl;
                            return;
                        }
                    }
                    cout << "\nCustomer not found. Purchase recorded without linking to customer.\n";
                    return;
                } else {
                    cout << "\nInsufficient stock available.\n";
                    return;
                }
            }
        }
        cout << "\nBook not found in inventory.\n";
    }

    // Display sales report
    void displaySalesReport() {
        cout << "\nSales Report:\n";
        cout << setw(20) << "Title" << setw(10) << "Quantity" << setw(15) << "Total Price\n";
        for (const auto& sale : sales) {
            cout << setw(20) << sale.title << setw(10) << sale.quantity 
                 << setw(15) << fixed << setprecision(2) << sale.totalPrice << endl;
        }
    }
};

// Main Function
int main() {
    Bookshop bookshop; // Create a Bookshop object
    int choice;        // Variable to store user choice

    do {
        // Display menu options
        cout << "\n--- Bookshop Management System ---\n";
        cout << "1. Add Book\n";
        cout << "2. Delete Book\n";
        cout << "3. View Books\n";
        cout << "4. Add Customer\n";
        cout << "5. View Customers\n";
        cout << "6. Purchase Book\n";
        cout << "7. Display Sales Report\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        // Perform actions based on user choice
        switch (choice) {
            case 1: {
                string title, author, publisher, edition;
                int ID, stock;
                double price;
                cout << "Enter Title: ";
                cin.ignore();
                getline(cin, title);
                cout << "Enter Author: ";
                getline(cin, author);
                cout << "Enter Publisher: ";
                getline(cin, publisher);
                cout << "Enter Edition: ";
                getline(cin, edition);
                cout << "Enter ID: ";
                cin >> ID;
                cout << "Enter Price: ";
                cin >> price;
                cout << "Enter Stock: ";
                cin >> stock;
                bookshop.addBook(Book(title, author, publisher, edition, ID, price, stock));
                break;
            }
            case 2: {
                int ID;
                cout << "Enter ID of the book to delete: ";
                cin >> ID;
                bookshop.deleteBook(ID);
                break;
            }
            case 3:
                bookshop.viewBooks();
                break;
            case 4: {
                string name, contact;
                int customerID;
                cout << "Enter Customer Name: ";
                cin.ignore();
                getline(cin, name);
                cout << "Enter Customer ID: ";
                cin >> customerID;
                cout << "Enter Contact: ";
                cin.ignore();
                getline(cin, contact);
                bookshop.addCustomer(Customer(name, customerID, contact));
                break;
            }
            case 5:
                bookshop.viewCustomers();
                break;
            case 6: {
                string title;
                int quantity, customerID;
                cout << "Enter Book Title: ";
                cin.ignore();
                getline(cin, title);
                cout << "Enter Quantity: ";
                cin >> quantity;
                cout << "Enter Customer ID: ";
                cin >> customerID;
                bookshop.purchaseBook(title, quantity, customerID);
                break;
            }
            case 7:
                bookshop.displaySalesReport();
                break;
            case 8:
                cout << "Exiting the system. Thank you!\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 8);

    return 0;
}

