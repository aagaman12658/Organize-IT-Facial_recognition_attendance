#include <stdio.h>
#include <curl/curl.h>
#include "gtk_c_gui.h"
#include <jansson.h>
#include "post_basic_file_to_database.h"
#include "gtk_dashboard_gui.h"



struct MemoryStruct {
    char* memory;
    size_t size;
};





//function declaration
static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);
static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream);
void get_the_part_of_email_before_service_provider_name(char* email);
void delete_data_previously_present_in_main_json_in_database(char* email);
char** get_data_present_in_main_json_in_database(char* email, int* number_of_classes_created_by_user, int* class_status);
int get_number_of_studetns_in_classroom_selected(char* email, char* unique_id_for_classroom);
int* get_present_days_for_each_classroom(char* email, char* unique_id_for_classroom, int number_of_students);
int get_class_days_from_each_classroom(char* email, char* unique_id_for_classroom);
char* get_names_present_in_resource_json_in_database(char* email, char** unique_id, char* classname, int number_of_classroom_created_by_user);
char* get_classname_only_from_resource_json(char* email, char* unique_id);
int post_basic_file_to_database(char* json_data, char* email);
int update_resource_file_in_database(char* json_data, char* email, char* unique_identifier_for_each_class);

//global variables not to be changed manually
char json_file_name[20];










//writes json response to a variable
static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    char* ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}


//write json response to a file
static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
    return written;
}

//gets the part infront of @ in an email address so it can be saved as unique id for each user in database
void get_the_part_of_email_before_service_provider_name(char* email) {
    char* token =strtok(email, "@");
    strcpy(json_file_name, token);
}


//<<<<<<<<<<<<<<<<<<<<<------------------under construction--------------------------------->>>>>>>>>>>>>>>>>>>>>>>>>
//but still it deletes something which is still not decided from the database
void delete_data_previously_present_in_main_json_in_database(char* email) {
    CURL* curl;
    CURLcode res;
    get_the_part_of_email_before_service_provider_name(email);
    char uri[100] = "https://organize-it-7d19e-default-rtdb.firebaseio.com/";
    strcat(uri, json_file_name);
    strcat(uri, "/main.json");
    curl = curl_easy_init();
    if (curl) {

        curl_easy_setopt(curl, CURLOPT_URL, uri);

        // Set the request type to DELETE
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

        // Add the Firebase Realtime Database secret in a header 
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        }

        // Clean up
        curl_easy_cleanup(curl);
    }
}


//gets data present in main.json present for specific user as specified in the url
char** get_data_present_in_main_json_in_database(char* email, int* number_of_classes_created_by_user, int* class_status) {
    FILE* fp = fopen("get_main_json.txt", "wb+");
    if (!fp)
    {
        printf("Error opening output file\n");
        return 1;
    }
 

    CURL* curl;
    CURLcode res;
    get_the_part_of_email_before_service_provider_name(email);
    char uri[100] = "https://organize-it-7d19e-default-rtdb.firebaseio.com/";
    strcat(uri, json_file_name);
    strcat(uri, "/main.json");
    curl = curl_easy_init();
    if (curl) {

        curl_easy_setopt(curl, CURLOPT_URL, uri);

        // Set the request type to DELETE
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

        // Add the Firebase Realtime Database secret in a header 
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        //write inside file
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        }

        // Clean up
        curl_easy_cleanup(curl);
    }
    fclose(fp);
    fp = fopen("get_main_json.txt", "rb");
    //added to check whether CURLOPT_WRITEFUNCTION was working properly
    /*ShellExecute(NULL, "open", "get_main_json.txt", NULL, NULL, SW_SHOWNORMAL);*/
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = (char*)malloc(file_size+1);
    if (buffer == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return 1;
    }

    size_t read_size = fread(buffer, 1, file_size, fp);
    if (read_size != file_size) {
        fprintf(stderr, "Error reading file\n");
        return 1;
    }

    buffer[file_size-1] = '\0';
    json_t* root;
    json_error_t error;
    root = json_load_file("get_main_json.txt", 0, &error);

    // Extract the values of the fields
    const char* email_from_db= json_string_value(json_object_get(root, "Email"));
    *number_of_classes_created_by_user = json_integer_value(json_object_get(root, "Number_of_classrooms"));
    *class_status = json_integer_value(json_object_get(root, "current_class_status"));
    int loop_limit = *number_of_classes_created_by_user;
    char** classrooms = (char**)malloc(loop_limit * sizeof(char*));
    for (int i = 0; i < loop_limit; i++) {
        char key[15];
        sprintf(key, "classroom_%d", i);
        json_t* classroom = json_object_get(root, key);

        if (!json_is_string(classroom)) {
            printf("error: %s is not a string\n", key);
            json_decref(root);
            return NULL;
        }

        classrooms[i] = (char*)json_string_value(classroom);
        printf("classroom_%d : %s", i, classrooms[i]);
    }
    free(buffer);
    return classrooms;// this returns to 

}

//fetches teh number of students present in the database. please dont edit the studetns error present in the naming as it might interfere
//with other file where the function is called
int get_number_of_studetns_in_classroom_selected(char*email,char* unique_id_for_classroom){
    int i = 0;

    CURL* curl;
    CURLcode res;
    json_t* root;
    json_error_t error;
    struct MemoryStruct chunk;

    chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    curl = curl_easy_init();
    if (curl) {
        get_the_part_of_email_before_service_provider_name(email);
        char uri[100] = "https://organize-it-7d19e-default-rtdb.firebaseio.com/";
        strcat(uri, json_file_name);
        strcat(uri, "/resource.json");
        curl_easy_setopt(curl, CURLOPT_URL, uri);

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

        // Add the Firebase Realtime Database secret in a header 
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        }
        else {

            curl_easy_cleanup(curl);
            root = json_loads(chunk.memory, 0, &error);

            if (!root) {
                fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
            }


            json_t* object = json_object_get(root, unique_id_for_classroom);
            int number_of_students = json_integer_value(json_object_get(object, "number_of_students"));
            json_decref(root);
            free(chunk.memory);
            return number_of_students;
        }


    }

}

//fetches the number of days each student was present 
int* get_present_days_for_each_classroom(char* email, char* unique_id_for_classroom,int number_of_students) {
    int i = 0;

    CURL* curl;
    CURLcode res;
    json_t* root;
    json_error_t error;
    struct MemoryStruct chunk;

    chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    curl = curl_easy_init();
    if (curl) {
        get_the_part_of_email_before_service_provider_name(email);
        char uri[100] = "https://organize-it-7d19e-default-rtdb.firebaseio.com/";
        strcat(uri, json_file_name);
        strcat(uri, "/resource.json");
        curl_easy_setopt(curl, CURLOPT_URL, uri);

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

        // Add the Firebase Realtime Database secret in a header 
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        }
        else {

            curl_easy_cleanup(curl);
            root = json_loads(chunk.memory, 0, &error);

            if (!root) {
                fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
            }
           

            //get the present_days object
            json_t* object = json_object_get(root, unique_id_for_classroom);
            json_t* present_days = json_object_get(object, "present_days");


            //create an array to store the values
            int* values = (int*)malloc(number_of_students * sizeof(int));
            for (int j = 0; j < number_of_students; j++) {
                values[j] = 0;
            }
            int i = 0;

            //iterate through the present_days object
            const char* key;
            json_t* value;
            json_object_foreach(present_days, key, value) {
                //get the value for each key and store it in the array
                values[i] = json_integer_value(value);
                i++;
            }

            //print the values
            for (int j = 0; j < i; j++) {
                printf("value = %d\n", values[j]);
            }

            //clean up
            json_decref(root);

            return values;

        }
    }
}


//fetches number of days the class was conducted for each classroom
int get_class_days_from_each_classroom(char* email,char* unique_id_for_classroom) {
    int i = 0;

    CURL* curl;
    CURLcode res;
    json_t* root;
    json_error_t error;
    struct MemoryStruct chunk;

    chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    curl = curl_easy_init();
    if (curl) {
        get_the_part_of_email_before_service_provider_name(email);
        char uri[100] = "https://organize-it-7d19e-default-rtdb.firebaseio.com/";
        strcat(uri, json_file_name);
        strcat(uri, "/resource.json");
        curl_easy_setopt(curl, CURLOPT_URL, uri);

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

        // Add the Firebase Realtime Database secret in a header 
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        }
        else {

            curl_easy_cleanup(curl);
            root = json_loads(chunk.memory, 0, &error);

            if (!root) {
                fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
            }


            //get the present_days object
            json_t* object = json_object_get(root, unique_id_for_classroom);
            int class_days = json_integer_value(json_object_get(object, "class_days"));


        
            //clean up
            json_decref(root);

            return class_days;

        }
    }
}


//gets names present in resource.json saved in the database
char* get_names_present_in_resource_json_in_database(char* email, char** unique_id, char* classname, int number_of_classroom_created_by_user) {
    int i = 0;

    CURL* curl;
    CURLcode res;
    json_t* root;
    json_error_t error;
    struct MemoryStruct chunk;

    chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    curl = curl_easy_init();
    if (curl) {
        get_the_part_of_email_before_service_provider_name(email);
        char uri[100] = "https://organize-it-7d19e-default-rtdb.firebaseio.com/";
        strcat(uri, json_file_name);
        strcat(uri, "/resource.json");
        curl_easy_setopt(curl, CURLOPT_URL, uri);

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

        // Add the Firebase Realtime Database secret in a header 
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        }
        else {

            curl_easy_cleanup(curl);
            root = json_loads(chunk.memory, 0, &error);
            
            if (!root) {
                fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
            }

          
            int index_having_same_classname = 0;
            for (int i = 0; i < number_of_classroom_created_by_user; i++)
            {
                json_t* unique_ids = json_object_get(root, unique_id[i]);
                char* classname_test = (char*)json_string_value(json_object_get(unique_ids, "classname"));
                if (strcmp(classname, classname_test) == 0) {
                    index_having_same_classname = i;
                    break;
                }
            }
            printf("Got the index: %d", index_having_same_classname);
            char** students_names = (char**)malloc(1024 * sizeof(char));
            // Extract names
            json_t* unique_ids = json_object_get(root, unique_id[index_having_same_classname]);
            json_t* name_object = json_object_get(unique_ids, "name");
            int i;
            for (i = 0; i < 100; i++) {
                students_names[i] = (char*)json_string_value(json_array_get(name_object, i));
                printf("%s\n", students_names[i]);
            }
            FILE* file = fopen("names.txt", "w");//dont change this names.txt file
            if (file == NULL) {
                printf("Error opening file!\n");
            }
            for (int i = 0; i < 100; i++) {
                fprintf(file, "%s\n", students_names[i]);
            }
            fclose(file);
            json_decref(root);
            
            free(students_names);
            free(chunk.memory);
            return unique_id[index_having_same_classname];
           }

       
    }
 
}


//fetches classname only from resource.json saved in firebase realtime database
char* get_classname_only_from_resource_json(char* email, char *unique_id) {
    FILE* fp = fopen("get_classname_from_resource_json.txt", "wb+");
    if (!fp)
    {
        printf("Error opening output file\n");
        return 1;
    }


    CURL* curl;
    CURLcode res;
    printf("unique_id= %s\n\n\n\n", unique_id);
    get_the_part_of_email_before_service_provider_name(email);
    char uri[100] = "https://organize-it-7d19e-default-rtdb.firebaseio.com/";
    strcat(uri, json_file_name);
    strcat(uri, "/resource.json");
    curl = curl_easy_init();
    if (curl) {

        curl_easy_setopt(curl, CURLOPT_URL, uri);

        // Set the request type to DELETE
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

        // Add the Firebase Realtime Database secret in a header 
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        //write inside file
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        // Perform the request
        res = curl_easy_perform(curl);
        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        }

        // Clean up
        curl_easy_cleanup(curl);
    }
    fclose(fp);
    fp = fopen("get_classname_from_resource_json.txt", "rb");
    //added to check whether CURLOPT_WRITEFUNCTION was working properly
  /*  ShellExecute(NULL, "open", "get_classname_from_resource_json.txt", NULL, NULL, SW_SHOWNORMAL);*/
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

    buffer[file_size-1] = '\0';
    json_t* root;
    json_error_t error;

    root = json_load_file("get_classname_from_resource_json.txt", 0, &error);

    if (!root) {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
        return 1;
    }

    // Extract classname
    json_t* unique_ids = json_object_get(root, unique_id);
    char* classname = json_string_value(json_object_get(unique_ids, "classname"));
    fclose(fp);
    free(buffer);
    return classname;

}


//posts ( **using patch request so no new unique_id is created) basic file to database here basic files mean main.json
int post_basic_file_to_database(char* json_data, char*email) {

    // Use curl to send a POST request to the realtime database with the json data
    CURL* curl = curl_easy_init();
    // Open the image file for reading


    if (!curl) {
        printf("Error: unable to initialize curl\n");
        return 1;
    }
    printf("email = %s", email);
    get_the_part_of_email_before_service_provider_name(email);
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    char uri[100] = "https://organize-it-7d19e-default-rtdb.firebaseio.com/";
    strcat(uri, json_file_name);
    strcat(uri, "/main.json");
    curl_easy_setopt(curl, CURLOPT_URL, uri);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        printf("Error: unable to send request: %s\n", curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);
    free(json_data);
    if (res == CURLE_OK) {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code == 200) {
            printf("Successfully posted image to the realtime database\n");
        }
        else {
            printf("Error: received response code %ld\n", response_code);
        }
    }
    curl_slist_free_all(headers);

    return (int)res;
}

//posts (**using POST request so unique id is created for each classroom) resource.json to database
char* post_resource_file_to_database(char* json_data, char* email) {
    FILE* fp = fopen("postresponse.txt", "wb+");
    if (!fp)
    {
        printf("Error opening output file\n");
        return 1;
    }
    // Use curl to send a POST request to the realtime database with the json data
    CURL* curl = curl_easy_init();
    // Open the image file for reading


    if (!curl) {
        printf("Error: unable to initialize curl\n");
        return 1;
    }
    printf("email = %s", email);
    get_the_part_of_email_before_service_provider_name(email);
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    char uri[100] = "https://organize-it-7d19e-default-rtdb.firebaseio.com/";
    strcat(uri, json_file_name);
    strcat(uri, "/resource.json");
    curl_easy_setopt(curl, CURLOPT_URL, uri);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

    //write inside file
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        printf("Error: unable to send request: %s\n", curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);
    free(json_data);
    if (res == CURLE_OK) {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code == 200) {
            printf("Successfully posted image to the realtime database\n");
        }
        else {
            printf("Error: received response code %ld\n", response_code);
        }
    }
    curl_slist_free_all(headers);
    fclose(fp);
    fp = fopen("postresponse.txt", "rb");
    //added to check whether CURLOPT_WRITEFUNCTION was working properly
    /*ShellExecute(NULL, "open", "postresponse.txt", NULL, NULL, SW_SHOWNORMAL);*/
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

    buffer[file_size-1] = '\0';
    json_t* root;
    json_error_t error;
    root = json_load_file("postresponse.txt", 0, &error);

    // Extract the values of the fields
    char* response = json_string_value(json_object_get(root, "name"));
    free(buffer);
    return response;
}


//updates the resource.json present in database
int update_resource_file_in_database(char* json_data, char* email, char* unique_identifier_for_each_class) {

    // Use curl to send a POST request to the realtime database with the json data
    CURL* curl = curl_easy_init();
    // Open the image file for reading


    if (!curl) {
        printf("Error: unable to initialize curl\n");
        return 1;
    }
    printf("email = %s", email);
    get_the_part_of_email_before_service_provider_name(email);
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    char uri[1024] = "https://organize-it-7d19e-default-rtdb.firebaseio.com/";
    strcat(uri, json_file_name);
    strcat(uri, "/resource/");
    strcat(uri, unique_identifier_for_each_class);
    strcat(uri, ".json");
    curl_easy_setopt(curl, CURLOPT_URL, uri);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        printf("Error: unable to send request: %s\n", curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);
    free(json_data);
    if (res == CURLE_OK) {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code == 200) {
            printf("Successfully posted image to the realtime database\n");
        }
        else {
            printf("Error: received response code %ld\n", response_code);
        }
    }
    curl_slist_free_all(headers);
    return 0;
 }
