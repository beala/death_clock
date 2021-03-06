#include <pebble.h>
#include "PDutils.h"

static Window *window;
static TextLayer *text_layer;
static char display_string[20] = "";
static const long int death_second = 2990239200;

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  time_t cur_sec = p_mktime(tick_time);
  time_t time_left = difftime(death_second, cur_sec);
  // TODO: Factor this out, and generalize the logic.
  long int billions = time_left/1000000000;
  long int millions = (time_left - billions*1000000000) /1000000;
  long int thousands = (time_left - billions*1000000000 - millions*1000000)/1000;
  long int hundreds = time_left - billions*1000000000 - millions*1000000 - thousands*1000;
  snprintf(display_string, 20, "%ld\n%03ld\n%03ld\n%03ld", billions, millions, thousands, hundreds);
  text_layer_set_text(text_layer, display_string);
  layer_mark_dirty((Layer*) text_layer);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(window, GColorBlack);

  text_layer = text_layer_create((GRect) { .origin = { 0, -5 }, .size = { bounds.size.w, bounds.size.h + 5} });
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_font(text_layer, fonts_get_system_font("RESOURCE_ID_BITHAM_42_BOLD"));
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
