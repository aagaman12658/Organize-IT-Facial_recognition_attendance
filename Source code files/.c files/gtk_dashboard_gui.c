/* ========================================================================= */
/**
 * @file gtk_dashboard_gui.c
 * @author Organize-IT!
 * @date 2023
 */
 /* ========================================================================= */

/** @defgroup gtk_dashboard_gui gtk_dashboard_gui.c
 * This is the dashboard window where users will be able to acess different functionalities of the program.
 * @{
 */


/* ========================================================================= */
/* Include files section                                                     */
/* ========================================================================= */

//c libraries
#include<gtk-4.0/gtk/gtk.h>
#include<curl/curl.h>
#include<stdio.h>
#include<glib/gstdio.h>
#include <jansson.h>

//user created headers
#include "gtk_c_gui.h"
#include "gtk_dashboard_gui.h"
#include "attendance_window_public.h"
#include "ErrorMessages.h"
#include "LOGIN_NORMAL.h"
#include "post_basic_file_to_database.h"


/* ========================================================================= */
/* Fucntion prototypes section                                               */
/* ========================================================================= */
//function declaration/function prototypes
void activate_dashboard_window(char* email_sent_by_main_file);
void activate_profile_window();
static void activate_attendance_window();
void open_attendance_window();
void activate_query_window();
static void action_clbk_for_txt_file(GSimpleAction* simple_action, G_GNUC_UNUSED GVariant* parameter, gpointer* data);
void on_response_for_txt_file(GtkNativeDialog* native, int response);
void open_parent3();
void open_parent2();
void open_parent1(); 
void on_clicking_ok_for_create_classroom_query();
void save_basic_details();
void on_clicking_create_classroom();
void open_actual_attendance_window(GtkWidget* button);
int change_uri_to_localpath_for_txt_file(gchar* file_uri);
void add_metadata_to_main_json(json_t* root);
static void activate_entry_for_no_of_students();
int convert_to_int();
static void activate_entry_for_classname();


/* ========================================================================= */
/* Global variables section                                                  */
/* ========================================================================= */


//global variables for dashboard
GtkWidget* window2; //dashboard window
GtkWidget* window3; //attendance window
GtkWidget* window4; //query window
GtkWidget* window6; //profile window
GtkWidget* grid_for_attendance_window; //grid for the window where classroom created by user is present
gchar* classname= (gchar*)"";
GtkWidget* entry,*entry2;
char* no_of_students=(char*)"";
const char displayNameForProfile[50];
int initial_no_of_classes = 0;
int class_status = 0;
int number_of_students;
int number_of_classroom_created_by_user = 0;
int depth_of_new_classroom = 4;
char namep[50];
FILE* file_for_txt_file;
gchar* uri_for_txt_file = (gchar*)"";
GtkWidget* toggle;//toggle for query window
char user_email[50];
char** return_response;
char** unique_id;
char email_to_be_sent_back_to_db[50];


//This function is called by activate_query_window() function to take classname as input
static void activate_entry_for_classname() {
    classname = (gchar*)gtk_editable_get_text(GTK_EDITABLE(entry));
}


//converts no of students to integer as it is initially entered as char*
/*! \fn     int convert_to_int()
    \brief  converts no of students to integer as it is initially entered as char*
    \param number_of_students -> saves the number of students in a classroom created by user in integer format
*/
int convert_to_int(){
  
    int number_int; // Number as an integer

    number_int = atoi(no_of_students); // Convert the string to an integer

    printf("The integer value is: %d\n", number_int); // Print the integer value

    number_of_students = number_int;
    return 0;
}

//This function is called by activate_query_window() funcion to take no_of_students as input
/*! \fn     static void activate_entry_for_no_of_students()
    \brief  This function is called by activate_query_window() funcion to take no_of_students as input
    \param no_of_students -> saves the number of students in a classroom created by user in char* format
*/
static void activate_entry_for_no_of_students() {
    no_of_students = (char*)gtk_editable_get_text(GTK_EDITABLE(entry2));
    printf("no of students %s", no_of_students);
}

//This function is just create for test purpose to see whether return_displayName() fucntion is working properly
//verdict: return display_name() is not working properly here as activate_print_window() is not defined in the original file where return_dispalyName() is defined
//return_dispalyName is defined in login_normal.c
void activate_print_window() {
    printf("\n\n\n\n\n\n\n\n\ displayName from button = %s", return_displayName());
}


// This function is created to create a "Json Payload" which will be delivered to firebase realtime database
/*! \fn     void add_metadata_to_main_json(json_t* root)
    \brief  This function is created to create a "Json Payload" which will be delivered to firebase realtime database
*/
void add_metadata_to_main_json(json_t* root) {
    json_object_set_new(root, "Email", json_string(email_to_be_sent_back_to_db));
    json_object_set_new(root, "Number_of_classrooms", json_integer(number_of_classroom_created_by_user));
    while( class_status < number_of_classroom_created_by_user) {
        json_object_set_new(root, g_strdup_printf("classroom_%d", class_status), json_string(return_response));
        class_status++;
    }
    json_object_set_new(root, "current_class_status", json_integer(class_status));
}



// This function is create to convert uri extracted by g_file_get_uri() to local path
// uri looks like this : file:///C:/Users/user/folder/json_draft.txt
//local path looks like this : C:\Users\user\folder\json_draft.txt 
// This function is called by : on_response_for_txt_file(GtkNativeDialog* native, int response) function

/*! \fn     void add_metadata_to_main_json(json_t* root)
    \brief   This function is create to convert uri extracted by g_file_get_uri() to local path. Uri looks like this : file:///C:/Users/user/folder/json_draft.txt . Local path looks like this : C:\Users\user\folder\json_draft.txt.
*/
int change_uri_to_localpath_for_txt_file(gchar* file_uri) {
    // Get the file URI


        // Skip the "file://" prefix
    gchar* file_path = file_uri + 8;

    // Replace '/' with '\'
    int length = strlen(file_path);
    for (int i = 0; i < length; i++) {
        if (file_path[i] == '/') {
            file_path[i] = '\\';
        }
    }

    // Print the result
    printf("\n\n\n\n\n\n\n\n\n\nLocal Path : %s\n\n\n\n\n\n\n\n\n\n\n\n\n", file_path);
    uri_for_txt_file = file_path;

    return 0;
}


//opens actual attendance window for taking attendance 

/*! \fn     void open_actual_attendance_window(GtkWidget* button)
    \brief   opens actual attendance window for taking attendance
*/
void open_actual_attendance_window(GtkWidget* button) {
    char* label = gtk_button_get_label(GTK_BUTTON(button));
    printf("\n\n\n\n\n\n\n\n\n\nuser_email= %s\n\n\n\n\n\n\n\n\n\n", user_email);
    activate_attendance_window_for_taking_attendance(user_email,unique_id, label, number_of_classroom_created_by_user);
}


//creates new buttons for new classroom when user creates new classroom
//also keeps track of the no of classroom created by user and creates the classroom whenever the user logs into the account

/*! \fn    void on_clicking_create_classroom()
    \brief   Creates new buttons for new classroom when user creates new classroom. Also keeps track of the no of classroom created by user and creates the classroom whenever the user logs into the account
*/
void on_clicking_create_classroom() {
     
     
        depth_of_new_classroom += 1;
        GtkWidget* button_for_new_classroom = gtk_button_new_with_label(classname);
        gtk_grid_attach(GTK_GRID(grid_for_attendance_window), button_for_new_classroom,0,depth_of_new_classroom,1,1);
        g_signal_handlers_disconnect_by_func(button_for_new_classroom, G_CALLBACK(open_actual_attendance_window), button_for_new_classroom);
        g_signal_connect(button_for_new_classroom, "clicked", G_CALLBACK(open_actual_attendance_window), button_for_new_classroom);
 
    }


//saves basic details of the user to realtime database provided by firebase
/*! \fn    void save_basic_details()
    \brief  saves basic details of the user to realtime database provided by firebase. Basic details include: user email, no of classrooms created, current class status, unique_id of classroom if any classroom is created.
*/
void save_basic_details() {
    json_t* root = json_object();
    add_metadata_to_main_json(root);
    char* main_json = json_dumps(root, JSON_COMPACT);
    printf("JsonPayload:%s", main_json);
    post_basic_file_to_database(main_json,user_email);
    json_decref(root);
}

//describes the behavior of program on clicking create classroom query
/*! \fn    void on_clicking_ok_for_create_classroom_query()
    \brief Describes the behavior of program on clicking create classroom query.
*/
void on_clicking_ok_for_create_classroom_query() {
    
    GtkWidget* window6;
    GtkWidget* title;
    GtkWidget* grid;
    PangoFontDescription* font_desc;
    GdkRGBA rgba;
    GtkTextTag* tag;
    GtkCssProvider* provider;
    GtkStyleContext* context;
    GtkWidget* button;
    if(strcmp(classname,"")==0||strcmp(no_of_students,"")==0||strcmp(uri_for_txt_file,"")==0)
    {
        MessageBeep(MB_OK);
        window6 = gtk_window_new();
        gtk_window_set_title(GTK_WINDOW(window6), "Error:");
        gtk_window_set_default_size(GTK_WINDOW(window6), 300, 100);
        css_for_window(window6);
        grid = gtk_grid_new();
        gtk_window_set_child(GTK_WINDOW(window6), grid);
        gtk_window_set_modal(window6, true);
        title = gtk_label_new("Please Enter Details!!");
        gtk_grid_attach(GTK_GRID(grid), title, 0, 20, 10, 5);
        provider = gtk_css_provider_new();
        gtk_css_provider_load_from_data(provider,
            "label {"
            " font: 30px sans;"
            "  color: black;"
            "   font-weight: bold;"
            "}",
            -1);
        context = gtk_widget_get_style_context(title);
        gtk_style_context_add_provider(context,
            GTK_STYLE_PROVIDER(provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

        button = gtk_button_new_with_label("ok");
        gtk_grid_attach(GTK_GRID(grid), button, 4, 5, 1, 1);
        g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window6);
        gtk_window_set_transient_for(window6, window4);

        gtk_widget_show(window6);
    }
    else
    {
        printf("Hello");
        on_clicking_create_classroom();
        convert_to_int();
        printf("Hello");
        //sort_names_and_send_to_db(char* filepath_for_txt_containing_name, char* email, char* classname, char* return_response);
        return_response = sort_names_and_send_to_db(uri_for_txt_file,user_email,classname,number_of_students);
        number_of_classroom_created_by_user++;
        save_basic_details();
        unique_id = get_data_present_in_main_json_in_database(user_email, &number_of_classroom_created_by_user, &class_status);
        gtk_window_destroy(window4);
    }

}


//opens parent window when window2 is closed
/*! \fn    void open_parent1()
    \brief opens parent window when window2 is closed
*/
void open_parent1() {
    gtk_widget_set_visible(window, true);
}

//opens parent window2 when window3 is closed
/*! \fn    void open_parent2()
    \brief opens parent window2 when window3 is closed
*/
void open_parent2() {
    gtk_widget_set_visible(window2, true);
}

//opens parent window3 when child window is closed
/*! \fn    void open_parent3()
    \brief opens parent window3 when child is closed
*/
void open_parent3() {
    gtk_widget_set_visible(window3, true);
}


//For testing purpose
/*! \fn    static void action_clbk(GSimpleAction* simple_action, G_GNUC_UNUSED GVariant* parameter, G_GNUC_UNUSED gpointer* data)
    \brief For testing button press event
*/
static void action_clbk(GSimpleAction* simple_action, G_GNUC_UNUSED GVariant* parameter, G_GNUC_UNUSED gpointer* data)
{
    g_print("The action %s was clicked.\n", g_action_get_name(G_ACTION(simple_action)));
}


//is executed when dilog box is opened for taking the txt file input
/*! \fn   void on_response_for_txt_file(GtkNativeDialog* native, int response)
    \brief This function is executed when dialog box is opened for taking the txt file input. This function takes the uri of text file from user and passes it to another function to turn it into local path.
*/
void on_response_for_txt_file(GtkNativeDialog* native, int response) {
    if (response == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(native);
        file_for_txt_file = gtk_file_chooser_get_file(chooser);
        char* filename = g_file_get_basename(file_for_txt_file);
        uri_for_txt_file = g_file_get_uri(file_for_txt_file);
        g_print("\n\n\n\n\n\nThe file path of  choosen one is: %s\n\n\n\n\n\n\n\n", uri_for_txt_file);
        change_uri_to_localpath_for_txt_file(uri_for_txt_file);
        g_print("\n\n\n\n\n\nThe filename choosen: %s\n\n\n\n\n\n\n\n", filename);
        g_print("\n\n\n\n\n\nThe file path of  choosen one is: %s\n\n\n\n\n\n\n\n", uri_for_txt_file);
        /*open_file(filename);*/
        /*ShellExecute(NULL, "open", uri_for_txt_file, NULL, NULL, SW_SHOWNORMAL);*/
        if (uri_for_txt_file != NULL)
        {
            gtk_check_button_set_active(GTK_CHECK_BUTTON(toggle), true);
        }

    }

}


//opens dialog box for taking txt file as input
/*! \fn  static void action_clbk_for_txt_file(GSimpleAction* simple_action, G_GNUC_UNUSED GVariant* parameter, gpointer* data)
    \brief Opens dialog box for taking txt file as input.
*/
static void action_clbk_for_txt_file(GSimpleAction* simple_action, G_GNUC_UNUSED GVariant* parameter, gpointer* data)
{
    //for Opening File Chooser Dialog
    GtkFileChooserNative* native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;


    native = gtk_file_chooser_native_new("Open File", GTK_WINDOW(window), action, "_Open", "_Cancel");
    g_signal_connect(native, "response", G_CALLBACK(on_response_for_txt_file), NULL);
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(native));





    g_print("The action %s was clicked.\n", g_action_get_name(G_ACTION(simple_action)));
}


//activates query window for taking details of the classroom to be created
/*! \fn  void activate_query_window()
    \brief Activates query window for taking details of the classroom to be created.
*/
void activate_query_window() {

    GtkWidget* button;
    GtkWidget* grid;
    GtkWidget* label;
    PangoFontDescription* font_desc;
    GdkRGBA rgba;
    GtkTextTag* tag;
    GtkCssProvider* provider;
    GtkStyleContext* context;
   
    
    window4 = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window4), "Details");
    gtk_window_set_default_size(GTK_WINDOW(window4), 700, 200);
    css_for_window(window4);
    gtk_window_set_transient_for(window4, window3);
    gtk_window_set_modal(window4,window3);

    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window4), grid);

    label = gtk_label_new("Class details");
    gtk_grid_attach(GTK_GRID(grid), label, 30, 0 , 5, 20);
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "label{"
        " font: 40px consolas;"
        "  color: black;"
        "   font-weight : bold;"
        "    opacity: 100;"
        "}",
        -1);
    context = gtk_widget_get_style_context(label);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);


    label = gtk_label_new("Classname : ");
    gtk_grid_attach(GTK_GRID(grid), label,0, 25, 20, 20);
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "label{"
        " font: 20px consolas;"
        "  color: white;"
        "   font-weight : bold;"
        "    opacity: 100;"
        "}",
        -1);
    context = gtk_widget_get_style_context(label);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    label = gtk_label_new("No of Students : ");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 43, 20, 20);
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "label{"
        " font: 20px consolas;"
        "  color: white;"
        "   font-weight : bold;"
        "    opacity: 100;"
        "}",
        -1);
    context = gtk_widget_get_style_context(label);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    label = gtk_label_new("Txt file containing students name : ");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 75, 20, 20);
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "label{"
        " font: 20px consolas;"
        "  color: white;"
        "   font-weight : bold;"
        "    opacity: 100;"
        "}",
        -1);
    context = gtk_widget_get_style_context(label);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);


    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 30, 30, 20, 10);
    g_signal_connect(entry, "activate", G_CALLBACK(activate_entry_for_classname), NULL);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter Classname");
    entry2 = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry2, 30, 45, 20, 10);
    g_signal_connect(entry, "activate", G_CALLBACK(change_focus_func), entry2);
    g_signal_connect(entry2, "activate", G_CALLBACK(activate_entry_for_no_of_students), NULL);
   
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry2), "Enter number of students");

    button = gtk_button_new_with_label("Add file");
    gtk_grid_attach(GTK_GRID(grid), button, 30, 81, 1, 1);
    g_signal_connect(button, "clicked", G_CALLBACK(action_clbk_for_txt_file), NULL);
    
    toggle = gtk_check_button_new();

    gtk_grid_attach(GTK_GRID(grid), toggle, 32, 81, 1, 1);
    button = gtk_button_new_with_label("Ok");
    
    g_signal_connect(entry2, "activate", G_CALLBACK(change_focus_func), button);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(activate_entry_for_classname), NULL);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(activate_entry_for_no_of_students), NULL);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(on_clicking_ok_for_create_classroom_query), NULL);
   
    gtk_grid_attach(GTK_GRID(grid), button, 30, 100, 1, 1);
    css_for_buttons(button);
    

    gtk_widget_show(window4);
}

//opens demo attendance window
/*! \fn  void open_attendance_window() 
    \brief Opens demo attendance window.
*/
void open_attendance_window() {
    activate_attendance_window_for_taking_attendance_demo(user_email);
}

//activates attendance window where the classrooms created by the user are present 
/*! \fn  static void activate_attendance_window( )
    \brief activates attendance window where the classrooms created by the user are present.
*/
static void activate_attendance_window( ) {
   
    GtkWidget* scrolledwindow;
    GtkCssProvider* provider;
    GtkStyleContext* context;
    GtkWidget* button;
    
    GtkWidget* label;
    gtk_widget_set_visible(window2, false);
  
    
    window3 = gtk_application_window_new(GTK_APPLICATION(app));
    scrolledwindow = gtk_scrolled_window_new();
    gtk_window_set_child(GTK_WINDOW(window3), scrolledwindow);
    gtk_window_set_title(GTK_WINDOW(window3), "Classrooms");
    gtk_window_set_default_size(GTK_WINDOW(window3), 1920, 1080);
    gtk_window_maximize(GTK_WINDOW(window3));
    css_for_window(window3);
    grid_for_attendance_window = gtk_grid_new();

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledwindow), grid_for_attendance_window);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid_for_attendance_window), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid_for_attendance_window), TRUE);

    label = gtk_label_new("Your Classrooms");
    /* Change default font and color throughout the widget */
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "label {"
        " font: 50px sans;"
        "  color: white;"
        "   font-weight: bold;"
        "}",
        -1);
    context = gtk_widget_get_style_context(label);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);



    gtk_grid_attach(GTK_GRID(grid_for_attendance_window), label, 0, 1, 1, 1);

 
    //-----------------------------------------------------------------------------------------------------------------
    //buttons section
    //determines whether user has created any classroom or not
    button = gtk_button_new_with_mnemonic("Create Classroom");
    gtk_grid_attach(GTK_GRID(grid_for_attendance_window), button, 0, 3, 1, 1);
    //gtk_widget_set_size_request(button, 5, 2);
    g_signal_connect(button, "clicked", G_CALLBACK(activate_query_window), NULL);
    
    button = gtk_button_new_with_label("Demo Class");
    gtk_grid_attach(GTK_GRID(grid_for_attendance_window), button, 0, 2, 1, 1);
    //gtk_widget_set_size_request(button, 5, 2);
    g_signal_connect(button, "clicked", G_CALLBACK(open_attendance_window), NULL);
    
    for (initial_no_of_classes = 0; initial_no_of_classes < number_of_classroom_created_by_user; initial_no_of_classes++) {
   
        classname = get_classname_only_from_resource_json(user_email, unique_id[initial_no_of_classes]);

        on_clicking_create_classroom();
    }
   
    gtk_widget_show(window3);
    g_signal_connect(window3, "destroy", G_CALLBACK(open_parent2), NULL);



}

//activates the profile window   <<<<<---under construction----->>>>>
/*! \fn  void activate_profile_window()
    \brief activates the profile window   <<<<<---under construction----->>>>>
*/
void activate_profile_window() {
    GtkWidget* label;
    GtkWidget* grid;
    window6 = gtk_window_new();
    gtk_window_set_default_size(GTK_WINDOW(window6), 600, 300);
    gtk_window_set_title(GTK_WINDOW(window6), "Profile");
    gtk_window_set_resizable(GTK_WINDOW(window6), false);

    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window6), grid);
    printf("\n\n\n\n\n\n\n\n\ndisplayName from profile window = %s\n\n\n\n\n\n\n\n", displayNameForProfile);
    label = gtk_label_new_with_mnemonic(displayNameForProfile);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 5, 2);
    css_for_label(label);

    gtk_widget_show(window6);

}

//activates dashboard for the user from where user can enter different windows
/*! \fn  void activate_dashboard_window(char* email_sent_by_main_file)
    \brief activates dashboard for the user from where user can enter different windows
*/
void activate_dashboard_window(char* email_sent_by_main_file) {
   

    

    GSimpleAction* act_profile;
    GSimpleAction* act_disconnect;
    GSimpleAction* act_open;

    /// ***
    GMenu* menu_bar;
    GMenu* attendance_menu;
    GMenu* userprofile_menu;
 
    /// ***
    GMenuItem* menu_item_connect;
    GMenuItem* menu_item_disconnect;
    GtkWidget* grid;
    GtkWidget* label;
    GtkWidget* image;
    ///***
    PangoFontDescription* font_desc;
    GdkRGBA rgba;
    GtkTextTag* tag;
    GtkCssProvider* provider;
    GtkStyleContext* context;
    GtkWidget* button;
    strcpy(user_email, email_sent_by_main_file);
    strcpy(email_to_be_sent_back_to_db, email_sent_by_main_file);
    printf("user_email_before_calling_main_fucntion ; %s", user_email);
   
    printf("\n\n\n\n\n\n\n\n\nemail= %s\n\n\n\n\n\n\n\n", user_email);
    get_data_present_in_main_json_in_database(user_email, &number_of_classroom_created_by_user, &class_status);

    printf("\n\n\n\n\n\n\n\n\nuser_email_after_calling_main_fucntion ; %s\n\n\n\n\n\n\n\n\n", email_to_be_sent_back_to_db);
    if(number_of_classroom_created_by_user>0)
    {
        unique_id = get_data_present_in_main_json_in_database(user_email, &number_of_classroom_created_by_user, &class_status);

        
    }
    /*strcpy(displayNameForProfile, name);*/  //name is transferred from login_normal.c or loginwithgoogle.c
    /// *** Menu Bar
    menu_bar = g_menu_new();

    /// *** Network_Menu
    attendance_menu = g_menu_new();
    g_menu_append_submenu(menu_bar, "Attendance", G_MENU_MODEL(attendance_menu));
    

    /// *** Server_Menu
    userprofile_menu = g_menu_new();
 
    g_menu_append_submenu(menu_bar, "Account", G_MENU_MODEL(userprofile_menu));
    
    grid = gtk_grid_new();
    ///*****************************************************************************************************************
    //Title section
    label = gtk_label_new("Welcome!\n\nYou can use different facilities from top left corner");
    /* Change default font and color throughout the widget */
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "label {"
        " font: 50px sans;"
        "  color: white;"
        "   font-weight: bold;"
        "}",
        -1);
    context = gtk_widget_get_style_context(label);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);



    gtk_grid_attach(GTK_GRID(grid), label, 0, 50, 90, 10);

 
    //-----------------------------------------------------------------------------------------------
    //image section
    image = gtk_image_new_from_file("C:\\Users\\aagam\\Downloads\\image.png");


    gtk_grid_attach(GTK_GRID(grid), image, 90, 30, 100, 100);
    ///***********************************************************************************************************************
    //window intialization section
    gtk_widget_set_visible(window,false);
    window2 = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window2), "Dashboard");
    gtk_window_maximize(GTK_WINDOW(window2));
    gtk_window_set_default_size(GTK_WINDOW(window2), 1920, 1080);
    gtk_window_set_child(GTK_WINDOW(window2), grid);
    css_for_window(window2);
   
   
    /// *** Create Connect and Disconnect Actions
    act_profile = g_simple_action_new("profile", NULL);
    act_disconnect = g_simple_action_new("disconnect", NULL);
    act_open = g_simple_action_new("open", NULL);

    /// *** Add them to the ActionMap
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(act_profile));
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(act_disconnect));
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(act_open));

    /// *** Connect them to the activate Signal
    g_signal_connect(act_profile, "activate", G_CALLBACK(activate_profile_window), name);
    g_signal_connect(act_disconnect, "activate", G_CALLBACK(action_clbk), NULL);
    g_signal_connect(act_open, "activate", G_CALLBACK(activate_attendance_window), NULL);

    /// *** Create the Connect Item
    menu_item_connect = g_menu_item_new("Profile", "app.profile");
    g_menu_append_item(userprofile_menu, menu_item_connect);

    /// *** Create the Disconnect Item
    menu_item_disconnect = g_menu_item_new("Change Password", "app.disconnect");
    g_menu_append_item(userprofile_menu, menu_item_disconnect);

    /// *** Create the Disconnect Item
    menu_item_disconnect = g_menu_item_new("Open", "app.open");
    g_menu_append_item(attendance_menu, menu_item_disconnect);

    /// ***
    gtk_application_set_menubar(GTK_APPLICATION(app), G_MENU_MODEL(menu_bar));
    gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(window2), TRUE);

    gtk_window_present(window2);
    /// *** Clean

    g_signal_connect(window2, "destroy", G_CALLBACK(open_parent1), NULL);
    g_signal_connect_swapped(window2, "destroy", G_CALLBACK(save_basic_details), NULL);

    /// *** Clean
    g_object_unref(act_profile);
    g_object_unref(act_disconnect);
    g_object_unref(menu_item_connect);
    g_object_unref(menu_item_disconnect);
    g_object_unref(userprofile_menu);
    g_object_unref(attendance_menu);
    g_object_unref(menu_bar);
}

/** @} */