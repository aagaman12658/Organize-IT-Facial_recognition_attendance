#ifndef GTK_C_GUI
#define GTK_C_GUI
#include<gtk-4.0/gtk/gtk.h>

//function declarations
static void activate(GtkApplication* app, gpointer user_data);
int css_for_window(GtkWidget* window);
int css_for_buttons(GtkWidget* button);
static void activate_entry_for_password();
static void activate_entry_for_email();
void  change_focus_func(GtkWidget* Widget1, GtkWidget* Widget2);
char* return_email();
char* return_password();
void invalid_input();
char* return_password();
char* return_email();
int css_for_label(GtkWidget* label);
static void activate_entry_for_name();
static void activate_entry_for_password_from_signup();
void clear_entry();
static void activate_entry_for_email_from_signup();
void clear_entry_for_signupWindow();
char* return_name();
char* return_text();
int return_valueresult();


//global variable and not to be changed manually
gchar* email;
gchar* password;
GtkWidget* window;
GtkApplication* app;

#endif
