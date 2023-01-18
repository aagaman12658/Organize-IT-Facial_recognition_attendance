/* ========================================================================= */
/**
 * @file ErrorMessages.c
 * @author Organize-IT!
 * @date 2023
 */
 /* ========================================================================= */

/** @defgroup ErrorMessages ErrorMessages.c
 * This file contains featues that allows the new user to sign up
 * @{
 */

/* ========================================================================= */
/* Include files section                                                     */
/* ========================================================================= */

//c library headers
#include<gtk-4.0/gtk/gtk.h>
#include<curl/curl.h>
#include<stdio.h>

//custom headers created by user
#include "gtk_c_gui.h"
#include "ErrorMessages.h"
#include "attendance_window_public.h"


/* ========================================================================= */
/* Fucntion prototypes section                                               */
/* ========================================================================= */

//function prototypes
void passwords_do_not_match();
void signup_successful();
void invalid_email_id();
void error_message_sent_by_api(char* message);
void message_sent_to_print(char* message);




//function definitions


//displays passwords do not match prompt
/*! \fn     void passwords_do_not_match()
    \brief  Displays passwords do not match prompt
*/
void passwords_do_not_match() {
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
    title = gtk_label_new("Passwords Do Not Match!");
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
    
    button = gtk_button_new_with_label("ok");
    gtk_grid_attach(GTK_GRID(grid), button, 4, 5, 1, 1);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window2);
    gtk_window_set_transient_for(window2, window);

    gtk_widget_show(window2);


}


//displays signup_successful message
/*! \fn     void signup_successful()
    \brief  Displays signup_successful message
*/
void signup_successful() {
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
    gtk_window_set_title(GTK_WINDOW(window2), "Congratulations:");
    gtk_window_set_default_size(GTK_WINDOW(window2), 300, 100);
    css_for_window(window2);
    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window2), grid);
    gtk_window_set_modal(window2, true);
    title = gtk_label_new("Signup Successful!");
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

    button = gtk_button_new_with_label("ok");
    gtk_grid_attach(GTK_GRID(grid), button, 4, 5, 1, 1);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window2);
    gtk_window_set_transient_for(window2, window);


    gtk_widget_show(window2);

}

//displays invalid email id prompt
/*! \fn     void invalid_email_id()
    \brief  Displays invalid email id prompt
*/
void invalid_email_id() {
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
    title = gtk_label_new("Email doesnot exist!");
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

    button = gtk_button_new_with_label("ok");
    gtk_grid_attach(GTK_GRID(grid), button, 4, 5, 1, 1);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window2);
    gtk_window_set_transient_for(window2, window);

    gtk_widget_show(window2);


}

//displays any message sent by api 
/*! \fn    void error_message_sent_by_api(char* message)
    \brief  Displays any message sent by api 
*/
void error_message_sent_by_api(char* message) {
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
    title = gtk_label_new(message);
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

    button = gtk_button_new_with_label("ok");
    gtk_grid_attach(GTK_GRID(grid), button, 4, 5, 1, 1);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window2);
    gtk_window_set_transient_for(window2, window);

    gtk_widget_show(window2);


}

//displays message sent to print 
/*! \fn    void message_sent_to_print(char* message) 
    \brief  Displays any message sent to print 
*/
void message_sent_to_print(char* message) {
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
    gtk_window_set_title(GTK_WINDOW(window2), "Attention!!!");
    gtk_window_set_default_size(GTK_WINDOW(window2), 300, 100);
    css_for_window(window2);
    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window2), grid);
    gtk_window_set_modal(window2, true);
    title = gtk_label_new(message);
    gtk_grid_attach(GTK_GRID(grid), title, 0, 0, 10, 5);
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "label {"
        " font: 30px sans;"
        "  color: white;"
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


    gtk_widget_show(window2);
}

/** @} */