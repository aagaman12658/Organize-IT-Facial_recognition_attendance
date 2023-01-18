#include<stdio.h>
#include<gtk-4.0/gtk/gtk.h>
#include<glib/gstdio.h>
#include"loginwithgoogle.h"
#include"SIGNUP_Normal.h"
#include"LOGIN_NORMAL.h"
#include"gtk_c_gui.h"
#include<curl/curl.h>
#include<Windows.h>
#include"gtk_dashboard_gui.h"
#include "ErrorMessages.h"
#include <jansson.h>

//*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*
//function declarations/function prototypes
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
static void activate_entry_for_email_from_signup();
void clear_entry();
void clear_entry_for_signupWindow();
char* return_name();
char* return_text();
int return_valueresult();
void under_development();
int check_email_validity();

//*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*_*
//global variable and not to be changed manually
gchar* email; //stores email entered by user
gchar* password; //stores password entered by user
gchar* confirmPassword; //stores confirm password entered by user
gchar* name; //stores name entered by user that is finally stores in firebase
GtkWidget* window; //creates root window
GtkWidget* entry_1, * entry_2, * entry_0, * entry, * entry2,*entry_3;
GtkApplication* app; // creates app 
char* text; // stores is_smtp_valid result sent back by abstract api
int valueresult;// stores is_smtp_valid_result sent back by abstract api


//displays features under development prompt
void under_development() {
    error_message_sent_by_api("Feature Under Development!");
}

//not to be declared with function prototypes as it is used by multiple other attached files and might cause error
static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
    return written;
}

//clears entry of the initial window  once the user is logged-in
void clear_entry() {
    gtk_editable_set_text(GTK_EDITABLE(entry), "");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter email address or phone number");
    gtk_editable_set_text(GTK_EDITABLE(entry2), "");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry2), "Enter password");
   
}


//clears entry of the sign-up window once a new user is signed-up
void clear_entry_for_signupWindow() {
    gtk_editable_set_text(GTK_EDITABLE(entry_1), "");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_1), "Enter email address or phone number");
    gtk_editable_set_text(GTK_EDITABLE(entry_2), "");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_2), "Enter password");
    gtk_editable_set_text(GTK_EDITABLE(entry_3), "");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_3), "Confirm password");
    gtk_editable_set_text(GTK_EDITABLE(entry_0), "");
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_0), "Enter name");

}


//checks for the validity of email using "abstract" api
int check_email_validity() {
    CURL* curl;
    CURLcode res;



    FILE* fp = fopen("email_validity_result.txt", "wb+");
   
    curl_global_init(CURL_GLOBAL_ALL);
    if (!fp)
    {
        printf("Error opening output file\n");
        return 1;
    }


    curl = curl_easy_init();
    if (curl) {
        char uri[1024] = "https://emailvalidation.abstractapi.com/v1/?api_key=69167b1616574e94b56145b9c041862c&email=";
        strcat(uri, email);

        curl_easy_setopt(curl, CURLOPT_URL, uri);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        //write inside file
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        /* Clean up */
        curl_easy_cleanup(curl);
    }
    fclose(fp);
    fp = fopen("email_validity_result.txt", "rb");
    //added to check whether CURLOPT_WRITEFUNCTION was working properly
    /*ShellExecute(NULL, "open", "email_validity_result.txt", NULL, NULL, SW_SHOWNORMAL);*/
    json_error_t error;
    json_t* root;

    // Read the JSON data from the file
    root = json_load_file("email_validity_result.txt", 0, &error);
    if (!root) {
        fprintf(stderr, "Error: on line %d: %s\n", error.line, error.text);
        return EXIT_FAILURE;
    }

    json_t* is_smtp_valid = json_object_get(root, "is_smtp_valid");
    valueresult = json_is_true(json_object_get(is_smtp_valid, "value"));
    text = json_string_value(json_object_get(is_smtp_valid, "text"));

    printf("\n\n\n\n\ntext-from-root:%s\n\n\n\n\n",text);
    json_decref(root);
    return EXIT_SUCCESS;

}
// displays invalid input message when the user enters any invalid data 
void invalid_input() {
    MessageBeep(MB_OK);
    GtkWidget* window2;
    GtkWidget* title;
    GtkWidget* grid;
    PangoFontDescription* font_desc;
    GdkRGBA rgba;
    GtkTextTag* tag;
    GtkCssProvider* provider;
    GtkStyleContext* context;
    GtkWidget* button;

    window2 = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window2), "Error:");
    gtk_window_set_default_size(GTK_WINDOW(window2), 300, 100);
    css_for_window(window2);
    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window2), grid);
    gtk_window_set_modal(window2, true);
    title = gtk_label_new("please enter valid email and password");
    gtk_grid_attach(GTK_GRID(grid), title, 0, 0, 10, 5);
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
    title = gtk_label_new("ps: dont forget to press \"Enter\"\"");
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
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window2);
    gtk_window_set_transient_for(window2,window);

    gtk_widget_show(window2);
   

}

//returns password to the files accessing it
char* return_password() {
    return password;
 }


//returns emails to the files accessing it
char* return_email() {
    return email;
 }

//returns name to the files accessing it
char* return_name() {
    return name;
}

//returns valueresult to the files accessing it
//valueresult is the variable used for checking the validity of email
int return_valueresult() {
    return valueresult;
}

//also used to check the validity of mail just as valueresult
char* return_text() {
    return text;
}

//call signup_normal() function which is used for signing up new user once confirm password and password match with each other
// once this function successfully calls sign_up() normal window, the function in SIGNUP_NORMAL.c is called
void call_to_signup_normal() {
    if (strcmp(password, confirmPassword) != 0) {
        clear_entry_for_signupWindow();
        passwords_do_not_match();
    }
    else {
        signup_normal();
        clear_entry_for_signupWindow();
    }
}

//creates a signup query window where user is prompted to enter name, email, password, and comfirm the password
void signup_normal_window() {
    GtkWidget* signup_window;
    GtkWidget * entry_4;
    GtkWidget* grid;
    GtkWidget* button;
    GtkWidget* label;

    signup_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(signup_window), "Sign Up");
    gtk_window_set_default_size(GTK_WINDOW(signup_window), 600, 300);
    gtk_window_set_transient_for(signup_window,window);
    gtk_window_set_modal(signup_window, true);
    css_for_window(signup_window);
    
    grid = gtk_grid_new();

    gtk_window_set_child(GTK_WINDOW(signup_window), grid);

    entry_0 = gtk_entry_new();
    entry_1 = gtk_entry_new();
    entry_2 = gtk_entry_new();
    entry_3 = gtk_entry_new();
    entry_4 = gtk_entry_new();


    label = gtk_label_new("Name:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 2, 1);
    css_for_label(label);

    
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_0), "Enter name");
    gtk_grid_attach(GTK_GRID(grid), entry_0, 1, 2, 8, 3);
    gtk_widget_set_size_request(entry_0,8,3);
   

    label = gtk_label_new("Email:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 8, 2, 1);
    css_for_label(label);

    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_1), "Enter email");
    gtk_grid_attach(GTK_GRID(grid), entry_1, 1, 11, 8, 3);

    label = gtk_label_new("Password:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 19, 2, 1);
    css_for_label(label);

    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_2), "Enter password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_2), FALSE);
    gtk_grid_attach(GTK_GRID(grid), entry_2, 1, 22, 8, 3);

    label = gtk_label_new("Confirm Password:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 31, 2, 1);
    css_for_label(label);

    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_3), "Confirm password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_3), FALSE);
    gtk_grid_attach(GTK_GRID(grid), entry_3, 1, 34, 8, 3);

    button = gtk_button_new_with_label("Ok");
    gtk_grid_attach(GTK_GRID(grid), button, 2, 43, 1, 1);

    g_signal_connect(button, "clicked", G_CALLBACK(activate_entry_for_email_from_signup), NULL);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(activate_entry_for_password_from_signup), NULL);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(activate_entry_for_name), entry_0);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(check_email_validity), NULL);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(call_to_signup_normal), NULL);
    gtk_widget_show(signup_window);
}

//used to change the focus from one widget to another
void  change_focus_func(GtkWidget* Widget1, GtkWidget* Widget2) {
    gtk_widget_grab_focus(Widget2);
}

//activates entry for email that is entered in login window
static void activate_entry_for_email() {
    
    email = (char*)gtk_editable_get_text(GTK_EDITABLE(entry));
}

//activates entry for email that is entered in signup window
static void activate_entry_for_email_from_signup() {

    email = (char*)gtk_editable_get_text(GTK_EDITABLE(entry_1));
}

//activates entry for password that is entered in login window
static void activate_entry_for_password() {
    password = (char*)gtk_editable_get_text(GTK_EDITABLE(entry2));
}

//activates entry for password and confirm password that is entered in sign_up window
static void activate_entry_for_password_from_signup() {
    password = (char*)gtk_editable_get_text(GTK_EDITABLE(entry_2));
    confirmPassword = (char*)gtk_editable_get_text(GTK_EDITABLE(entry_3));
}

//activates entry for name that is entered in signup window
static void activate_entry_for_name(){
    name = (char*)gtk_editable_get_text(GTK_EDITABLE(entry_0));

}





// For people who will be using this code to create any further functionalities for this softwar
//copy this and use to crate new child windows when necessary and modity this code as necessary

//static void activate_child_window( gpointer user_data) {
//    GtkWidget* window2;
//    GtkWidget* view;
//    GtkTextBuffer* buffer;
//    GtkTextIter start, end;
//    PangoFontDescription* font_desc;
//    GdkRGBA rgba;
//    GtkTextTag* tag;
//    GtkCssProvider* provider;
//    GtkStyleContext* context;
//
//    window2 = gtk_window_new();
//    gtk_window_set_title(GTK_WINDOW(window2), "Browse_files");
//    gtk_window_set_default_size(GTK_WINDOW(window2), 968, 720);
//
//    view = gtk_text_view_new();
//
//
//    gtk_window_set_child(GTK_WINDOW(window2), view);
//    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
//
//    gtk_text_buffer_set_text(buffer, "Hello, this is some text", -1);
//
//    /* Change default font and color throughout the widget */
//    provider = gtk_css_provider_new();
//    gtk_css_provider_load_from_data(provider,
//        "textview {"
//        " font: 10px consolas;"
//        "  color: green;"
//        "}",
//        -1);
//    context = gtk_widget_get_style_context(view);
//    gtk_style_context_add_provider(context,
//        GTK_STYLE_PROVIDER(provider),
//        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
//
//    /* Change left margin throughout the widget */
//    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(view), 100);
//
//    /* Use a tag to change the color for just one part of the widget */
//    tag = gtk_text_buffer_create_tag(buffer, "blue_foreground",
//        "foreground", "blue",
//        NULL);
//    gtk_text_buffer_get_iter_at_offset(buffer, &start, 7);
//    gtk_text_buffer_get_iter_at_offset(buffer, &end, 12);
//    gtk_text_buffer_apply_tag(buffer, tag, &start, &end);
//
//    gtk_widget_show(window2);
//
//}

//sets apperance properties for the text labels present in buttons
int css_for_buttons(GtkWidget* button) {
    PangoFontDescription* font_desc;
    GdkRGBA rgba;
    GtkTextTag* tag;
    GtkCssProvider* provider;
    GtkStyleContext* context;


    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "button {"
        " background-color:blue;"
        "  font: 20px consolas;"
        "   color: black;"
        "    opacity: 100;"
        "}",
        -1);
    context = gtk_widget_get_style_context(button);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    return 0;
}

//sets apperance properties for the text labels present in window
int css_for_label(GtkWidget* label) {
    PangoFontDescription* font_desc;
    GdkRGBA rgba;
    GtkTextTag* tag;
    GtkCssProvider* provider;
    GtkStyleContext* context;


    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "label {"
        "  font: 20px consolas;"
        "   font-weight: bold;"
        "    color: black;"
        "     opacity: 100;"
        "}",
        -1);
    context = gtk_widget_get_style_context(label);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    return 0;
}

//sets appearance properties for the text labels present in titlebar  and sets appearance properties for window
int css_for_window(GtkWidget* window)
{
    PangoFontDescription* font_desc;
    GdkRGBA rgba;
    GtkTextTag* tag;
    GtkCssProvider* provider;
    GtkStyleContext* context;

    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "window {"
        " background-color: darker(gray);"
        "  font: 20px consolas;"
        "    color: black;"
        "     opacity: 100;"
        "}",
        -1);
    context = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    return 0;
}


//login window(this window creates the basic appearance and functionlaities of appearance window
static void
activate(GtkApplication* app,
    gpointer        user_data)
{
  
    GtkWidget* grid;
    GtkWidget* button,*button2;
    GtkEntryBuffer* buffer,buffer2;
    GtkTextIter start, end;
    PangoFontDescription* font_desc;
    GdkRGBA rgba;
    GtkTextTag* tag;
    GtkCssProvider* provider;
    GtkStyleContext* context;
    GtkWidget* title;
   
    GtkWidget* image;
    
   
   

    const int window_width = 1920;
    const int window_height =1080;

    /* create a new window, and set its title */
    window = gtk_application_window_new(app);
    
    gtk_window_set_title(GTK_WINDOW(window), "Organize-it!");
    gtk_window_set_default_size(GTK_WINDOW(window), window_width, window_height);
 /*   gtk_window_set_resizable(window, false);*/
    gtk_window_maximize(GTK_WINDOW(window));
    css_for_window(window);
 
   
    /* Here we construct the container that is going pack our buttons */
    grid = gtk_grid_new();
    /* Pack the container in the window */
    gtk_window_set_child(GTK_WINDOW(window), grid);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
   
    
    gtk_grid_set_column_spacing(GTK_GRID(grid), 19);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);

   //-------------------------------------------------------------------------------------------------
   //title section
    title = gtk_label_new_with_mnemonic("Organize-It!");
;


    /* Change default font and color throughout the widget */
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "label {"
        " font: 100px sans;"
        "  color: white;"
        "   font-weight: bold;"
        "}",
        -1);
    context = gtk_widget_get_style_context(title);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);


   
    gtk_grid_attach(GTK_GRID(grid), title, 3, 0, 5, 3);

    //-----------------------------------------------------------------------------------------------
    //image section
    image = gtk_image_new_from_file("D:\\new backup for can you c\\deployment gtk c gui\\gyk_c_gui_experimental_deployment\\images\\Organize-It-logo.png");
    

    gtk_grid_attach(GTK_GRID(grid), image,2,1,7,6);
    
    
    //------------------------------------------------------------------
    //entry section

    entry = gtk_entry_new();
    entry2 = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter email address or phone number");
    g_signal_connect(entry, "activate", G_CALLBACK(activate_entry_for_email), NULL);


    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "entry {"
        " font: 20px consolas;"
        "  color: black;"
        "   opacity: 100;"
        "}",
        -1);
    context = gtk_widget_get_style_context(entry);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_grid_attach(GTK_GRID(grid), entry, 4,6, 3, 1);




    gtk_entry_set_visibility(GTK_ENTRY(entry2), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry2), "Enter password (at least 6 characters) ");
    g_signal_connect(entry2, "activate", G_CALLBACK(activate_entry_for_password), NULL);
    g_signal_connect(entry, "activate", G_CALLBACK(change_focus_func), entry2);
    button = gtk_button_new_with_label("Login");
    g_signal_connect_swapped(entry2, "activate", G_CALLBACK(change_focus_func), button);

    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "entry {"
        " font: 20px consolas;"
        "  color: black;"
        "   opacity: 100;"
        "}",
        -1);
    context = gtk_widget_get_style_context(entry);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_grid_attach(GTK_GRID(grid), entry2, 4, 9, 3,1);
    //------------------------------------------------------------------------------------------------
    //button section
  
    g_signal_connect(button, "clicked", G_CALLBACK(activate_entry_for_email), NULL);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(activate_entry_for_password), NULL);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(login_normal), NULL);
   

    /* Place the first button in the grid cell (0, 0), and make it fill
     * just 1 cell horizontally and vertically (ie no spanning)
     */
    gtk_grid_attach(GTK_GRID(grid), button, 4, 11, 1, 1);

    css_for_buttons(button);

    button = gtk_button_new_with_label("Forgot Password?");
    g_signal_connect(button, "clicked", G_CALLBACK(under_development), NULL);

    /* Place the first button in the grid cell (0, 0), and make it fill
     * just 1 cell horizontally and vertically (ie no spanning)
     */
    gtk_grid_attach(GTK_GRID(grid), button, 5, 11, 1, 1);

    css_for_buttons(button);


    button2 = gtk_button_new_with_label("SignUp");
    g_signal_connect_swapped(button2, "clicked", G_CALLBACK(signup_normal_window), NULL);

    /* Place the first button in the grid cell (0, 0), and make it fill
     * just 1 cell horizontally and vertically (ie no spanning)
     */
    gtk_grid_attach(GTK_GRID(grid), button2, 6,11, 1, 1 );

    css_for_buttons(button2);

    button = gtk_button_new_with_label("SignUp With Phone Number");
    g_signal_connect(button, "clicked", G_CALLBACK(under_development), NULL);

    /* Place the second button in the grid cell (1, 0), and make it fill
     * just 1 cell horizontally and vertically (ie no spanning)
     */
    gtk_grid_attach(GTK_GRID(grid), button,  4, 12, 3, 1);

    css_for_buttons(button);

    button = gtk_button_new_with_label("SignIn With Google");
    g_signal_connect(button, "clicked", G_CALLBACK(waiting_window_for_login_with_google), NULL);

    
    /* Place the second button in the grid cell (1, 0), and make it fill
     * just 1 cell horizontally and vertically (ie no spanning)
     */
  
    gtk_grid_attach(GTK_GRID(grid), button, 4,13, 3, 1);

    css_for_buttons(button);

    button = gtk_button_new_with_label("SignIn With Facebook");
    g_signal_connect(button, "clicked", G_CALLBACK(under_development), NULL);

    /* Place the second button in the grid cell (1, 0), and make it fill
     * just 1 cell horizontally and vertically (ie no spanning)
     */
    gtk_grid_attach(GTK_GRID(grid), button, 4, 14, 3, 1);

    css_for_buttons(button);

    button = gtk_button_new_with_label("Quit");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window);

    /* Place the Quit button in the grid cell (0, 1), and make it
     * span 2 columns.
     */
    gtk_grid_attach(GTK_GRID(grid), button, 5, 15, 1, 1);

    css_for_buttons(button);

   

    gtk_widget_show(window);
}



// Entry point of the program 
int
main(int    argc,
    char** argv)
{

    int status;

    app = gtk_application_new("org.aagaman.attendance", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}