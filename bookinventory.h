#ifndef BOOKINVENTORY_H
#define BOOKINVENTORY_H

#include <iostream>
#include <vector>
#include <string>

class Sale;   // Forward declaration
class Customer; // Forward declaration

class Book {
private:
    std::string title;
    std::string author;
    std::string publisher;
    std::string edition;
    int ID;
    double price;
    int stock;
public:
    Book(std::string title, std::string author, std::string publisher, 
         std::string edition, int ID, double price, int stock);
    friend class Bookshop;
    friend class Sale;
};

class Sale {
public:
    std::string title;
    int quantity;
    double totalPrice;
    Sale(std::string t, int q, double tp);
};

class Customer {
private:
    std::string name;
    int customerID;
    std::string contact;
    
public:
    std::vector<Sale> purchaseHistory;
    Customer(std::string name, int customerID, std::string contact);
    void addPurchase(Sale sale);
    void viewPurchaseHistory();
    friend class Bookshop;
};

class Bookshop {
    public:
    std::vector<Book> inventory;
    std::vector<Sale> sales;
    std::vector<Customer> customers;
    std::string normalizeString(const std::string& input);
    void addBook(Book newBook);
    void deleteBook(int idtoDelete);
    void viewBooks();
    void addCustomer(Customer newCustomer);
    void viewCustomers();
    void purchaseBook(std::string title, int quantity, int customerID);
    void displaySalesReport();
};

#endif // BOOKINVENTORY_H
