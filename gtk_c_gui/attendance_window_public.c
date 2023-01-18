#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "gtk_c_gui.h"
#include "gtk_dashboard_gui.h"
#include "attendance_window_public.h"
#include <glib.h>
#include <windows.h>
#include <jansson.h>
#include <curl/curl.h>
#include "post_basic_file_to_database.h"
#include "img_to_base64.h"
#include "ErrorMessages.h"

// Struct to store data for each row

typedef struct {
    char name[100];
    int roll_no;
    gboolean toggle1;
    gboolean toggle2;
    int days_present;
    int class_days;
    GtkTreeIter* iter;
} StudentData;


// Data model for TreeView
enum {
    COL_NAME = 0,
    COL_ROLL_NO,
    COL_TOGGLE1,
    COL_TOGGLE2,
    COL_DAYS_PRESENT,
    COL_CLASS_DAYS,
    NUM_COLS = 6,
};


GtkWidget* tree_view;
GFile* file_for_present_photo, * file_for_reference_photo;
char* uri_for_present_photo;
char* uri_for_reference_photo;
gchar* uris_for_reference_photos[100];
char* unique_id_for_classroom;
char* email_to_pass_around;
int number_of_students;
GtkWidget* window5;

void make_every_one_of_uri_null()
{
    for (int i = 0; i < 100; i++) {
        uris_for_reference_photos[i] = NULL;
    }
}



//facial recognition part
char* face_token1 = (char*)"init";
char* face_token2 = (char*)"init";
int change_uri_to_localpath_for_present_photo(gchar* file_uri) {
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
    uri_for_present_photo = file_path;

    return 0;
}
int change_uri_to_localpath_for_reference_photo(gchar* file_uri) {
    // Get the file URI

   
        // Skip the "file://" prefix
        gchar * file_path = file_uri + 8;

    // Replace '/' with '\'
    int length = strlen(file_path);
    for (int i = 0; i < length; i++) {
        if (file_path[i] == '/') {
            file_path[i] = '\\';
        }
    }

    // Print the result
    printf("\n\n\n\n\n\n\n\n\n\nLocal Path : %s\n\n\n\n\n\n\n\n\n\n\n\n\n", file_path);
    uri_for_reference_photo = file_path;

    return 0;
}

static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
    return written;
}
void facetoken_present_image() {
    CURL* curl;
    CURLcode res;

    FILE* fp = fopen("outfileforfacedetails1.txt", "wb+");
    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);
    if (!fp)
    {
        printf("Error opening output file\n");
    }
    /* get a curl handle */
    curl = curl_easy_init();
    if (curl) {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be an https:// URL if that is what should receive the
           data. */

        curl_easy_setopt(curl, CURLOPT_URL, "https://api-us.faceplusplus.com/facepp/v3/detect");

        curl_mime* multipart = curl_mime_init(curl);
        curl_mimepart* part = curl_mime_addpart(multipart);


        curl_mime_name(part, "api_key");
        curl_mime_data(part, "2abctJm0UK8LHJqXmX3kODf6JW38nALS", CURL_ZERO_TERMINATED);
        part = curl_mime_addpart(multipart);
        curl_mime_name(part, "api_secret");
        curl_mime_data(part, "Fmt-sIPxQCRlkw_5PaGI_hJxxNmLilN9", CURL_ZERO_TERMINATED);
        part = curl_mime_addpart(multipart);
        curl_mime_name(part, "image_file");
        curl_mime_filedata(part, uri_for_present_photo);

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, multipart);


        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        //write inside file
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);




        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        }

        /* always cleanup */
        curl_mime_free(multipart);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    fclose(fp);
    fp = fopen("outfileforfacedetails1.txt", "rb");
    //added to check whether CURLOPT_WRITEFUNCTION was working properly


    ShellExecute(NULL, "open", "outfileforfacedetails1.txt", NULL, NULL, SW_SHOWNORMAL);

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Error allocating memory\n");
    }

    size_t read_size = fread(buffer, 1, file_size, fp);
    if (read_size != file_size) {
        fprintf(stderr, "Error reading file\n");
    }

    buffer[file_size-1] = '\0';
       free(buffer);
    json_t* root;
    json_error_t error;
    root = json_load_file("outfileforfacedetails1.txt", 0, &error);
    json_t* faces_json = json_object_get(root, "faces");
    json_t* face_json = json_array_get(faces_json, 0);
    json_t* face_token_json = json_object_get(face_json, "face_token");
    face_token2 = (char*)json_string_value(face_token_json);
    
    fclose(fp);
}
void facetoken_ref_image() {
    CURL* curl;
    CURLcode res;

    curl_off_t speed_upload, total_time;

    FILE* fp = fopen("outfileforfacedetails.txt", "wb+");

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);
    if (!fp)
    {
        printf("Error opening output file\n");
    }
    /* get a curl handle */
    curl = curl_easy_init();
    if (curl) {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be an https:// URL if that is what should receive the
           data. */

        curl_easy_setopt(curl, CURLOPT_URL, "https://api-us.faceplusplus.com/facepp/v3/detect");

        curl_mime* multipart = curl_mime_init(curl);
        curl_mimepart* part = curl_mime_addpart(multipart);


        curl_mime_name(part, "api_key");
        curl_mime_data(part, "2abctJm0UK8LHJqXmX3kODf6JW38nALS", CURL_ZERO_TERMINATED);
        part = curl_mime_addpart(multipart);
        curl_mime_name(part, "api_secret");
        curl_mime_data(part, "Fmt-sIPxQCRlkw_5PaGI_hJxxNmLilN9", CURL_ZERO_TERMINATED);
        part = curl_mime_addpart(multipart);
        curl_mime_name(part, "image_file");
        curl_mime_filedata(part, uri_for_reference_photo);

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, multipart);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        //write inside file
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);



        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        }

        /* always cleanup */
        curl_mime_free(multipart);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    fclose(fp);
    fp = fopen("outfileforfacedetails.txt", "rb");
    //added to check whether CURLOPT_WRITEFUNCTION was working properly


    ShellExecute(NULL, "open", "outfileforfacedetails.txt", NULL, NULL, SW_SHOWNORMAL);
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Error allocating memory\n");
    }

    size_t read_size = fread(buffer, 1, file_size, fp);
    if (read_size != file_size) {
        fprintf(stderr, "Error reading file\n");
    }

    buffer[file_size-1] = '\0';
    json_t* root;
    json_error_t error;
    root = json_load_file("outfileforfacedetails.txt", 0, &error);
    json_t* faces_json = json_object_get(root, "faces");
    json_t* face_json = json_array_get(faces_json, 0);
    json_t* face_token_json = json_object_get(face_json, "face_token");
    face_token1 = (char*)json_string_value(face_token_json);
    free(buffer);
    fclose(fp);
}
float attendance()
{
    facetoken_ref_image();
    facetoken_present_image();
    printf("\nfacetoken1 = %s\n", face_token1);
    printf("\nfacetoken2 = %s\n", face_token2);

    CURL* curl;
    CURLcode res;
    curl_off_t speed_upload, total_time;
    FILE* fp = fopen("Result.txt", "wb+");

    curl_global_init(CURL_GLOBAL_ALL);
    if (!fp)
    {
        printf("Error opening output file\n");
    }
    /* get a curl handle */
    curl = curl_easy_init();
    if (curl) {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be an https:// URL if that is what should receive the
           data. */

        curl_easy_setopt(curl, CURLOPT_URL, "https://api-us.faceplusplus.com/facepp/v3/compare");

        curl_mime* multipart = curl_mime_init(curl);
        curl_mimepart* part = curl_mime_addpart(multipart);


        curl_mime_name(part, "api_key");
        curl_mime_data(part, "2abctJm0UK8LHJqXmX3kODf6JW38nALS", CURL_ZERO_TERMINATED);
        part = curl_mime_addpart(multipart);
        curl_mime_name(part, "api_secret");
        curl_mime_data(part, "Fmt-sIPxQCRlkw_5PaGI_hJxxNmLilN9", CURL_ZERO_TERMINATED);
        part = curl_mime_addpart(multipart);
        curl_mime_name(part, "face_token1");
        curl_mime_data(part, face_token1, CURL_ZERO_TERMINATED);
        part = curl_mime_addpart(multipart);
        curl_mime_name(part, "face_token2");
        curl_mime_data(part, face_token2, CURL_ZERO_TERMINATED);

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, multipart);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        //write inside file
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);



        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        }

        /* always cleanup */
        curl_mime_free(multipart);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    fclose(fp);
    fp = fopen("Result.txt", "rb");
    //added to check whether CURLOPT_WRITEFUNCTION was working properly


    ShellExecute(NULL, "open", "Result.txt", NULL, NULL, SW_SHOWNORMAL);
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Error allocating memory\n");
    }

    size_t read_size = fread(buffer, 1, file_size, fp);
    if (read_size != file_size) {
        fprintf(stderr, "Error reading file\n");
    }

    buffer[file_size-1] = '\0';
    json_t* root;
    json_error_t error;
    root = json_load_file("Result.txt", 0, &error);
    float confidence = json_real_value(json_object_get(root, "confidence"));
    char* error_message = json_string_value(json_object_get(root, "error_message"));
    if(error_message!=NULL)
    {
        message_sent_to_print("              Important!\
                            Supported Image Format : JPG (JPEG), PNG\
                            Size : between 48 * 48 and 4096 * 4096 (pixels)\
                            File size : no larger than 2MB");


  
    }
    printf("\n\n\n\n\n\n\n\n\n\n\n\nConfidence==%f\n\n\n\n\n\n\n\n\n\n\n\n", confidence);
    free(buffer);
    return confidence;
}

void remove_present_photo() {
    GtkTreeModel* model;
    GtkTreeIter iter;
    StudentData student;
    GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    gtk_tree_selection_get_selected(selection, &model, &iter);
    gtk_tree_model_get(model, &iter, COL_NAME, &student.name, COL_ROLL_NO, &student.roll_no, COL_TOGGLE1, &student.toggle1, COL_TOGGLE2, &student.toggle2, COL_DAYS_PRESENT, &student.days_present, COL_CLASS_DAYS, &student.class_days, -1);
    student.toggle2 = false;
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_ROLL_NO, student.roll_no, COL_TOGGLE1, student.toggle1, COL_TOGGLE2, student.toggle2, COL_DAYS_PRESENT, student.days_present, COL_CLASS_DAYS, student.class_days, -1);

}

void remove_referernce_photo() {
    GtkTreeModel* model;
    GtkTreeIter iter;
    StudentData student;
    GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    gtk_tree_selection_get_selected(selection, &model, &iter);
    gtk_tree_model_get(model, &iter, COL_NAME, &student.name, COL_ROLL_NO, &student.roll_no, COL_TOGGLE1, &student.toggle1, COL_TOGGLE2, &student.toggle2, COL_DAYS_PRESENT, &student.days_present, COL_CLASS_DAYS, &student.class_days, -1);
    student.toggle1 = false;
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_ROLL_NO, student.roll_no, COL_TOGGLE1, student.toggle1, COL_TOGGLE2, student.toggle2, COL_DAYS_PRESENT, student.days_present, COL_CLASS_DAYS, student.class_days, -1);
}
    
void remove_both_photos() {
    GtkTreeModel* model;
    GtkTreeIter iter;
    StudentData student;
    GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    gtk_tree_selection_get_selected(selection, &model, &iter);
    gtk_tree_model_get(model, &iter, COL_NAME, &student.name, COL_ROLL_NO, &student.roll_no, COL_TOGGLE1, &student.toggle1, COL_TOGGLE2, &student.toggle2, COL_DAYS_PRESENT, &student.days_present, COL_CLASS_DAYS, &student.class_days, -1);
    student.toggle1 = false;
    student.toggle2 = false;
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_ROLL_NO, student.roll_no, COL_TOGGLE1, student.toggle1, COL_TOGGLE2, student.toggle2, COL_DAYS_PRESENT, student.days_present, COL_CLASS_DAYS, student.class_days, -1);
}

void
on_response_for_present_photo(GtkNativeDialog* native,
    int              response)
{
    GtkTreeModel* model;
    GtkTreeIter iter;
    StudentData student;
    GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    gtk_tree_selection_get_selected(selection, &model, &iter);
    gtk_tree_model_get(model, &iter, COL_NAME, &student.name, COL_ROLL_NO, &student.roll_no, COL_TOGGLE1, &student.toggle1, COL_TOGGLE2, &student.toggle2, COL_DAYS_PRESENT, &student.days_present, COL_CLASS_DAYS, &student.class_days, -1);
    if (student.toggle2 != true && student.toggle1 == true)
    {
        if (response == GTK_RESPONSE_ACCEPT)
        {
            GtkFileChooser* chooser = GTK_FILE_CHOOSER(native);
            file_for_present_photo = gtk_file_chooser_get_file(chooser);
            char* filename = g_file_get_basename(file_for_present_photo);
            uri_for_present_photo = g_file_get_uri(file_for_present_photo);
            change_uri_to_localpath_for_present_photo(uri_for_present_photo);
            g_print("\n\n\n\n\n\nThe filename choosen: %s\n\n\n\n\n\n\n\n", filename);
            g_print("\n\n\n\n\n\nThe file path of  choosen one is: %s\n\n\n\n\n\n\n\n", uri_for_present_photo);
            /*open_file(filename);*/
            ShellExecute(NULL, "open", uri_for_present_photo, NULL, NULL, SW_SHOWNORMAL);

        }
        // Get selected row in tree view
    // For Updating The toggle and Classes Attended

        if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
            // Get data for selected row
            // Increment days_present value

            /* student.days_present++;*/

            if (file_for_present_photo != NULL)
            {
                student.toggle2 = true;
            }
            if (file_for_present_photo != NULL && file_for_reference_photo != NULL)
            {
                float confidence = attendance();
                if (confidence > 60)
                {
                    student.days_present++;
                }
            }
            printf("\n\n\n\n\n\n\n\n\n\nThe student name is : %s\n\n\n\n\n\n\n\n", student.name);
            // Update data in tree view
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_ROLL_NO, student.roll_no, COL_TOGGLE1, student.toggle1, COL_TOGGLE2, student.toggle2, COL_DAYS_PRESENT, student.days_present, COL_CLASS_DAYS, student.class_days, -1);
        }
        uri_for_present_photo = NULL;
        uri_for_reference_photo = NULL;
        file_for_reference_photo = NULL;
        file_for_present_photo = NULL;
        g_object_unref(file_for_reference_photo);
        g_object_unref(file_for_present_photo);

        g_object_unref(native);
    }
    else if(student.toggle2 == true) {
        message_sent_to_print("Present photo already added!");
    }
    else {
        message_sent_to_print("Add reference photo first");
    }
   
}
void
on_response_for_reference_photo(GtkNativeDialog* native,
    int              response)
{
    GtkTreeModel* model;
    GtkTreeIter iter;
    GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    StudentData student;
    gtk_tree_selection_get_selected(selection, &model, &iter);
    gtk_tree_model_get(model, &iter, COL_NAME, &student.name, COL_ROLL_NO, &student.roll_no, COL_TOGGLE1, &student.toggle1, COL_TOGGLE2, &student.toggle2, COL_DAYS_PRESENT, &student.days_present, COL_CLASS_DAYS, &student.class_days, -1);
    // Increment days_present value
    if (student.toggle1 != true)
    {
        if (response == GTK_RESPONSE_ACCEPT)
        {
            GtkFileChooser* chooser = GTK_FILE_CHOOSER(native);
            file_for_reference_photo = gtk_file_chooser_get_file(chooser);
            char* filename = g_file_get_basename(file_for_reference_photo);
            uri_for_reference_photo = g_file_get_uri(file_for_reference_photo);
            change_uri_to_localpath_for_reference_photo(uri_for_reference_photo);
            g_print("\n\n\n\n\n\nThe filename choosen: %s\n\n\n\n\n\n\n\n", filename);
            g_print("\n\n\n\n\n\nThe file path of  choosen one is: %s\n\n\n\n\n\n\n\n", uri_for_reference_photo);
            /*open_file(filename);*/
            ShellExecute(NULL, "open", uri_for_reference_photo, NULL, NULL, SW_SHOWNORMAL);

        }
        //For Updating Toggle and Total Class Days
       // Get selected row in tree view
        if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
            // Get data for selected row

            if (file_for_reference_photo != NULL)
            {
                student.toggle1 = true;
            }
            printf("\n\n\n\n\n\n\n\n\n\nThe student name is : %s\n\n\n\n\n\n\n\n", student.name);
            // Update data in tree view
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_ROLL_NO, student.roll_no, COL_TOGGLE1, student.toggle1, COL_TOGGLE2, student.toggle2, COL_DAYS_PRESENT, student.days_present, COL_CLASS_DAYS, student.class_days, -1);
        }

        g_object_unref(native);
    }
    else if (student.toggle1 == true) {
        message_sent_to_print("Reference Photo already added");
    }
}

 void
on_response_for_reference_photo_all_at_once(GtkNativeDialog* native,
    int              response)
{
     
     gint length;

     GtkTreeModel* model= gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view));
     GtkTreeIter iter;
     StudentData student;
    // Increment days_present value
 
        if (response == GTK_RESPONSE_ACCEPT)
        {
            GtkFileChooser* chooser = GTK_FILE_CHOOSER(native);
            //file_for_reference_photo = gtk_file_chooser_get_file(chooser);
            //char* filename = g_file_get_basename(file_for_reference_photo);
            //uri_for_reference_photo = g_file_get_uri(file_for_reference_photo);
            //change_uri_to_localpath_for_reference_photo(uri_for_reference_photo);
            //g_print("\n\n\n\n\n\nThe filename choosen: %s\n\n\n\n\n\n\n\n", filename);
            //g_print("\n\n\n\n\n\nThe file path of  choosen one is: %s\n\n\n\n\n\n\n\n", uri_for_reference_photo);
            ///*open_file(filename);*/
            //ShellExecute(NULL, "open", uri_for_reference_photo, NULL, NULL, SW_SHOWNORMAL);
       
            gchar* uri;
            char* base64_data[100];
            int i = 0;
            GListModel* files_list;
            files_list = gtk_file_chooser_get_files(GTK_FILE_CHOOSER(chooser));
            // Declare an array to store the URI of the selected files
          
            length = g_list_model_get_n_items(files_list);

            for (gint i = 0; i < length; i++) {
                GFile* file = G_FILE(g_list_model_get_item(files_list, i));
                uri = g_file_get_uri(file);
                // Do something with the URI
                 // Do something with the URI
                gchar* file_path = uri + 8;

                // Replace '/' with '\'
                int length = strlen(file_path);
                for (int i = 0; i < length; i++) {
                    if (file_path[i] == '/') {
                        file_path[i] = '\\';
                    }
                }
                uri = file_path;
                printf("%s", file_path);
                uris_for_reference_photos[i] = uri;
                g_object_unref(file);
            }

            // Print the URI of the selected files
            for (i = 0; i < length; i++) {
                printf("%s\n", uris_for_reference_photos[i]);
            }

          /*  for (i = 0; i < length; i++) {
                encode_to_base_64(uris_for_reference_photos[i], base64_data[i]);
            }*/

        }
        //For Updating Toggle and Total Class Days
       // Get selected row in tree view
        gtk_tree_model_get_iter_first(model, &iter);
        int i = 0;
        do {
            gtk_tree_model_get(model, &iter, COL_NAME, &student.name, COL_ROLL_NO, &student.roll_no, COL_TOGGLE1, &student.toggle1, COL_TOGGLE2, &student.toggle2, COL_DAYS_PRESENT, &student.days_present, COL_CLASS_DAYS, &student.class_days, -1);
            if (uris_for_reference_photos[i] != NULL)
            {
                student.toggle1 = true;
            }
            i++;
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_ROLL_NO, student.roll_no, COL_TOGGLE1, student.toggle1, COL_TOGGLE2, student.toggle2, COL_DAYS_PRESENT, student.days_present, COL_CLASS_DAYS, student.class_days, -1);
        } while (gtk_tree_model_iter_next(model, &iter));
        g_object_unref(native);
   
}


// Compare function to sort student data by name
int compare_students(const void* a, const void* b) {
    const StudentData* sa = (const StudentData*)a;
    const StudentData* sb = (const StudentData*)b;
    return strcmp(sa->name, sb->name);
}

// Callback function for "Add Photo" menu
static void action_clbk_for_present_photo(GSimpleAction* simple_action, G_GNUC_UNUSED GVariant* parameter, gpointer* data)
{
    //for Opening File Chooser Dialog
    GtkFileChooserNative* native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;


    native = gtk_file_chooser_native_new("Open File", GTK_WINDOW(window), action, "_Open", "_Cancel");
    g_signal_connect(native, "response", G_CALLBACK(on_response_for_present_photo), NULL);
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(native));





    g_print("The action %s was clicked.\n", g_action_get_name(G_ACTION(simple_action)));
}

static void action_clbk_for_reference_photo(GSimpleAction* simple_action, G_GNUC_UNUSED GVariant* parameter, gpointer* data)
{
    //for Opening File Chooser Dialog
    GtkFileChooserNative* native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;


    native = gtk_file_chooser_native_new("Open File", GTK_WINDOW(window), action, "_Open", "_Cancel");
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(native), TRUE);
    g_signal_connect(native, "response", G_CALLBACK(on_response_for_reference_photo), NULL);
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(native));

   
}


void action_clbk_for_reference_photo_all_at_once(GSimpleAction* simple_action, G_GNUC_UNUSED GVariant* parameter, gpointer* data) {
    //for Opening File Chooser Dialog
    /*message_sent_to_print("Please Don't forget to put the image files in Order");*/
    GtkFileChooserNative* native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;


    native = gtk_file_chooser_native_new("Open File", GTK_WINDOW(window), action, "_Open", "_Cancel");
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(native), TRUE);
    g_signal_connect(native, "response", G_CALLBACK(on_response_for_reference_photo_all_at_once), NULL);
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(native));

}
char* sort_names_and_send_to_db(char* filepath_for_txt_containing_name,char* email,char* classname, int number_of_students) {
   
    FILE* file = fopen(filepath_for_txt_containing_name, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filepath_for_txt_containing_name);
    }

    // Count number of students


    StudentData students[100];
    for (int i = 0; i < number_of_students; i++) {
        if (fscanf(file, "%s", students[i].name) != 1) {
            fprintf(stderr, "Error reading file: %s\n", filepath_for_txt_containing_name);
        }
        students[i].toggle1 = FALSE;
        students[i].toggle2 = FALSE;
        students[i].days_present = 0;
        students[i].class_days = 0;

    }
    fclose(file);

    // Sort student data by name
    qsort(students, number_of_students, sizeof(StudentData), compare_students);




    // Create JSON object
    json_t* root = json_object();
    json_t* json_names = json_array();
    json_t* json_images = json_object();
    json_t* json_present_days = json_object();

    for (int i = 0; i < number_of_students; i++) {
        // Add name to names array
        json_array_append_new(json_names, json_string(students[i].name));

    }

    json_object_set_new(root, "classname", json_string(classname));
    json_object_set_new(root, "number_of_students", json_integer(number_of_students));
    json_object_set_new(root, "name", json_names);

    // Dump JSON object to string
    char* json_str = json_dumps(root, JSON_COMPACT);

    // Print JSON object
    printf("JSON object:\n%s\n", json_str);
    char* return_response = post_resource_file_to_database(json_str, email);
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nreturn response ---- = %s\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", return_response);
    // Free memory
    return return_response;
    json_decref(root);
}


void activate_attendance_window_for_taking_attendance_demo(char* email)
{
    make_every_one_of_uri_null();
    GtkWidget* window5, * scrolledwindow;
    GtkWidget* grid;
    PangoFontDescription* font_desc;
    GdkRGBA rgba;
    GtkTextTag* tag;
    GtkCssProvider* provider;
    GtkStyleContext* context;
    GtkWidget* button;
    GSimpleAction* action;

    /// ***
    GMenu* menu_bar;
    GMenu* add_file_menu;
    GMenuItem* menu_item_connect;

    /// ***
    StudentData students[100];
    /// *** Menu Bar
    menu_bar = g_menu_new();

    /// *** Network_Menu
    add_file_menu = g_menu_new();
    g_menu_append_submenu(menu_bar, "Add file", G_MENU_MODEL(add_file_menu));



    window5 = gtk_application_window_new(app);
    gtk_window_maximize(GTK_WINDOW(window5));
    scrolledwindow = gtk_scrolled_window_new();
    gtk_window_set_child(GTK_WINDOW(window5), scrolledwindow);
    grid = gtk_grid_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledwindow), grid);
    gtk_window_set_title(GTK_WINDOW(window5), "IRC Client");
    gtk_window_set_default_size(GTK_WINDOW(window5), 1920, 1080);
    //    // Initialize GTK
   /* gtk_init(&argc, &argv);*/


    /// *** Create Connect and Disconnect Actions
    action = g_simple_action_new("open_reference_photo", NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action));
    g_signal_connect(action, "activate", G_CALLBACK(action_clbk_for_reference_photo), NULL);

    action = g_simple_action_new("open_present_photo", NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action));
    g_signal_connect(action, "activate", G_CALLBACK(action_clbk_for_present_photo), NULL);

    menu_item_connect = g_menu_item_new("Add Reference Photo", "app.open_reference_photo");
    g_menu_append_item(add_file_menu, menu_item_connect);

    menu_item_connect = g_menu_item_new("Add Present Photo", "app.open_present_photo");
    g_menu_append_item(add_file_menu, menu_item_connect);

    gtk_application_set_menubar(GTK_APPLICATION(app), G_MENU_MODEL(menu_bar));
    gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(window5), TRUE);

    tree_view = gtk_tree_view_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledwindow), tree_view);


    // Read student data from file


    // Count number of students
    int num_students = 100;

    
    for (int i = 0; i < num_students; i++) {
        strcpy(students[i].name , g_strdup_printf("Student_%d", i));
        students[i].toggle1 = FALSE;
        students[i].toggle2 = FALSE;
        students[i].days_present = 0;
        students[i].class_days = 0;

    }

    // Sort student data by name
    qsort(students, num_students, sizeof(StudentData), compare_students);
 

    for (int i = 0; i < num_students; i++) {
        students[i].roll_no = i + 1;
    }
    // Create model for TreeView
    GtkListStore* store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_INT, G_TYPE_INT);

    // Add students to model
    for (int i = 0; i < num_students; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            COL_NAME, students[i].name,
            COL_ROLL_NO, students[i].roll_no,
            COL_TOGGLE1, students[i].toggle1,
            COL_TOGGLE2, students[i].toggle2,
            COL_DAYS_PRESENT, students[i].days_present,
            COL_CLASS_DAYS, students[i].class_days,
            -1);
        students[i].iter = &iter;
    }

    // Create columns for TreeView
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", COL_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Roll No.", renderer, "text", COL_ROLL_NO, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_toggle_new();
    column = gtk_tree_view_column_new_with_attributes("Reference Image", renderer, "active", COL_TOGGLE1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_toggle_new();
    column = gtk_tree_view_column_new_with_attributes("Present Image", renderer, "active", COL_TOGGLE2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Classes Attended", renderer, "text", COL_DAYS_PRESENT, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Total Class Days", renderer, "text", COL_CLASS_DAYS, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    // Set model for TreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(store));


    g_object_unref(store);
    gtk_widget_show(window5);
  
}
void update_db_with_updated_data() {
    GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view));
    GtkTreeIter iter;
    StudentData student;
    StudentData students[100];
    json_t* root = json_object();
    json_t* inner = json_object();
    gtk_tree_model_get_iter_first(model, &iter);
    FILE* file = fopen("names.txt", "r");

    for (int i = 0; i < 100; i++) {
        if (fscanf(file, "%s", students[i].name) != 1) {
            fprintf(stderr, "Error reading file\n");
        }
    }
    fclose(file);
    int i = 0;
    do {
        gtk_tree_model_get(model, &iter, COL_NAME, &student.name, COL_ROLL_NO, &student.roll_no, COL_TOGGLE1, &student.toggle1, COL_TOGGLE2, &student.toggle2, COL_DAYS_PRESENT, &student.days_present, COL_CLASS_DAYS, &student.class_days, -1);
        json_object_set_new(inner, students[i].name, json_integer(student.days_present));
        printf("%s\n", students[i].name);
        json_object_set(root, "present_days", inner);
        i++;
    } while (gtk_tree_model_iter_next(model, &iter));
    json_object_set(root, "class_days", json_integer(student.class_days));
    char* resulting_json = json_dumps(root, JSON_INDENT(2));
    printf("%s\n", resulting_json);
    printf("calling you from function: %s\n", unique_id_for_classroom);
    printf("email: %s\n", email_to_pass_around);
    update_resource_file_in_database(resulting_json, email_to_pass_around, unique_id_for_classroom);

    json_decref(root);
}

void increase_class_day_by_1() {


    GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view));
    GtkTreeIter iter;
    StudentData student;
 


    gtk_tree_model_get_iter_first(model, &iter);
    int i = 0;
    do {
        gtk_tree_model_get(model, &iter, COL_NAME, &student.name, COL_ROLL_NO, &student.roll_no, COL_TOGGLE1, &student.toggle1, COL_TOGGLE2, &student.toggle2, COL_DAYS_PRESENT, &student.days_present, COL_CLASS_DAYS, &student.class_days, -1);
        student.class_days++;
        gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_ROLL_NO, student.roll_no, COL_TOGGLE1, student.toggle1, COL_TOGGLE2, student.toggle2, COL_DAYS_PRESENT, student.days_present, COL_CLASS_DAYS, student.class_days, -1);
    } while (gtk_tree_model_iter_next(model, &iter));
}

void decrease_class_day_by_1() {
   
    GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view));
    GtkTreeIter iter;
    StudentData student;

    gtk_tree_model_get_iter_first(model, &iter);
    int i = 0;
    do {
        gtk_tree_model_get(model, &iter, COL_NAME, &student.name, COL_ROLL_NO, &student.roll_no, COL_TOGGLE1, &student.toggle1, COL_TOGGLE2, &student.toggle2, COL_DAYS_PRESENT, &student.days_present, COL_CLASS_DAYS, &student.class_days, -1);
        if(student.class_days!=0)
        {
            student.class_days--;
        }
        gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_ROLL_NO, student.roll_no, COL_TOGGLE1, student.toggle1, COL_TOGGLE2, student.toggle2, COL_DAYS_PRESENT, student.days_present, COL_CLASS_DAYS, student.class_days, -1);
    } while (gtk_tree_model_iter_next(model, &iter));
}

int* make_all_values_minus_one() {
    int* values = (int*)malloc(number_of_students * sizeof(int));
    for (int i = 0; i < number_of_students; i++) {
        values[i] = -1;
    }
    return values;
 }
void activate_attendance_window_for_taking_attendance(char* email,char** unique_id, char* classname, int number_of_classroom_created_by_user)
{
    message_sent_to_print("              Important!\
                            Supported Image Format : JPG (JPEG), PNG\
                            Size : between 48 * 48 and 4096 * 4096 (pixels)\
                            File size : no larger than 2MB");
   
    GtkWidget* scrolledwindow;
    GtkWidget* grid;
    PangoFontDescription* font_desc;
    GdkRGBA rgba;
    GtkTextTag* tag;
    GtkCssProvider* provider;
    GtkStyleContext* context;
    GtkWidget* button;
    GSimpleAction* action;

    /// ***
    GMenu* menu_bar;
    GMenu* add_file_menu;
    GMenuItem* menu_item_connect;

    /// ***
    StudentData students[100];
    email_to_pass_around = email;
    printf(email);
    /// *** Menu Bar
    menu_bar = g_menu_new();

    /// *** Network_Menu
    add_file_menu = g_menu_new();
    g_menu_append_submenu(menu_bar, "Add photos", G_MENU_MODEL(add_file_menu));



    window5 = gtk_application_window_new(app);
    gtk_window_maximize(GTK_WINDOW(window5));
    scrolledwindow = gtk_scrolled_window_new();
    gtk_window_set_child(GTK_WINDOW(window5), scrolledwindow);
    grid = gtk_grid_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledwindow), grid);
    gtk_window_set_title(GTK_WINDOW(window5), classname);
    gtk_window_set_default_size(GTK_WINDOW(window5), 1920, 1080);
    //    // Initialize GTK
   /* gtk_init(&argc, &argv);*/


    /// *** Create Connect and Disconnect Actions
    action = g_simple_action_new("open_reference_photo", NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action));
    g_signal_connect(action, "activate", G_CALLBACK(action_clbk_for_reference_photo), NULL);

    action = g_simple_action_new("open_present_photo", NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action));
    g_signal_connect(action, "activate", G_CALLBACK(action_clbk_for_present_photo), NULL);

    action = g_simple_action_new("open_all_reference_photo", NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action));
    g_signal_connect(action, "activate", G_CALLBACK(action_clbk_for_reference_photo_all_at_once),NULL);

    action = g_simple_action_new("remove_reference_photo", NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action));
    g_signal_connect(action, "activate", G_CALLBACK(remove_referernce_photo), NULL);

    action = g_simple_action_new("remove_present_photo", NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action));
    g_signal_connect(action, "activate", G_CALLBACK(remove_present_photo), NULL);

    action = g_simple_action_new("remove_both_photos", NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action));
    g_signal_connect(action, "activate", G_CALLBACK(remove_both_photos), NULL);

    /*action = g_simple_action_new*/
    action = g_simple_action_new("add_to_class_day", NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action));
    g_signal_connect(action, "activate", G_CALLBACK(increase_class_day_by_1), NULL);

    action = g_simple_action_new("subtract_from_class_day", NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action));
    g_signal_connect(action, "activate", G_CALLBACK(decrease_class_day_by_1), NULL);

    action = g_simple_action_new("save_data", NULL);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(action));
    g_signal_connect(action, "activate", G_CALLBACK(update_db_with_updated_data),NULL);

    menu_item_connect = g_menu_item_new("Add Reference Photo", "app.open_reference_photo");
    g_menu_append_item(add_file_menu, menu_item_connect);

    menu_item_connect = g_menu_item_new("Add Present Photo", "app.open_present_photo");
    g_menu_append_item(add_file_menu, menu_item_connect);

    menu_item_connect = g_menu_item_new("Add all reference photos", "app.open_all_reference_photo");
    g_menu_append_item(add_file_menu, menu_item_connect);


    add_file_menu = g_menu_new();
    g_menu_append_submenu(menu_bar, "Remove photos", G_MENU_MODEL(add_file_menu));

    menu_item_connect = g_menu_item_new("Remove Reference Photo", "app.remove_reference_photo");
    g_menu_append_item(add_file_menu, menu_item_connect);

    menu_item_connect = g_menu_item_new("Remove Present Photo", "app.remove_present_photo");
    g_menu_append_item(add_file_menu, menu_item_connect);

    menu_item_connect = g_menu_item_new("Remove both", "app.remove_both_photos");
    g_menu_append_item(add_file_menu, menu_item_connect);

    add_file_menu = g_menu_new();
    g_menu_append_submenu(menu_bar, "New Day", G_MENU_MODEL(add_file_menu));


    menu_item_connect = g_menu_item_new("Increase Total Class Days", "app.add_to_class_day");
    g_menu_append_item(add_file_menu, menu_item_connect);

    menu_item_connect = g_menu_item_new("Decrease Total Class Days", "app.subtract_from_class_day");
    g_menu_append_item(add_file_menu, menu_item_connect);

    add_file_menu = g_menu_new();
    g_menu_append_submenu(menu_bar, "Save Data", G_MENU_MODEL(add_file_menu));

    menu_item_connect = g_menu_item_new("Save", "app.save_data");
    g_menu_append_item(add_file_menu, menu_item_connect);

    gtk_application_set_menubar(GTK_APPLICATION(app), G_MENU_MODEL(menu_bar));
    gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(window5), TRUE);

    tree_view = gtk_tree_view_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledwindow), tree_view);

    unique_id_for_classroom = get_names_present_in_resource_json_in_database(email, unique_id, classname, number_of_classroom_created_by_user);
    
    printf(unique_id_for_classroom);
    number_of_students = get_number_of_studetns_in_classroom_selected(email, unique_id_for_classroom);
    
    printf("number of students for attendance window: %d", number_of_students);
    int* days_present = make_all_values_minus_one();
    for (int j = 0; j < number_of_students; j++)
    {
        printf("value_from_attendance_window_before_calling_api = %d\n", days_present[j]);
    }
    days_present = get_present_days_for_each_classroom(email, unique_id_for_classroom, number_of_students);
    for (int j = 0; j < number_of_students; j++) {
        printf("value_from_attendance_window = %d\n", days_present[j]);
    }
    int class_days = -1;
    class_days = get_class_days_from_each_classroom(email, unique_id_for_classroom);
    FILE* file = fopen("names.txt", "r");

    for (int i = 0; i < number_of_students; i++) {
        if (fscanf(file, "%s", students[i].name) != 1) {
            fprintf(stderr, "Error reading file\n");
        }
        students[i].toggle1 = FALSE;
        students[i].toggle2 = FALSE;

        if (days_present[i]!=-1)
        {
            students[i].days_present = days_present[i];
        }
        else {
            students[i].days_present = 0;
        }
        if (class_days!=-1)
        {
            students[i].class_days = class_days;
        }
        else {
            students[i].class_days = 0;
        }

    }
    printf("aagaman below variable naming by another file");
    fclose(file);
  


    for (int i = 0; i < number_of_students; i++) {
        students[i].roll_no = i + 1;
    }

    
    // Create model for TreeView
    GtkListStore* store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_INT, G_TYPE_INT);

    // Add students to model
    for (int i = 0; i < number_of_students; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            COL_NAME, students[i].name,
            COL_ROLL_NO, students[i].roll_no,
            COL_TOGGLE1, students[i].toggle1,
            COL_TOGGLE2, students[i].toggle2,
            COL_DAYS_PRESENT, students[i].days_present,
            COL_CLASS_DAYS, students[i].class_days,
            -1);
        students[i].iter = &iter;
    }
   
    // Create columns for TreeView
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", COL_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Roll No.", renderer, "text", COL_ROLL_NO, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_toggle_new();
    column = gtk_tree_view_column_new_with_attributes("Reference Image", renderer, "active", COL_TOGGLE1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_toggle_new();
    column = gtk_tree_view_column_new_with_attributes("Present Image", renderer, "active", COL_TOGGLE2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Classes Attended", renderer, "text", COL_DAYS_PRESENT, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Total Class Days", renderer, "text", COL_CLASS_DAYS, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    // Set model for TreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(store));

    
    
    g_object_unref(store);
    gtk_widget_show(window5);
}