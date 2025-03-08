import mysql.connector
import json

def connect_to_db():
    try:
        return mysql.connector.connect(
            host="localhost",
            user="root",
            password="sanjay", 
            database="bms"
        )
    except mysql.connector.Error as err:
        print(f"Database connection error: {err}")
        return None

def get_customer(phone_number):
    db = connect_to_db()
    if not db:
        return None

    cursor = db.cursor(dictionary=True)
    query = "SELECT cid, name FROM customers WHERE phno = %s"
    try:
        cursor.execute(query, (phone_number,))
        customer = cursor.fetchone()
        print(customer)
        return customer
    except mysql.connector.Error as err:
        print(f"Error fetching customer: {err}")
        return None
    finally:
        db.close()

def create_customer(phone_number, name):
    db = connect_to_db()
    if not db:
        return None

    cursor = db.cursor()
    query = "INSERT INTO customers (phno, name) VALUES (%s, %s)"
    try:
        cursor.execute(query, (phone_number, name))
        db.commit()
        customer_id = cursor.lastrowid
        return customer_id
    except mysql.connector.Error as err:
        print(f"Error creating customer: {err}")
        db.rollback()
        return None
    finally:
        db.close()

def create_bill(customer_id, total_amount, bill_items):
    db = connect_to_db()
    if not db:
        return None

    cursor = db.cursor()
    bill_items_json = json.dumps(bill_items)
    query = """
        INSERT INTO bills (customer_id, purchase_date, purchase_time, total_amount, bill_items)
        VALUES (%s, CURDATE(), CURTIME(), %s, %s)
    """
    try:
        cursor.execute(query, (customer_id, total_amount, bill_items_json))
        db.commit()
        bill_number = cursor.lastrowid
        return bill_number
    except mysql.connector.Error as err:
        print(f"Error creating bill: {err}")
        db.rollback()
        return None
    finally:
        db.close()


def get_customer_name(customer_id):
    db = connect_to_db()
    if not db:
        return "Unknown Customer"

    cursor = db.cursor(dictionary=True)
    query = "SELECT name FROM customers WHERE cid = %s"
    try:
        cursor.execute(query, (customer_id,))
        customer = cursor.fetchone()
        return customer['name'] if customer else "Unknown Customer"
    except mysql.connector.Error as err:
        print(f"Error fetching customer name: {err}")
        return "Unknown Customer"
    finally:
        db.close()

def get_product(product_id):
    db = connect_to_db()
    if not db:
        return None 

    cursor = db.cursor(dictionary=True)
    query = "SELECT * FROM products WHERE id = %s" 
    try:
        cursor.execute(query, (product_id,))
        product = cursor.fetchone()
        return product
    except mysql.connector.Error as err:
        print(f"Error fetching product: {err}")
        return None
    finally:
        db.close()