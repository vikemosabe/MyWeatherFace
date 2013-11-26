#include <pebble.h>
#include "config.h"

#define ConstantGRect(x, y, w, h) {{(x), (y)}, {(w), (h)}}
 
static Window *window;
static GBitmap     *background_image;
static GBitmap     *battery_image;
static GBitmap     *bluetooth_image;
static GBitmap     *icon_image;
static BitmapLayer *background_layer;
static BitmapLayer *battery_layer;
static BitmapLayer *bluetooth_layer;
static BitmapLayer *icon_layer;
static TextLayer *temp_layer;
static TextLayer *bar_layer;
static TextLayer *sunrise_layer;
static TextLayer *sunset_layer;
static TextLayer *error_layer;
static TextLayer *updated_layer;
static TextLayer *conditions_layer;
static TextLayer *day_layer;
static TextLayer *date_layer;
static TextLayer *time_layer;
static TextLayer *secs_layer;
static TextLayer *ampm_layer;
static TextLayer *moonPercent_layer;
static GFont *font_date;
static GFont *font_time;
static GFont *font_temp;
static GFont *font_cond;
static GFont *font_times;
static GFont *font_moon;

// Define layer rectangles (x, y, width, height)
GRect TEMP_RECT  = ConstantGRect(5, 0, 75, 26);
GRect BAR_RECT  = ConstantGRect(44, 0, 100, 26);
GRect UPDATED_RECT  = ConstantGRect(2, 53, 75, 20);
GRect COND_RECT  = ConstantGRect(44, 53, 99, 20);
GRect SRISE_RECT  = ConstantGRect(5, 145, 75, 20);
GRect SSET_RECT  = ConstantGRect(39, 145, 100, 20);
GRect DAY_RECT    = ConstantGRect(5, 115, 144, 33);
GRect TIME_RECT  = ConstantGRect(5, 66, 114, 60);
GRect SECS_RECT  = ConstantGRect(119, 70, 144-116, 28);
GRect AMPM_RECT  = ConstantGRect(116, 89, 144-116, 28);
GRect DATE_RECT  = ConstantGRect(1, 115, 144-1, 168-118);
GRect MOON_RECT    = ConstantGRect(0, 142, 144, 168-142);
GRect ERR_RECT    = ConstantGRect(0, 35, 144, 26);
GRect BATT_RECT  = ConstantGRect( 123,  94,  17,   9 );
GRect BT_RECT    = ConstantGRect(   4,  94,  17,   9 );

// Define placeholders for time and date
static char time_text[] = "00:00";
static char ampm_text[] = "AM";

// Work around to handle initial display for minutes to work when testing units_changed
static bool first_cycle = true;

/*
  Setup new TextLayer
*/
static TextLayer * setup_text_layer( GRect rect, GTextAlignment align , GFont font ) {
  TextLayer *newLayer = text_layer_create( rect );
  text_layer_set_text_color( newLayer, GColorWhite );
  text_layer_set_background_color( newLayer, GColorClear );
  text_layer_set_text_alignment( newLayer, align );
  text_layer_set_font( newLayer, font );

  return newLayer;
}
/*
  Handle bluetooth events

void handle_bluetooth( bool connected ) {
  if ( bluetooth_image != NULL ) {
    gbitmap_destroy( bluetooth_image );
  }

  if ( connected ) {
    bluetooth_image = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_BLUETOOTH );
  } else {
    bluetooth_image = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_NO_BLUETOOTH );
    vibes_short_pulse();
  }

  bitmap_layer_set_bitmap( bluetooth_layer, bluetooth_image );
}
*/

/*
  Handle battery events

void handle_battery( BatteryChargeState charge_state ) {
  if ( battery_image != NULL ) {
    gbitmap_destroy( battery_image );
  }

  if ( charge_state.is_charging ) {

    battery_image = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_BATT_CHARGING );

  } else {

    switch ( charge_state.charge_percent ) {
      case 0 ... 20:
        battery_image = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_BATT_000_020 );
        break;
      case 21 ... 40:
        battery_image = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_BATT_020_040 );
        break;
      case 41 ... 60:
        battery_image = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_BATT_040_060 );
        break;
      case 61 ... 80:
        battery_image = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_BATT_060_080 );
        break;
      case 81 ... 100:
        battery_image = gbitmap_create_with_resource( RESOURCE_ID_IMAGE_BATT_080_100 );
        break;
      }

  }

  bitmap_layer_set_bitmap( battery_layer, battery_image );
}
*/

/*
  Handle tick events
*/
void handle_tick( struct tm *tick_time, TimeUnits units_changed ) {
  // Handle day change
  if ( ( ( units_changed & DAY_UNIT ) == DAY_UNIT ) || first_cycle ) {
    // Update text layer for current day
    

  }

  // Handle time (hour and minute) change
  if ( ( ( units_changed & MINUTE_UNIT ) == MINUTE_UNIT ) || first_cycle ) {
    // Display hours (i.e. 18 or 06)
    strftime( time_text, sizeof( time_text ), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time );

    // Remove leading zero (only in 12h-mode)
    if ( !clock_is_24h_style() && (time_text[0] == '0') ) {
      memmove( time_text, &time_text[1], sizeof( time_text ) - 1 );
    }

    text_layer_set_text( time_layer, time_text );

    // Update AM/PM indicator (i.e. AM or PM or nothing when using 24-hour style)
    strftime( ampm_text, sizeof( ampm_text ), clock_is_24h_style() ? "" : "%p", tick_time );
    text_layer_set_text( ampm_layer, ampm_text );
  }

  // Clear
  if ( first_cycle ) {
    first_cycle = false;
  }
}


/*
  Initialization
*/
void handle_init( void ) {
  window = window_create();
  window_stack_push( window, true );
  Layer *window_layer = window_get_root_layer( window );

  // Background image
  background_image = gbitmap_create_with_resource( RESOURCE_ID_BG_IMAGE );
  background_layer = bitmap_layer_create( layer_get_frame( window_layer ) );
  bitmap_layer_set_bitmap( background_layer, background_image );
  layer_add_child( window_layer, bitmap_layer_get_layer( background_layer ) );


  // Load fonts
  font_date = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_ROBOTO_CONDENSED_20 ) );
  font_time = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_41 ) );
  font_temp = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_FUTURA_TEMP_18 ) );
  font_cond = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_FUTURA_CONDITIONS_12 ) );
  font_times = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_FUTURA_TIMES_14 ) );
  font_moon = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_MOON_PHASES_SUBSET_24 ) );

  // Setup time layer
  time_layer = setup_text_layer( TIME_RECT, GTextAlignmentCenter, font_time );
  layer_add_child( window_layer, text_layer_get_layer( time_layer ) );

  // Setup AM/PM name layer
  ampm_layer = setup_text_layer( AMPM_RECT, GTextAlignmentCenter, font_date );
  layer_add_child( window_layer, text_layer_get_layer( ampm_layer ) );

  // Setup temp layer
  temp_layer = setup_text_layer( TEMP_RECT, GTextAlignmentCenter, font_temp );
  layer_add_child( window_layer, text_layer_get_layer( temp_layer ) );

  // Setup conditions layer
  conditions_layer = setup_text_layer( COND_RECT, GTextAlignmentCenter, font_cond );
  layer_add_child( window_layer, text_layer_get_layer( conditions_layer ) );
	
  // Setup updated layer
  updated_layer = setup_text_layer( UPDATED_RECT, GTextAlignmentCenter, font_cond );
  layer_add_child( window_layer, text_layer_get_layer( updated_layer ) );

  // Setup barometric pressure layer
  bar_layer = setup_text_layer( BAR_RECT, GTextAlignmentCenter, font_cond );
  layer_add_child( window_layer, text_layer_get_layer( bar_layer ) );

  // Setup sunrise layer
  sunrise_layer = setup_text_layer( SRISE_RECT, GTextAlignmentCenter, font_cond );
  layer_add_child( window_layer, text_layer_get_layer( sunrise_layer ) );

  // Setup sunset layer
  sunset_layer = setup_text_layer( SSET_RECT, GTextAlignmentCenter, font_cond );
  layer_add_child( window_layer, text_layer_get_layer( sunset_layer ) );
	
  // Setup day layer
  day_layer = setup_text_layer( DAY_RECT, GTextAlignmentCenter, font_date );
  layer_add_child( window_layer, text_layer_get_layer( day_layer ) );

  // Setup seconds layer
  secs_layer = setup_text_layer( SECS_RECT, GTextAlignmentCenter, font_date );
  layer_add_child( window_layer, text_layer_get_layer( secs_layer ) );

  // Setup date layer
  date_layer = setup_text_layer( DATE_RECT, GTextAlignmentCenter, font_date );
  layer_add_child( window_layer, text_layer_get_layer( date_layer ) );

  // Setup moon layer
  moonPercent_layer = setup_text_layer( MOON_RECT, GTextAlignmentCenter, font_moon );
  layer_add_child( window_layer, text_layer_get_layer( moonPercent_layer ) );

  // Setup error layer
  error_layer = setup_text_layer( ERR_RECT, GTextAlignmentCenter, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD) );
  layer_add_child( window_layer, text_layer_get_layer( error_layer ) );

  // Subscribe to services
  tick_timer_service_subscribe( SECOND_UNIT, handle_tick );

  // Force update for battery and bluetooth status
  //handle_battery( battery_state_service_peek() );
  //handle_bluetooth( bluetooth_connection_service_peek() );

  //battery_state_service_subscribe( &handle_battery );
  //bluetooth_connection_service_subscribe( &handle_bluetooth );

  // Avoids a blank screen on watch start.
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);
  handle_tick( tick_time, SECOND_UNIT );
}

/*
  Destroy GBitmap and BitmapLayer
*/
void destroy_graphics( GBitmap *image, BitmapLayer *layer ) {
  layer_remove_from_parent( bitmap_layer_get_layer( layer ) );
  bitmap_layer_destroy( layer );
  if ( image != NULL ) {
    gbitmap_destroy( image );
  }
}


/*
  dealloc
*/
void handle_deinit( void ) {
  // Subscribe from services
  //tick_timer_service_unsubscribe();
  //battery_state_service_unsubscribe();

  // Destroy image objects
  destroy_graphics( background_image, background_layer );
  destroy_graphics( battery_image, battery_layer );
  destroy_graphics( bluetooth_image, bluetooth_layer );
  destroy_graphics( icon_image, icon_layer );

  // Destroy tex tobjects
  text_layer_destroy( temp_layer );
  text_layer_destroy( bar_layer );
  text_layer_destroy( sunrise_layer );
  text_layer_destroy( sunset_layer );
  text_layer_destroy( error_layer );
  text_layer_destroy( updated_layer );
  text_layer_destroy( conditions_layer );
  text_layer_destroy( day_layer );
  text_layer_destroy( date_layer );
  text_layer_destroy( time_layer );
  text_layer_destroy( secs_layer );
  text_layer_destroy( ampm_layer );
  text_layer_destroy( moonPercent_layer );
  
  // Destroy font objects
  fonts_unload_custom_font( font_date );
  fonts_unload_custom_font( font_time );
  fonts_unload_custom_font( font_temp );
  fonts_unload_custom_font( font_cond );
  fonts_unload_custom_font( font_times );
  fonts_unload_custom_font( font_moon );

  // Destroy window
  window_destroy( window );
}

/*
  Main process
*/
int main( void ) {
  handle_init();
  app_event_loop();
  handle_deinit();
}