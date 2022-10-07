#include <pebble.h>

static Window *s_main_window;

static TextLayer *s_time_layer;
static TextLayer *s_weather_layer;

static GFont s_pt_astra_sans_regular_48;
static GFont s_pt_astra_sans_regular_24;

static void update_time() {

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), "%H:%M : %I:%M", tick_time);

  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  if (tick_time->tm_min % 30 == 0) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    dict_write_uint8(iter, 0, 0);

    app_message_outbox_send();
  }
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_time_layer = text_layer_create(GRect(0, 52, bounds.size.w, bounds.size.h));
  s_weather_layer = text_layer_create(GRect(0, 120, bounds.size.w, 30));

  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_background_color(s_weather_layer, GColorClear);

  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text_color(s_weather_layer, GColorWhite);

  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text(s_weather_layer, "Loading...");

  s_pt_astra_sans_regular_48 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PT_ASTRA_SANS_REGULAR_48));
  s_pt_astra_sans_regular_24 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PT_ASTRA_SANS_REGULAR_24));
  
  text_layer_set_font(s_time_layer, s_pt_astra_sans_regular_48);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  text_layer_set_font(s_weather_layer, s_pt_astra_sans_regular_24);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);

  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_weather_layer);

  fonts_unload_custom_font(s_pt_astra_sans_regular_48);
  fonts_unload_custom_font(s_pt_astra_sans_regular_24);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];

  Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
  Tuple *conditions_tuple  = dict_find(iterator, MESSAGE_KEY_CONDITIONS);
  
  if(temp_tuple && conditions_tuple) {
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);

    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
    text_layer_set_text(s_weather_layer, weather_layer_buffer);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);
  window_set_background_color(s_main_window, GColorBlack);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  update_time();

  app_message_register_inbox_received(inbox_received_callback);

  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);

  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
