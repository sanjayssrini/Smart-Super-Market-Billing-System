#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include <time.h>

void error_exit(MYSQL *con) {	
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

void generate_pdf(char ids[][6], char products[][100], double prices[], int quantities[], double amounts[], int num_items, double total, int customer_id, int bill_number, const char *customer_name) {
    cairo_surface_t *surface;
    cairo_t *cr;

    char pdf_filename[100];
    sprintf(pdf_filename, "ID%d%d.pdf", customer_id, bill_number);
    surface = cairo_pdf_surface_create(pdf_filename, 595, 842);
    cr = cairo_create(surface);

    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 18);
    cairo_move_to(cr, 50, 50);
    cairo_show_text(cr, "Supermarket Bill");

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char date[20], time_str[20];
    strftime(date, sizeof(date), "%Y-%m-%d", tm_info);
    strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);

    cairo_set_font_size(cr, 12);
    cairo_move_to(cr, 50, 80);
    cairo_show_text(cr, "Date:");
    cairo_move_to(cr, 100, 80);
    cairo_show_text(cr, date);
    
    cairo_move_to(cr, 50, 100);
    cairo_show_text(cr, "Time:");
    cairo_move_to(cr, 100, 100);
    cairo_show_text(cr, time_str);

    char bill_id[100];
    sprintf(bill_id, "Bill ID: ID%d%d", customer_id, bill_number);
    cairo_move_to(cr, 50, 120);
    cairo_show_text(cr, "Customer Name:");
    cairo_move_to(cr, 175, 120);
    cairo_show_text(cr, customer_name);

    cairo_move_to(cr, 50, 140);
    cairo_show_text(cr, bill_id);

    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, 50, 160);
    cairo_show_text(cr, "ID");
    cairo_move_to(cr, 150, 160);
    cairo_show_text(cr, "Name");
    cairo_move_to(cr, 300, 160);
    cairo_show_text(cr, "Qty");
    cairo_move_to(cr, 400, 160);
    cairo_show_text(cr, "Price");
    cairo_move_to(cr, 500, 160);
    cairo_show_text(cr, "Amount");

    cairo_set_line_width(cr, 1);
    cairo_move_to(cr, 50, 170);
    cairo_line_to(cr, 550, 170);
    cairo_stroke(cr);

    int y_offset = 190;
    for (int i = 0; i < num_items; i++) {
        cairo_move_to(cr, 50, y_offset);
        cairo_show_text(cr, ids[i]);

        cairo_move_to(cr, 150, y_offset);
        if (strlen(products[i]) > 15) {
            char wrapped_text[100];
            int wrap_index = 15;
            strncpy(wrapped_text, products[i], wrap_index);
            wrapped_text[wrap_index] = '\0';
            cairo_show_text(cr, wrapped_text);

            cairo_move_to(cr, 150, y_offset + 15);
            cairo_show_text(cr, products[i] + wrap_index);

            y_offset += 15;
        } else {
            cairo_show_text(cr, products[i]);
        }

        cairo_move_to(cr, 300, y_offset);
        char qty_text[50];
        sprintf(qty_text, "%d", quantities[i]);
        cairo_show_text(cr, qty_text);

        cairo_move_to(cr, 400, y_offset);
        char price_text[50];
        sprintf(price_text, "₹%.2f", prices[i]);
        cairo_show_text(cr, price_text);

        cairo_move_to(cr, 500, y_offset);
        char amount_text[50];
        sprintf(amount_text, "₹%.2f", amounts[i]);
        cairo_show_text(cr, amount_text);

        y_offset += 30;
    }

    cairo_move_to(cr, 50, y_offset);
    cairo_line_to(cr, 550, y_offset);
    cairo_stroke(cr);

    y_offset += 30;
    cairo_move_to(cr, 50, y_offset);
    cairo_show_text(cr, "Total:");

    cairo_move_to(cr, 500, y_offset);
    char total_text[50];
    sprintf(total_text, "₹%.2f", total);
    cairo_show_text(cr, total_text);

    cairo_show_page(cr);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    printf("PDF generated: %s\n", pdf_filename);
}

int main() {
    MYSQL *con = mysql_init(NULL);
    if (con == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return EXIT_FAILURE;
    }

    if (mysql_real_connect(con, "localhost", "root", "sanjay", "bms", 0, NULL, 0) == NULL) {
        error_exit(con);
    }

    char phone_number[15];
    char customer_name[100];
    int customer_id;

    printf("Enter Customer Phone Number: ");
    scanf("%s", phone_number);

    char query[256];
    sprintf(query, "SELECT cid, name FROM customers WHERE phno = '%s'", phone_number);
    if (mysql_query(con, query)) {
        error_exit(con);
    }

    MYSQL_RES *result = mysql_store_result(con);
    if (result == NULL) {
        error_exit(con);
    }

    MYSQL_ROW row;
    if ((row = mysql_fetch_row(result))) {
        customer_id = atoi(row[0]);
        printf("Customer Found: %s\n", row[1]);
        strcpy(customer_name, row[1]);
    } else {
        printf("Customer not found. Enter Customer Name: ");
        scanf("%s", customer_name);

        sprintf(query, "INSERT INTO customers (phno, name) VALUES ('%s', '%s')", phone_number, customer_name);
        if (mysql_query(con, query)) {
            error_exit(con);
        }

        customer_id = mysql_insert_id(con);
        printf("New Customer ID: %d\n", customer_id);
    }
    mysql_free_result(result);

    sprintf(query, "CREATE TABLE IF NOT EXISTS `customer_%d` (bill_number INT AUTO_INCREMENT PRIMARY KEY, purchase_date DATE, purchase_time TIME, total_amount FLOAT)", customer_id);
    if (mysql_query(con, query)) {
        error_exit(con);
    }

    char product_id[6];
    char ids[100][6];
    char products[100][100];
    double prices[100];
    int quantities[100];
    double amounts[100];
    int num_items = 0;
    double total = 0.0;

    while (1) {
        printf("Enter 5-digit Product ID (or 'done' to finish): ");
        scanf("%5s", product_id);
        if (strcmp(product_id, "done") == 0) {
            break;
        }

        sprintf(query, "SELECT name, price FROM products WHERE id = '%s'", product_id);
        if (mysql_query(con, query)) {
            error_exit(con);
        }

        result = mysql_store_result(con);
        if (result == NULL) {
            error_exit(con);
        }

        if ((row = mysql_fetch_row(result))) {
            strcpy(products[num_items], row[0]);
            prices[num_items] = atof(row[1]);

            printf("Enter Quantity for %s: ", products[num_items]);
            scanf("%d", &quantities[num_items]);

            amounts[num_items] = prices[num_items] * quantities[num_items];
            total += amounts[num_items];
            strcpy(ids[num_items], product_id);
            printf("Product Found: %s, Price: %.2f, Quantity: %d\n", products[num_items], prices[num_items], quantities[num_items]);
            num_items++;
        } else {
            printf("Product not found.\n");
        }

        mysql_free_result(result);
    }

    if (num_items > 0) {
        sprintf(query, "INSERT INTO `customer_%d` (purchase_date, purchase_time, total_amount) VALUES (CURDATE(), CURTIME(), %.2f)", customer_id, total);
        if (mysql_query(con, query)) {
            error_exit(con);
        }

        int bill_number = mysql_insert_id(con);
        printf("Bill Number: ID%d%d\n", customer_id, bill_number);

        generate_pdf(ids, products, prices, quantities, amounts, num_items, total, customer_id, bill_number, customer_name);
    }

    mysql_close(con);
    return 0;
}
//gcc -o product_search product_search.c `mysql_config --cflags --libs` -I/usr/include/cairo -lcairo

