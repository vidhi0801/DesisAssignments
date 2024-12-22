#include "bookinventory.h"
#include<iostream>

int main() {
    // Create a Bookshop object
    Bookshop bookshop;

    // Add Books to the inventory
    bookshop.addBook(Book("The Catcher in the Rye", "J.D. Salinger", "Little, Brown", "1st Edition", 101, 10.99, 50));
    cout << "Book added successfully!" << std::endl;

    bookshop.addBook(Book("To Kill a Mockingbird", "Harper Lee", "J.B. Lippincott & Co.", "1st Edition", 102, 12.99, 30));
    cout << "Book added successfully!" << std::endl;

    // Add Customers
    bookshop.addCustomer(Customer("Alice", 1001, "alice@example.com"));
    cout << "Customer Alice added successfully!" << std::endl;

    bookshop.addCustomer(Customer("Bob", 1002, "bob@example.com"));
    cout << "Customer Bob added successfully!" << std::endl;

    // Display available books
    cout << "Viewing books..." << std::endl;
    bookshop.viewBooks();

    // Make some purchases
    cout << "Purchasing books..." << std::endl;
    bookshop.purchaseBook("The Catcher in the Rye", 2, 1001);
    cout << "Purchase for Alice completed!" << std::endl;

    bookshop.purchaseBook("To Kill a Mockingbird", 1, 1002);
    cout << "Purchase for Bob completed!" << std::endl;

    // View customer purchase history
    cout << "Viewing purchase history for Alice..." << std::endl;
    bookshop.customers[0].viewPurchaseHistory();
    cout << "Viewing purchase history for Bob..." << std::endl;
    bookshop.customers[1].viewPurchaseHistory();

    // Display sales report
    cout << "Displaying sales report..." << std::endl;
    bookshop.displaySalesReport();

    return 0;
}
