#include "reader2.h"
#include <pebble.h>

char *words[268];
unsigned page = 0;
bool playing = false;
unsigned speed = 2;
unsigned speed_counter = 0;
static GBitmap *image_pause;
static GBitmap *image_play;

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GBitmap *s_res_image_play;
static GFont s_res_roboto_condensed_21;
static ActionBarLayer *s_actionbarlayer_1;
static TextLayer *s_page;
static TextLayer *s_title;
static TextLayer *s_author;
static TextLayer *s_progress;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  
  s_res_image_play = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PLAY);
  s_res_roboto_condensed_21 = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);
  // s_actionbarlayer_1
  s_actionbarlayer_1 = action_bar_layer_create();
  action_bar_layer_add_to_window(s_actionbarlayer_1, s_window);
  action_bar_layer_set_background_color(s_actionbarlayer_1, GColorBlack);
  action_bar_layer_set_icon(s_actionbarlayer_1, BUTTON_ID_SELECT, s_res_image_play);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_actionbarlayer_1);
  
  // s_page
  s_page = text_layer_create(GRect(0, 60, 124, 30));
  text_layer_set_text(s_page, "areallybigwordnow");
	text_layer_set_text_alignment(s_page, GTextAlignmentCenter);
  text_layer_set_font(s_page, s_res_roboto_condensed_21);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_page);
  
  // s_title
  s_title = text_layer_create(GRect(0, 0, 124, 70));
  text_layer_set_text(s_title, "The Hacker Crackdown and more");
  text_layer_set_text_alignment(s_title, GTextAlignmentCenter);
  text_layer_set_font(s_title, s_res_roboto_condensed_21);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_title);
  
  // s_author
  s_author = text_layer_create(GRect(0, 90, 124, 35));
  text_layer_set_text(s_author, "by Bruce Stearling and William Gibson");
  text_layer_set_text_alignment(s_author, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_author);
  
  // s_progress
  s_progress = text_layer_create(GRect(0, 132, 124, 20));
  text_layer_set_text(s_progress, "0% complete");
  text_layer_set_text_alignment(s_progress, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_progress);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  action_bar_layer_destroy(s_actionbarlayer_1);
  text_layer_destroy(s_page);
  text_layer_destroy(s_title);
  text_layer_destroy(s_author);
  text_layer_destroy(s_progress);
  gbitmap_destroy(s_res_image_play);
}
// END AUTO-GENERATED UI CODE

static void next_page(void){
	
	if(page < 267){ // todo: use this when it works right: (sizeof(words) / sizeof(char)) - 1){
		page++;
		text_layer_set_text(s_page, words[page]);
	} else {
		
		// reset page pointer
		page = 0;
		
		// return to "cover"
		playing = false;
	}
}

static void prev_page(void){
	if(page < 267){ // todo: use sizeof(words)){
		page--;
		text_layer_set_text(s_page, words[page]);
	}
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	
	// reduce page delay
	speed++;
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {

	// increase page delay
	speed--;
	
}

static void middle_click_handler(ClickRecognizerRef recognizer, void *context){
	
	// pause/resume page turning
	if(playing){
		
		// update progres
		float progress = ((float)page / 268) * 100;  // todo: make based on actual size of book
		static char progress_string[20];
		snprintf(progress_string, 20, "%d%% complete", (int)progress);
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "progress_string: %s", progress_string);
		text_layer_set_text(s_progress, progress_string);
		
		// show info
		layer_set_hidden((Layer *)s_title, false);
		layer_set_hidden((Layer *)s_author, false);
		layer_set_hidden((Layer *)s_progress, false);
		
		// show correct controls
		layer_set_hidden((Layer *)s_page, true);
		action_bar_layer_set_icon(s_actionbarlayer_1, BUTTON_ID_SELECT, image_play);
		
		playing = false;
		
	} else {
		
		// hide info
		layer_set_hidden((Layer *)s_title, true);
		layer_set_hidden((Layer *)s_author, true);
		layer_set_hidden((Layer *)s_progress, true);
		
		// show page
		layer_set_hidden((Layer *)s_page, false);
		
		// indicate playing
		action_bar_layer_set_icon(s_actionbarlayer_1, BUTTON_ID_SELECT, image_pause);
		
		playing = true;
	}
	
}

// timer events
static void handle_page_timer(void *data) {

	if(playing){
		
		// adjust speed
		speed_counter = speed_counter + speed;
		
		if(speed_counter > 100){
			next_page();	
			speed_counter = 0;
		}
	}	else {
		

	}
	
	app_timer_register(10,handle_page_timer,NULL);
}

void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) down_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) up_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) middle_click_handler);
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_reader2(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
	
	// Set the click config provider:
  window_set_click_config_provider(s_window,click_config_provider);
	
	// set the pause button image
	image_pause = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PAUSE);
	image_play = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PLAY);
	
	// load the book
	ResHandle text_handle = resource_get_handle(RESOURCE_ID_TEXT_THC_INTRO_1);
	uint8_t *book_text = malloc(resource_size(text_handle));
	resource_load(text_handle,book_text,1024);
	
	char* currentCharacter = (char*)(book_text);
	int currentWord = 0;
	
	// apparently I'm not smart enough to do dynamic memory in C...
/*
	// count the words & allocate word array memory
	while (currentCharacter < (char*)book_text + resource_size(text_handle)) {
		++currentCharacter;
		if (*currentCharacter == ' ') {
			
			// terminate string
			*currentCharacter = '\0';
			
			// start next word
			++currentWord;
			++currentCharacter; // skip the space
		}
	}

	words = malloc(currentWord * sizeof(char*));
*/
	
	//words = malloc(1024 * sizeof(char*));
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "finished allocating word array");

	// load the word array
	//currentCharacter = (char*)(book_text);
	//currentWord = 0;
	words[0] = (char *)currentWord;
	
	while (currentCharacter < (char*)book_text + resource_size(text_handle)) {
		
		// newlines are stripped for now, but
		// double-newlines probably indicate a paragraph
		// and should be tagged at some point
		if(*currentCharacter == '\n'){
			*currentCharacter = ' ';
		}
		
		if (*currentCharacter == ' ') {
					
			// terminate string
			*currentCharacter = '\0';
			
			// start the next word
			++currentCharacter; // skip the terminator
			++currentWord;
			words[currentWord] = (char *)currentCharacter;
		} else {
			++currentCharacter;	
		}
	}
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "done loading");
	
	// set the title and by-line (hard-coded until a convention
	// is developed for the input files)
	text_layer_set_text(s_title, "The Hacker Crackdown");
	text_layer_set_text(s_author, "by Bruce Sterling");
	
	// set initial progress
	text_layer_set_text(s_progress, "0% complete");
	
	// set the first word
	text_layer_set_text(s_page, words[0]);
	
	free(book_text);
	
	// start timer to auto-update
	app_timer_register(1000, handle_page_timer, NULL);
}

void hide_reader2(void) {
  window_stack_remove(s_window, true);
}
