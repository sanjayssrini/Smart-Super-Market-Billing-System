# Smart Supermarket Self-Billing System

This is an innovative project done by [Sanjay S](https://github.com/sanjayssrini) and [Subhashini SM](https://github.com/subhashinism) on September 2024.

## Overview
Smart Supermarket Self-Billing System is an eco-friendly, efficient billing solution designed to streamline supermarket checkouts while reducing the carbon footprint by eliminating paper receipts. The project started as a C-based application utilizing MySQL for data management and Cairo for PDF generation, and later evolved into a Python-powered system with a modern web-based UI built using HTML, CSS, and JavaScript.

## Features
- **Paperless Billing:** Invoices are generated digitally, eliminating the need for paper receipts.
- **Barcode Scanning:** Utilizes webcam-based barcode scanning for fast product identification.
- **Customer Management:** Enables customer lookup through phone number and supports new customer registration.
- **Accurate Billing:** Real-time calculation of totals and detailed PDF invoice generation.
- **Modern Web Interface:** Responsive UI built with HTML, CSS, and JavaScript.

## 🛠️ Tech Stack
- **Frontend:** HTML, CSS, JavaScript
- **Backend:** Python, Flask
- **Database:** MySQL
- **PDF Generation:** Cairo Library
- **Original Prototype:** C Programming


## Installation

### Prerequisites
- MySQL Server
- GCC (for compiling the C version) or Python 3.x (for the Python version)
- Cairo Graphics Library (for PDF generation in C)
- A modern web browser

## MySQL Database Setup
Before running the application, set up your MySQL database with the following steps:

### 1. Create the Database
```sql
CREATE DATABASE bms;
USE bms;
```

### 2. Create the `customers` Table
```sql
CREATE TABLE customers (
    cid INT AUTO_INCREMENT PRIMARY KEY,
    phno VARCHAR(15) NOT NULL,
    name VARCHAR(100) NOT NULL
);
```

### 3. Create the `products` Table
```sql
CREATE TABLE products (
    pid INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    price DECIMAL(10,2) NOT NULL
);
```

### 4. Insert Sample Data into `products`
```sql
INSERT INTO products (name, price) VALUES
('Product A', 10.99),
('Product B', 15.49),
('Product C', 7.99);
```

### C Version Setup
1. **Install Dependencies:** Ensure MySQL, GCC, and Cairo are installed.
2. **Compile the Code:**
   ```bash
   gcc -o product_search product_search.c `mysql_config --cflags --libs` -I/usr/include/cairo -lcairo
   ```
3. **Run the Application:**
   ```bash
   ./product_search
   ```

### Python Version with Web UI Setup
1. **Create a Virtual Environment:**
   ```bash
   python3 -m venv venv
   source venv/bin/activate
   ```
2. **Install Dependencies:**
   ```bash
   pip install -r requirements.txt
   ```
3. **Configure Database:** Update MySQL connection settings in the configuration file as needed.
4. **Run the Application:**
   ```bash
   python app.py
   ```
5. **Access the UI:** Open your browser and navigate to `http://localhost:5000`.

## Usage
1. **Customer Lookup:** Enter your phone number to retrieve your customer profile or register as a new user.
2. **Product Scanning:** Use the barcode scanner or manually input product IDs to add items.
3. **Billing & Invoice Generation:** The system calculates totals, generates a PDF invoice, and logs transactions in the database.

## 🛠️ Future Improvements
- ✅ **WhatsApp/SMS API Integration** (Coming Soon 📲)
- 🔄 **Enhanced UI & UX**
- 💾 **More Optimized Database Queries**


## Contact
For questions or further information, please contact [sanjaysrinivasan095@gmail.com].

📢 *Contributions are welcome! Feel free to fork, improve, and raise a pull request.* 🚀

