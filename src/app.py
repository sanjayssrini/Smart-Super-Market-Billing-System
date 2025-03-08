from flask import Flask, render_template, request, redirect, url_for, session, jsonify
import database 

app = Flask(__name__)
app.secret_key = "sanjays12" 

@app.route('/', methods=['GET', 'POST'])
def login():
    print("----- Inside / route (Login) -----")
    if request.method == 'POST':
        phone_number = request.form['phone']
        customer = database.get_customer(phone_number)
        print("Phone number:", phone_number)
        print("Customer from database:", customer)

        if customer:
            session['customer_id'] = customer['cid']
            print("Session customer ID:", session.get('customer_id'))
            return redirect(url_for('main_menu'))
        else:
            if 'name' in request.form:
                name = request.form['name']
                customer_id = database.create_customer(phone_number, name)
                if customer_id:
                    session['customer_id'] = customer_id
                    print("Session customer ID:", session.get('customer_id'))
                    return redirect(url_for('main_menu'))
                else:
                    return render_template('page.html', error="Failed to create customer.")
            else:
                return render_template('page.html', error="Please enter your name.")
    else:
        return render_template('page.html')

@app.route('/check_phone', methods=['POST'])
def check_phone():
    phone_number = request.json.get('phone')
    customer = database.get_customer(phone_number)
    return jsonify({'exists': customer is not None})

@app.route('/main-menu')
def main_menu():
    print("----- Inside /main-menu route -----")
    if 'customer_id' in session:
        print("Customer ID found in session:", session['customer_id'])
        return render_template('main-menu.html')
    else:
        print("Customer ID not found in session, redirecting to login")
        return redirect(url_for('login'))

@app.route('/bill', methods=['GET', 'POST'])
def bill():
    print("----- Inside /bill route -----")
    if 'customer_id' in session:
        customer_id = session['customer_id']
        customer_name = database.get_customer_name(customer_id)
        print("Customer ID:", customer_id)
        print("Customer Name:", customer_name)

        if request.method == 'POST':
            product_id = request.form.get('product_id')
            quantity = int(request.form.get('quantity', 1))

            product = database.get_product(product_id)
            if product:
                bill_items = session.get('bill_items', [])
                bill_items.append({
                    'id': product['id'],
                    'name': product['name'],
                    'quantity': quantity,
                    'price': product['price'],
                    'amount': quantity * product['price']
                })
                session['bill_items'] = bill_items
                return jsonify(product)
            else:
                return jsonify({'error': 'Product not found!'}), 404

        return render_template('bill.html', customer_id=customer_id,customer_name=customer_name)
    else:
        return redirect(url_for('login')) 

@app.route('/remove_item/<int:item_id>')
def remove_item(item_id):
    print("----- Inside /remove_item route -----")
    if 'customer_id' in session:
        bill_items = session.get('bill_items', [])
        session['bill_items'] = [item for item in bill_items if item['id'] != item_id]
        return redirect(url_for('bill'))
    else:
        return redirect(url_for('login'))

@app.route('/generate_bill', methods=['POST'])
def generate_bill():
    return jsonify({'message': 'This route is no longer used for PDF generation.'}), 200

@app.route('/logout')
def logout():
    session.pop('customer_id', None)
    return redirect(url_for('login'))

if __name__ == '__main__':
    app.run(debug=True) 