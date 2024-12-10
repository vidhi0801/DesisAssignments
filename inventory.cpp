#include <iostream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <map>
#include <string>
using namespace std;

// Book class represents a single book's details
class Book {
private:
    string title;      // Title of the book
    string author;     // Author's name
    string publisher;  // Publisher's name
    string edition;    // Edition details
    int ID;            // Unique identifier for the book
    double price;      // Price of the book
    int stock;         // Available stock of the book

public:
    // Constructor to initialize book details
    Book(string title, string author, string publisher, string edition, int ID, double price, int stock) {
        this->title = title;
        this->author = author;
        this->publisher = publisher;
        this->edition = edition;
        this->ID = ID;
        this->price = price;
        this->stock = stock;
    }

    // Declaring the friend classes to allow them access to private members
    friend class Bookshop;
    friend class Sale;
};

// Sale class tracks details of a single sale transaction
class Sale {
public:
    string title;      // Title of the sold book
    int quantity;      // Quantity sold
    double totalPrice; // Total price of the sale

    // Constructor to initialize a sale transaction
    Sale(string t, int q, double tp) : title(t), quantity(q), totalPrice(tp) {}
};

// Bookshop class manages the book inventory and sales operations
class Bookshop {
private:
    vector<Book> inventory; // List of all books in the inventory
    vector<Sale> sales;     // List of all completed sales

public:
    // Function to add a new book to the inventory
    void addBook(Book newBook) {
        inventory.push_back(newBook);
    }

    // Utility function to normalize strings (convert to lowercase and remove spaces)
    string normalizeString(const string& input) {
        string normalized;
        for (char ch : input) {
            if (!isspace(ch)) { // Ignore spaces
                normalized += tolower(ch);
            }
        }
        return normalized;
    }

    // Function to delete a book by its ID
    void deleteBook(int idtoDelete) {
        // Iterate over the inventory
        for (auto it = inventory.begin(); it != inventory.end(); /* no increment here */) {
            if (it->ID == idtoDelete) { // If book ID matches
                inventory.erase(it); // Remove the book from inventory
                cout << "Book deleted successfully!\n";
                return; // Exit after deletion
            } else {
                ++it; // Increment iterator if no deletion occurs
            }
        }
        cout << "Book not found in inventory.\n";
    }

    // Function to display all books in the inventory
    void viewBooks() {
        if (inventory.empty()) { // Check if inventory is empty
            cout << "No books in the inventory.\n";
            return;
        }

        cout << "\n--- Book Inventory ---\n";
        // Display details of each book
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

    // Function to check for books below a stock threshold
    void checkLowStock(int threshold) {
        cout << "\n--- Books Below Threshold Stock ---\n";
        bool found = false; // Track if any books are below threshold

        // Iterate through the inventory
        for (const auto &book : inventory) {
            if (book.stock < threshold) { // Check stock level
                cout << "Title: " << book.title << "\n";
                cout << "Author: " << book.author << "\n";
                cout << "Stock: " << book.stock << "\n";
                cout << "-------------------------\n";
                found = true;
            }
        }

        if (!found) {
            cout << "No books below the threshold stock.\n";
        }
    }

    // Function to handle book purchases
    void purchaseBook(string title, int quantity) {
        string normalizedTitle = normalizeString(title); // Normalize input title
        for (auto& book : inventory) { // Search for the book in inventory
            if (normalizeString(book.title) == normalizedTitle) {
                if (book.stock >= quantity) { // Check stock availability
                    double totalPrice = quantity * book.price; // Calculate total price
                    book.stock -= quantity; // Reduce stock
                    sales.push_back(Sale(book.title, quantity, totalPrice)); // Log the sale
                    cout << "\nPurchase successful! Total Price: $" << totalPrice << endl;
                    return;
                } else {
                    cout << "\nInsufficient stock available.\n";
                    return;
                }
            }
        }
        cout << "\nBook not found in inventory.\n";
    }

    // Function to display sales report
    void displaySalesReport() {
        cout << "\nSales Report:\n";
        cout << setw(20) << "Title" << setw(10) << "Quantity" << setw(15) << "Total Price\n";
        for (const auto& sale : sales) { // Display details of each sale
            cout << setw(20) << sale.title << setw(10) << sale.quantity 
                 << setw(15) << fixed << setprecision(2) << sale.totalPrice << endl;
        }
    }
};

int main() {
    Bookshop bookshop; // Create an instance of Bookshop

    int choice; // Variable to store user's choice
    do {
        // Display menu options
        cout << "\n--- Bookshop Management System ---\n";
        cout << "1. Add Book\n";
        cout << "2. Delete Book\n";
        cout << "3. View Books\n";
        cout << "4. Check Low Stock\n";
        cout << "5. Purchase Book\n";
        cout << "6. Display Sales Report\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: { // Add a new book
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
            case 2: { // Delete a book by ID
                int ID;
                cout << "Enter ID of the book to delete: ";
                cin >> ID;
                bookshop.deleteBook(ID);
                break;
            }
            case 3: // View all books
                bookshop.viewBooks();
                break;
            case 4: { // Check for low-stock books
                int threshold;
                cout << "Enter stock threshold: ";
                cin >> threshold;
                bookshop.checkLowStock(threshold);
                break;
            }
            case 5: { // Purchase a book
                string title;
                int quantity;
                cout << "Enter book title: ";
                cin.ignore();
                getline(cin, title);
                cout << "Enter quantity: ";
                cin >> quantity;
                bookshop.purchaseBook(title, quantity);
                break;
            }
            case 6: // Display sales report
                bookshop.displaySalesReport();
                break;
            case 7: // Exit
                cout << "Exiting the system. Thank you!\n";
                break;
            default: // Handle invalid choices
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 7);

    return 0; // End of program
}
