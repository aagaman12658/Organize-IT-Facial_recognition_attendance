#include<stdio.h>
#include<curl/curl.h>
#include"LOGIN_NORMAL.h"
#include"gtk-4.0/gtk/gtk.h"
#include"gtk_c_gui.h"
#include<jansson.h>
#include"gtk_dashboard_gui.h"
#include"ErrorMessages.h"





//function prototypes
int login_normal();
static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream);


//global variables not to be changed
gchar* name;


//returns displayName()
char* return_displayName() {
    printf("\n\n\n\n\n\ndisplay name from return function (iam here)= %s\n\n\n\n\n\n\n", name);
    return name;
}

//writes the response sent by api to a file
static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
    return written;
}




//logs in the user in the database
int login_normal()
{
   
    gchar* email;
    gchar* password;
    CURL* curl;
    CURLcode res;
    FILE* fp = fopen("outfile.txt", "wb+");
    char* response = (char*)"false";
    curl_global_init(CURL_GLOBAL_ALL);
    if (!fp)
    {
        printf("Error opening output file\n");
        return 1;
    }

 
    email = (char*)return_email();
    password = (char*)return_password();

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = NULL;

        // Set the request URL and HTTP method
        curl_easy_setopt(curl, CURLOPT_URL, "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=AIzaSyArZnvmEMGaha4VM23QO5VwCoHEJyBxjIw");
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        // Set the request body
        char request_body[1024] = "{\"email\":\"";
        strcat(request_body, email);
        strcat(request_body, "\",\"password\":\"");
        strcat(request_body, password);
        strcat(request_body, "\",\"returnSecureToken\":true}");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body);

        // Set the Content-Type header
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        
        //write inside file
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    fclose(fp);
    fp = fopen("outfile.txt", "rb");
    //added to check whether CURLOPT_WRITEFUNCTION was working properly
    /*ShellExecute(NULL, "open", "outfile.txt", NULL, NULL, SW_SHOWNORMAL);*/
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
    root = json_load_file("outfile.txt", 0, &error);

    // Extract the values of the fields
    const char* kind = json_string_value(json_object_get(root, "kind"));
    const char* localId = json_string_value(json_object_get(root, "localId"));
    const char* email1 = json_string_value(json_object_get(root, "email"));
    name = json_string_value(json_object_get(root, "displayName"));
    const char* idToken = json_string_value(json_object_get(root, "idToken"));
    int registered = json_is_true(json_object_get(root, "registered"));
    const char* refreshToken = json_string_value(json_object_get(root, "refreshToken"));
    const char* expiresIn = json_string_value(json_object_get(root, "expiresIn"));
    json_t* error_in_process = json_object_get(root, "error");
    const char* message = json_string_value(json_object_get(error_in_process, "message"));
    printf("\n\n\n\ndisplayName = %s\n\n\n\n\n", name);
    // 1 means true
    if (registered == 1) {
        response = (char*)"true";
        clear_entry();
        activate_dashboard_window(email1);

    }
    else if(message!=NULL){
        error_message_sent_by_api(message);
    }
    else {
        error_message_sent_by_api("No internet!");
    }


    // Print the values
    printf("kind: %s\n", kind);
    printf("localId: %s\n", localId);
    printf("email: %s\n", email1);
    printf("displayName: %s\n",name);
    printf("idToken: %s\n", idToken);
    printf("registered: %s\n", response);
    printf("registered: %i\n", registered);
    printf("refreshToken: %s\n", refreshToken);
    printf("expiresIn: %s\n", expiresIn);

    // Free the memory used by the json_t object
    json_decref(root);
    fclose(fp);
    return 0;
}



