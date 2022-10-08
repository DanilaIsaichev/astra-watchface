#include <pebble.h>

static Window *s_main_window;

static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_day_layer;
static TextLayer *s_weather_layer;
static TextLayer *s_temperature_layer;

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

//
static void update_date() {

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char s_date_buffer[14];
  strftime(s_date_buffer, sizeof(s_date_buffer), "%B %e", tick_time);

  text_layer_set_text(s_date_layer, s_date_buffer);

  static char s_day_buffer[9];
  strftime(s_day_buffer, sizeof(s_day_buffer), "%A", tick_time);
  text_layer_set_text(s_day_layer, s_day_buffer);
}

static void date_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_date();
}
//

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_pt_astra_sans_regular_48 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PT_ASTRA_SANS_REGULAR_48));
  s_pt_astra_sans_regular_24 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PT_ASTRA_SANS_REGULAR_24));

  GPoint s_layer_coords = GPoint(0, bounds.size.h * 0.05);
  GSize s_layer_size = graphics_text_layout_get_content_size("September 30", s_pt_astra_sans_regular_24, bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter);
  s_date_layer = text_layer_create(GRect(0, s_layer_coords.y, bounds.size.w, s_layer_size.h + 3));//+4

  s_layer_coords = GPoint(0, s_layer_coords.y + s_layer_size.h);
  s_layer_size = graphics_text_layout_get_content_size("00:00", s_pt_astra_sans_regular_48, bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter);
  s_time_layer = text_layer_create(GRect(0, s_layer_coords.y, bounds.size.w, s_layer_size.h));

  s_layer_coords = GPoint(0, s_layer_coords.y + s_layer_size.h);
  s_layer_size = graphics_text_layout_get_content_size("Wednesday", s_pt_astra_sans_regular_24, bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter);
  s_day_layer = text_layer_create(GRect(0, s_layer_coords.y, bounds.size.w, s_layer_size.h + 3));

  s_layer_coords = GPoint(0, s_layer_coords.y + s_layer_size.h);
  s_layer_size = graphics_text_layout_get_content_size("Thunderstorm", s_pt_astra_sans_regular_24, bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter);
  s_weather_layer = text_layer_create(GRect(0, s_layer_coords.y, bounds.size.w, s_layer_size.h + 3));

  s_layer_coords = GPoint(0, s_layer_coords.y + s_layer_size.h);
  s_layer_size = graphics_text_layout_get_content_size("31 °C", s_pt_astra_sans_regular_24, bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter);
  s_temperature_layer = text_layer_create(GRect(0, s_layer_coords.y, bounds.size.w, s_layer_size.h + 3));

  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_background_color(s_day_layer, GColorClear);
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_background_color(s_temperature_layer, GColorClear);

  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text_color(s_day_layer, GColorWhite);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_color(s_temperature_layer, GColorWhite);

  text_layer_set_text(s_date_layer, "Month 0");
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text(s_day_layer, "Day");
  text_layer_set_text(s_weather_layer, "Loading...");
  text_layer_set_text(s_temperature_layer, "Loading...");

  text_layer_set_font(s_date_layer, s_pt_astra_sans_regular_24);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  text_layer_set_font(s_time_layer, s_pt_astra_sans_regular_48);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  text_layer_set_font(s_day_layer, s_pt_astra_sans_regular_24);
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);

  text_layer_set_font(s_weather_layer, s_pt_astra_sans_regular_24);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);

  text_layer_set_font(s_temperature_layer, s_pt_astra_sans_regular_24);
  text_layer_set_text_alignment(s_temperature_layer, GTextAlignmentCenter);

  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_day_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_temperature_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_day_layer);
  text_layer_destroy(s_weather_layer);
  text_layer_destroy(s_temperature_layer);

  fonts_unload_custom_font(s_pt_astra_sans_regular_48);
  fonts_unload_custom_font(s_pt_astra_sans_regular_24);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  static char temperature_buffer[8];
  static char conditions_buffer[32];

  Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
  Tuple *conditions_tuple  = dict_find(iterator, MESSAGE_KEY_CONDITIONS);
  
  if(temp_tuple && conditions_tuple) {
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d °C", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);

    text_layer_set_text(s_weather_layer, conditions_buffer);
    text_layer_set_text(s_temperature_layer, temperature_buffer);
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
  tick_timer_service_subscribe(DAY_UNIT, date_tick_handler);//
  update_time();
  update_date();

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
