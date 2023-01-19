/* ========================================================================= */
/**
 * @file loginwithgoogle.c
 * @author Organize-IT!
 * @date 2023
 */
 /* ========================================================================= */

/** @defgroup loginwithgoogle loginwithgoogle.c
 * This file contains featues that allows users to login via the google account already logged in, in their browser
 * @{
 */

/* ========================================================================= */
/* Include files section                                                     */
/* ========================================================================= */
//library headers
#include <windows.h>
#include <gtk-4.0/gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include<curl/curl.h>
#include <jansson.h>
//custom headers created by programmer
#include"gtk_c_gui.h"
#include "loginwithgoogle.h"
#include"gtk_dashboard_gui.h"



/* ========================================================================= */
/* Global variables section                                                  */
/* ========================================================================= */
int choice;
#define BUFSIZE 1024
int j = 1;
LPTSTR lptstr = (LPTSTR)"sample";
GtkWidget* window2;


/* ========================================================================= */
/* Fucntion prototypes section                                               */
/* ========================================================================= */
int login_with_google();
gboolean start_login();
int extract_and_print_code();
int request_for_access_token(char* code);
int change_id_token_with_info(char* idtoken);	
static size_t write_data(void* prt, size_t size, size_t nmemb, void* stream);
void waiting_window_for_login_with_google();
void change_value_for_choice();


/*! \fn    void change_value_for_choice() 
    \brief Changes choice variable to zero which is used to control the infinite loop.
*/

void change_value_for_choice() {
    choice = 0;
}


//initiates waiting window for login_with_google
/*! \fn    void waiting_window_for_login_with_google()
    \brief Initiates waiting window for login_with_google.
*/

void waiting_window_for_login_with_google() {
   
    GtkWidget* title;
    GtkWidget* grid;
    PangoFontDescription* font_desc;
    GdkRGBA rgba;
    GtkTextTag* tag;
    GtkCssProvider* provider;
    GtkStyleContext* context;
    GtkWidget* button;

    window2 = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window2), "Loading....");
    gtk_window_set_default_size(GTK_WINDOW(window2), 300, 100);
    css_for_window(window2);
    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window2), grid);

    title = gtk_label_new("Waiting for authorization........");
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

    button = gtk_button_new_with_label("Stop");
    gtk_grid_attach(GTK_GRID(grid), button, 4, 5, 1, 1);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(change_value_for_choice), NULL);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window2);
    gtk_widget_show(window2);
    start_login();

}

//write response sent by api to file
/*! \fn     static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
    \brief  Writes the response sent by api to a file
*/
static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
    return written;
}


//exchanges id_token with user info

/*! \fn    int change_id_token_with_info(char* idtoken)
    \brief  Exchanges id_token with user info
*/
int change_id_token_with_info(char* idtoken)
{
    CURL* curl;
    CURLcode res;
    FILE* fp = fopen("outfileforfinaldetails.txt", "wb+");
    char* response = (char*)"false";
    curl_global_init(CURL_GLOBAL_ALL);
    if (!fp)
    {
        printf("Error opening output file\n");
        return 1;
    }
    curl = curl_easy_init();
    if (curl) {
        char url[2048] = "https://www.googleapis.com/oauth2/v3/tokeninfo?id_token=";
        strcat(url, idtoken);
        printf("\nurl=%s\n", url);
        curl_easy_setopt(curl, CURLOPT_URL, url);

        
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        //write inside file
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    fclose(fp);
    fp = fopen("outfileforfinaldetails.txt", "rb");
    //added to check whether CURLOPT_WRITEFUNCTION was working properly


   /* ShellExecute(NULL, "open", "outfileforfinaldetails.txt", NULL, NULL, SW_SHOWNORMAL);*/


    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return 1;
    }

    size_t read_size = fread(buffer, 1, file_size, fp);
    if (read_size != file_size) {
        fprintf(stderr, "Error reading file\n");
        return 1;
    }

    buffer[file_size] = '\0';
    json_t* root;
    json_error_t error;
    root = json_load_file("outfileforfinaldetails.txt", 0, &error);
    char* email = json_string_value(json_object_get(root, "email"));
    const char* is_email_verified = json_is_true(json_object_get(root, "email_verified"));
    printf("%i", is_email_verified);
    if (is_email_verified == 0) {
        gtk_window_destroy(window2);
        activate_dashboard_window(email);
    }
    else
    {
        invalid_input();
    }
    return 0;
}


//requeqst for access token 

/*! \fn   int request_for_access_token(char* code)
    \brief Requeqsts rest api endpoint for access token 
*/
int request_for_access_token(char* code) {

    FILE* fp = fopen("outfileforaccesstoken.txt", "wb+");
    char* response = (char*)"false";
    curl_global_init(CURL_GLOBAL_ALL);
    if (!fp)
    {
        printf("Error opening output file\n");
        return 1;
    }

    // Make a request to exchange the authorization code for an access token
    CURL* curl = curl_easy_init();
    CURLcode res;
    if (curl) {
        struct curl_slist* headers = NULL;
        char post_data[2048] = "code=";
        strcat(post_data, code);
        strcat(post_data, "&client_id=873703754816-0rr3citgede0ru83cbqlnl8pinv79rkj.apps.googleusercontent.com&client_secret=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&redirect_uri=http://127.0.0.1:5500&grant_type=authorization_code");
        curl_easy_setopt(curl, CURLOPT_URL, "https://oauth2.googleapis.com/token");
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        //write inside file
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res= curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    fclose(fp);
    fp = fopen("outfileforaccesstoken.txt", "rb");
    //added to check whether CURLOPT_WRITEFUNCTION was working properly


    /*ShellExecute(NULL, "open", "outfileforaccesstoken.txt", NULL, NULL, SW_SHOWNORMAL);*/



    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return 1;
    }

    size_t read_size = fread(buffer, 1, file_size, fp);
    if (read_size != file_size) {
        fprintf(stderr, "Error reading file\n");
        return 1;
    }

    buffer[file_size] = '\0';
    json_t* root;
    json_error_t error;
    root = json_load_file("outfileforaccesstoken.txt", 0, &error);
    const char* idToken = json_string_value(json_object_get(root, "id_token"));
    printf("\nIdToken=%s\n\n",idToken);
    change_id_token_with_info(idToken);
    return 0;
}


//prints code present in url
void print_code(char* code) {
    printf("\n\n\n\n\n\ncode from function = %s\n", code);
}


//extracts and prints codeonly useful for devs to check whether the code is working properly or not
/*! \fn   int extract_and_print_code()
    \brief extracts and prints code, print code statement only useful for devs to check whether the code is working properly or not
*/
int extract_and_print_code(){

 
    char code[100];


    // Find the position of the "state" query parameter in the URL
    char* codePos = strstr(lptstr, "code=");
    if (codePos == NULL) {
        printf("Error: state query parameter not found in URL\n");
        return 1;
    }

    // Extract the "state" query parameter from the URL
    int i = 0;
    codePos += 5;  // Skip the "state=" characters
    while (*codePos != '\0' && *codePos != '&') {
        code[i] = *codePos;
        i++;
        codePos++;
    }
    code[i] = '\0';  // Null-terminate the string

    // Print the extracted "state" query parameter
    printf("\n\n\n\n\n\ncode = %s\n", code);
    request_for_access_token(code);
    return 0;
}

/*! \fn   gboolean start_login()
    \brief starts login_with_google with a certain amount of timeout so that the infinite loop doesnot freeze the program from executing
*/

gboolean start_login() {
    g_idle_add((GSourceFunc)login_with_google, NULL);
    return FALSE;
}



//initiates logging in process using google and tires to copy the url to clipboard
/*! \fn  int login_with_google()
    \brief Initiates logging in process using google and tires to copy the url to clipboard
*/
int login_with_google()
{
    choice = 1;
    ShellExecute(NULL, "open", "https://accounts.google.com/o/oauth2/v2/auth?scope=email%20profile&response_type=code&state=developed_by_aagaman&redirect_uri=http://127.0.0.1:5500&client_id=873703754816-0rr3citgede0ru83cbqlnl8pinv79rkj.apps.googleusercontent.com", NULL, NULL, SW_SHOWNORMAL);
    ///
    OpenClipboard(NULL);
    // Allocate a global memory block to hold the text data.
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 13);
    if (hMem == NULL) {
        // Error allocating memory.
        return 1;
    }

    // Lock the memory block and get a pointer to it.
    char* pData = (char*)GlobalLock(hMem);
    if (pData == NULL) {
        // Error locking memory.
        return 1;
    }

    // Copy the text data into the memory block.
    strcpy(pData, "Hello, world!");
    printf("%s", pData);

    // Unlock the memory block.


    // Set the data on the clipboard.
    if (!SetClipboardData(CF_TEXT, hMem)) {
        // Error setting clipboard data.
        printf("Error setting clipboard data");
        return 1;
    }

    GlobalUnlock(hMem);
    ///

LABEL:

    HGLOBAL hglb;



    
   
        // Check if the clipboard contains text data
        if (!IsClipboardFormatAvailable(CF_TEXT)) {
            printf("Error: Clipboard does not contain text data.\n");
        }

        // Open the clipboard
        if (!OpenClipboard(NULL)) {
            printf("Error: Could not open clipboard.\n");
            return 1;
        }

        // Get a handle to the text data on the clipboard
        hglb = GetClipboardData(CF_TEXT);
        if (hglb != NULL) {
            // Lock the handle and get a pointer to the text data
            lptstr = (LPTSTR)GlobalLock(hglb);
            if (lptstr != NULL) {
                // Print the clipboard data
                printf("Clipboard data: %s\n", lptstr);
                // Unlock the handle and close the clipboard
                GlobalUnlock(hglb);
                CloseClipboard();
            }
        }
        char state[50];
        char* state_ref = "developed_by_aagaman";
        // Find the position of the "state" query parameter in the URL
        char* statePos = strstr(lptstr, "state=");
        char* codepos = strstr(lptstr, "code=");
        if (statePos == NULL && codepos == NULL) {
            printf("Error: state query parameter not found in URL\n");
            while (g_main_context_iteration(NULL, FALSE));
            if (choice == 0) return FALSE;
            goto LABEL;
        }

        // Extract the "state" query parameter from the URL
        int i = 0;
        statePos += 6;  // Skip the "state=" characters
        while (*statePos != '\0' && *statePos != '&') {
            state[i] = *statePos;
            i++;
            statePos++;
        }
        state[i] = '\0';  // Null-terminate the string

        // Print the extracted "state" query parameter
        printf("%s\n", state);
        if (strcmp(state_ref, state) == 0) {
            system("cls");
            printf("url = %s", lptstr);
        }
        extract_and_print_code();
        return 0;
        // If we reach this point, something went wrong
        printf("Error: Could not retrieve clipboard data.\n");
        return 1;
    
    if (choice == 1) return true;
    else return false;

    
}

/** @} */