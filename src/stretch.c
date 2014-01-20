#include <pebble.h>

static Window *window;
static TextLayer *text_layer_top;
static TextLayer *text_layer_middle;
static TextLayer *time_layer;
static AppTimer *timer;

static BitmapLayer *image_layer;
static GBitmap *image_left;
static GBitmap *image_right;
static GBitmap *image_checkmark;

const uint16_t LENGTH_DELAY = 3;
const uint16_t LENGTH_STRETCH = 20;
static uint16_t round = 0;
static uint16_t round_time = 3;
static uint16_t stretch = 0;

char buf[6];
static void timer_callback(void *data) {
  timer = app_timer_register(1000, timer_callback, NULL);
   
  if (round_time == 0) {
    if (stretch == 0) {
      round_time = LENGTH_STRETCH;
      vibes_short_pulse();
      text_layer_set_text(text_layer_middle, "Stretch");
      stretch = 1;
    } else {
      round++;
      round_time = LENGTH_DELAY;      
      vibes_long_pulse();
      text_layer_set_text(text_layer_middle, "Prepare");
      stretch = 0;
    }
  }
  
  snprintf(buf, 6, "%d", round_time);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Round: %d Round Time: %d Time display: %s", round, round_time, buf);
  text_layer_set_text(time_layer, buf);
  
  round_time--;
  
  switch (round) {
    case 0:
      text_layer_set_text(text_layer_top, "Left inner leg");
      bitmap_layer_set_bitmap(image_layer, image_left);
      break;
    case 1:
      text_layer_set_text(text_layer_top, "Right inner leg");
      bitmap_layer_set_bitmap(image_layer, image_right);
      break;
    case 2:
      text_layer_set_text(text_layer_top, "Left calf");
      break;
    case 3:
      text_layer_set_text(text_layer_top, "Right calf");
      break;
    case 4:
      text_layer_set_text(text_layer_top, "Left front leg");
      break;
    case 5:
      text_layer_set_text(text_layer_top, "Right front leg");
      break;
    default:
      text_layer_set_text(text_layer_top, "You are done!");
      text_layer_set_text(text_layer_middle, "");
      text_layer_set_text(time_layer, "");
      
      bitmap_layer_set_bitmap(image_layer, image_checkmark);
      vibes_double_pulse();
      app_timer_cancel(timer);
      break;
  }
  
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  vibes_short_pulse();
  text_layer_set_text(text_layer_middle, "Prepare");
  timer_callback(NULL);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer_top = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 40 } });
  text_layer_set_text(text_layer_top, "Press Select to start Stretch");
  text_layer_set_text_alignment(text_layer_top, GTextAlignmentCenter);
  text_layer_set_overflow_mode(text_layer_top, GTextOverflowModeWordWrap);
  text_layer_set_font(text_layer_top, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

  text_layer_middle = text_layer_create((GRect) { .origin = { 0, 30 }, .size = { bounds.size.w, 40 } });
  text_layer_set_text_alignment(text_layer_middle, GTextAlignmentCenter);
  text_layer_set_font(text_layer_middle, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  
  time_layer = text_layer_create((GRect) { .origin = { 5, 85 }, .size = { 80, 49 } });
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  
  // Image
  image_left = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SIDE_LUNGE_LEFT);
  image_right = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SIDE_LUNGE_RIGHT);
  image_checkmark = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHECKMARK);
  
  const GPoint center = grect_center_point(&bounds);
  GRect image_frame = (GRect) { .origin = center, .size = image_left->bounds.size };
  image_frame.origin.x = 72;
  image_frame.origin.y = 80;
  
  image_layer = bitmap_layer_create(image_frame);

  layer_add_child(window_layer, text_layer_get_layer(text_layer_middle));
  layer_add_child(window_layer, text_layer_get_layer(text_layer_top));
  layer_add_child(window_layer, text_layer_get_layer(time_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer_top);
  text_layer_destroy(text_layer_middle);
  text_layer_destroy(time_layer);	

  bitmap_layer_destroy(image_layer);
  gbitmap_destroy(image_left);
  gbitmap_destroy(image_right);
  gbitmap_destroy(image_checkmark);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
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