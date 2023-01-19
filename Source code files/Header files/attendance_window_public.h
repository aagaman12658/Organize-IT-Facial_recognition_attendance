#ifndef ATTENDANCE_WINDOW_PUBLIC
#define ATTENDANCE_WINDOW_PUBLIC
#include<gtk-4.0/gtk/gtk.h>

void activate_attendance_window_for_taking_attendance(char* email, char** unique_id, char* classname,int number_of_classroom_created_by_user);
static void toggle_toggled(GtkWidget* widget, gpointer data);
static void action_clbk_for_reference_photo(GSimpleAction* simple_action, G_GNUC_UNUSED GVariant* parameter, gpointer* data);
static void action_clbk_for_present_photo(GSimpleAction* simple_action, G_GNUC_UNUSED GVariant* parameter, gpointer* data);
int compare_students(const void* a, const void* b);
void on_response_for_present_photo(GtkNativeDialog* native, int response);
char* sort_names_and_send_to_db(char* filepath_for_txt_containing_name, char* email, char* classname, int number_of_students);

void activate_attendance_window_for_taking_attendance_demo(char* email);

GtkWidget* window5;
#endif
