#ifndef POST_BASIC_FILE_TO_DATABASE
#define POST_BASIC_FILE_TO_DATABASE

int post_basic_file_to_database(char* json_data, char* email);
char* post_resource_file_to_database(char* json_data, char* email);
int update_resource_file_in_database(char* json_data, char* email, char* unique_identifier_for_each_class);
char** get_data_present_in_main_json_in_database(char *email, int* number_of_classes_created_by_user, int* class_status);
char* get_names_present_in_resource_json_in_database(char* email, char** unique_id, char* classname, int number_of_classroom_created_by_user);
char* get_classname_only_from_resource_json(char* email, char* unique_id);
void delete_data_previously_present_in_main_json_in_database(char* email);
int get_number_of_studetns_in_classroom_selected(char* email, char* unique_id_for_classroom);
int* get_present_days_for_each_classroom(char* email, char* unique_id_for_classroom, int number_of_students);
int get_class_days_from_each_classroom(char* email, char* unique_id_for_classroom);

#endif