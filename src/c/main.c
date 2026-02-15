#include <pebble.h>
#include "main.h"
#include "weekday.h"
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/fpath.h>
#include <pebble-fctx/ffont.h>
#include <pebble-scalable/pebble-scalable.h>

#define ROUND_VERTICAL_PADDING 15
#define ROTATION_SETTING_DEFAULT 0
#define ROTATION_SETTING_LEFT    0
#define ROTATION_SETTING_RIGHT   1

//Static and initial vars
static GFont //FontHour,
FontDate,
FontDate2,
FontBattery, 
//FontIcon, 
FontIcon2, 
FontIcon3
;

FFont* time_font;

static Window * s_window;

static Layer * s_canvas;
static Layer * s_canvas_bt_icon;
static Layer * s_canvas_qt_icon;

Layer * hour_area_layer;
Layer * minute_area_layer;

static int s_hours, s_minutes, s_weekday, s_day, s_month;


//////Init Configuration///
//Init Clay
ClaySettings settings;
// Initialize the default settings
static void prv_default_settings(){
settings.Back1Color = GColorBlack;
settings.FrameColor1 = GColorCobaltBlue;

  settings.Text1Color = GColorWhite;
  settings.Text2Color = GColorWhite;
  settings.Text3Color = GColorWhite;
  settings.Text4Color = GColorWhite;
  settings.Text5Color = GColorWhite;
  settings.Text6Color = GColorWhite;
  settings.HourColor = GColorWhite;
  settings.MinColor = GColorWhite;
  settings.AddZero12h = false;
  settings.RemoveZero24h = false;
  settings.BTVibeOn = true;
 }

bool BTOn=true;

//////End Configuration///
///////////////////////////

// Callback for js request
void request_watchjs(){
  //Starting loop at 0
//  s_loop = 0;
  // Begin dictionary
  DictionaryIterator * iter;
  app_message_outbox_begin( & iter);
  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);
  // Send the message!
  app_message_outbox_send();
}


///BT Connection
static void bluetooth_callback(bool connected){
  BTOn = connected;
}

static void bluetooth_vibe_icon (bool connected) {

  layer_set_hidden(s_canvas_bt_icon, connected);

  if(!connected && !quiet_time_is_active() && settings.BTVibeOn) {
    // Issue a vibrating alert
    vibes_double_pulse();
  }
}

static void quiet_time_icon () {
  if(!quiet_time_is_active()) {
  layer_set_hidden(s_canvas_qt_icon,true);
  }
}

void update_hour_area_layer(Layer *l, GContext* ctx7) {
  // check layer bounds
  GRect bounds = layer_get_unobstructed_bounds(l);

  #ifdef PBL_ROUND
    bounds = GRect(0, 0,180, 90);
  #else
     bounds = GRect(0,0,bounds.size.w,bounds.size.h/2);
  #endif

  // initialize FCTX, the fancy 3rd party drawing library that all the cool kids use
  FContext fctx;

  fctx_init_context(&fctx, ctx7);
  fctx_set_color_bias(&fctx, 0);
  fctx_set_fill_color(&fctx, settings.HourColor);


  // calculate font size
 #ifdef PBL_ROUND
  int font_size = bounds.size.h * 1.0; 
 #else
  int font_size = bounds.size.h * 1.1;
  #endif
  int h_adjust = 0;
  int v_adjust = 0;


    #ifdef PBL_COLOR
      fctx_enable_aa(true);
    #endif
  

  // if it's a round watch, EVERYTHING CHANGES
  #ifdef PBL_ROUND
    v_adjust = 0;

  #else
    // for rectangular watches, adjust X position based on sidebar position
   h_adjust = 0;
  #endif

  FPoint time_pos;
  fctx_begin_fill(&fctx);
  fctx_set_text_em_height(&fctx, time_font, font_size);

  int hourdraw;
  char hournow[8];
  if (clock_is_24h_style() && !settings.RemoveZero24h){
    hourdraw=s_hours;
    snprintf(hournow,sizeof(hournow),"%02d",hourdraw);
    }
  else if (clock_is_24h_style() && settings.RemoveZero24h){
    hourdraw=s_hours;
    snprintf(hournow,sizeof(hournow),"%d",hourdraw);
  }
  else if (!settings.AddZero12h){
    if (s_hours==0 || s_hours==12){
      hourdraw=12;
    }
    else hourdraw=s_hours%12;    
  snprintf(hournow, sizeof(hournow), "%d", hourdraw);
   }
  else{
    if (s_hours==0 || s_hours==12){
      hourdraw=12;
    }
    else hourdraw=s_hours%12;    
  snprintf(hournow, sizeof(hournow), "%02d", hourdraw);
  }
    
  // draw hours
  time_pos.x = INT_TO_FIXED(PBL_IF_ROUND_ELSE(90, bounds.size.w*2/3) + h_adjust);
  time_pos.y = INT_TO_FIXED(PBL_IF_ROUND_ELSE(48, bounds.size.h/2)  + v_adjust);

  fctx_set_offset(&fctx, time_pos);
  fctx_draw_string(&fctx, hournow, time_font, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentRight), FTextAnchorMiddle);
  fctx_end_fill(&fctx);

  fctx_deinit_context(&fctx);
}

void update_minute_area_layer(Layer *a, GContext* ctx8) {
  // check layer bounds
 GRect bounds = layer_get_unobstructed_bounds(a);

  #ifdef PBL_ROUND
    bounds = GRect(0, 90, 180, 90);
  #else
    bounds = GRect(0,bounds.size.h/2,bounds.size.w,bounds.size.h/2);
  #endif
  
  FContext fctx1;

  fctx_init_context(&fctx1, ctx8);
  fctx_set_color_bias(&fctx1, 0);
  fctx_set_fill_color(&fctx1, settings.MinColor);


  // calculate font size
#ifdef PBL_ROUND
  int font_size2 = bounds.size.h * 1.0; 
 #else
  int font_size2 = bounds.size.h * 1.1;
  #endif
  int h_adjust2 = 0;
  int v_adjust2 = 0;


    #ifdef PBL_COLOR
      fctx_enable_aa(true);
    #endif
 
  #ifdef PBL_ROUND
     v_adjust2 = 0;

  #else
     h_adjust2 = 0;
  #endif

  FPoint time_pos2;
  fctx_begin_fill(&fctx1);
  fctx_set_text_em_height(&fctx1, time_font, font_size2);

  int mindraw;
  mindraw = s_minutes;
  char minnow[8];
  snprintf(minnow, sizeof(minnow), "%02d", mindraw);
  
  //draw minutes
  time_pos2.x = INT_TO_FIXED(PBL_IF_ROUND_ELSE(90, bounds.size.w*2/3) + h_adjust2);
  time_pos2.y = INT_TO_FIXED(PBL_IF_ROUND_ELSE(132, bounds.size.h*3/2)  + v_adjust2);
  
  fctx_set_offset(&fctx1, time_pos2);
  fctx_draw_string(&fctx1, minnow, time_font, PBL_IF_ROUND_ELSE (GTextAlignmentCenter,GTextAlignmentRight), FTextAnchorMiddle);
  fctx_end_fill(&fctx1);

  fctx_deinit_context(&fctx1);
}

//Update main layer

static void layer_update_proc(Layer * layer1, GContext * ctx){
  // Create Rects
  GRect bounds = layer_get_bounds(layer1);
  
  GRect MediumBand =
    PBL_IF_ROUND_ELSE(
    GRect(8, 75, 36, 30),
    GRect(bounds.size.w * 102/144, bounds.size.h * 68/168, bounds.size.w/4, bounds.size.h * 32/168));
  
  
   //Build display
  graphics_context_set_fill_color(ctx, settings.Back1Color);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  graphics_context_set_fill_color(ctx, settings.FrameColor1);
  #ifdef PBL_PLATFORM_EMERY
  graphics_fill_rect(ctx, MediumBand,6,GCornersAll);
  #else
  graphics_fill_rect(ctx, MediumBand,4,GCornersAll);
  #endif
 
  
#ifdef PBL_PLATFORM_EMERY
  GRect ampmRect = 
      GRect(bounds.size.w * 100/144 + 2, bounds.size.h * -3/168, bounds.size.w * 40/144, bounds.size.h * 28/168);
        // GRect(100, -6, 40, 28)));
  
  GRect DateRect2 = 
        GRect(bounds.size.w * 100/144 + 2, (bounds.size.h * 65/168) + 6, bounds.size.w * 40/144, bounds.size.h * 40/168);
        // GRect(100, 65, 40, 40)

  GRect DateRect = 
      GRect(bounds.size.w * 100/144 + 1, bounds.size.h * 46/168 - 6, bounds.size.w * 40/144, bounds.size.h * 16/168);
    
  GRect MonthRect = 
     GRect(bounds.size.w * 100/144 + 1, bounds.size.h * 98/168 , bounds.size.w * 40/144, bounds.size.h * 16/168);
    
      
  GRect BatteryRect =
     GRect(bounds.size.w * 100/144 + 1, bounds.size.h * 148/168 - 6, bounds.size.w * 40/144, bounds.size.h * 20/168);
    

#else
  GRect ampmRect = 
      PBL_IF_ROUND_ELSE(
        GRect(136, 84, 40, 40),
        GRect(bounds.size.w * 100/144, bounds.size.h * -6/168, bounds.size.w * 40/144, bounds.size.h * 28/168));
        // GRect(100, -6, 40, 28)));
  
  GRect DateRect2 = 
      PBL_IF_ROUND_ELSE(
        GRect(6, 71, 40, 40),
        GRect(bounds.size.w * 100/144, bounds.size.h * 65/168, bounds.size.w * 40/144, bounds.size.h * 40/168));
        // GRect(100, 65, 40, 40)));

  GRect DateRect = 
      PBL_IF_ROUND_ELSE(
          GRect(6, 54, 40, 16),
          GRect(bounds.size.w * 100/144, bounds.size.h * 46/168, bounds.size.w * 40/144, bounds.size.h * 16/168));
          // GRect(100, 46, 40, 16)


  GRect MonthRect = 
    PBL_IF_ROUND_ELSE(
      GRect(6, 102, 40, 16),
      GRect(bounds.size.w * 100/144, bounds.size.h * 98/168, bounds.size.w * 40/144, bounds.size.h * 16/168));
      // GRect(100, 98, 40, 16)

      
  GRect BatteryRect =
   PBL_IF_ROUND_ELSE(
      GRect(6,120,40,20),
      GRect(bounds.size.w * 100/144, bounds.size.h * 148/168, bounds.size.w * 40/144, bounds.size.h * 20/168));
      // GRect(100,148, 40, 20)

#endif


  

  
    
  //Date
  // Local language
  const char * sys_locale = i18n_get_system_locale();
  char datedraw[10];
  fetchwday(s_weekday, sys_locale, datedraw);
  char datenow[10];
  snprintf(datenow, sizeof(datenow), "%s", datedraw);
 
  char monthdraw[10];
  fetchmonth (s_month, sys_locale, monthdraw);
  char monthnow[10];
  snprintf(monthnow, sizeof(monthnow), "%s", monthdraw);
    
  int daydraw;
  daydraw = s_day;
  char daynow[8];
  snprintf(daynow, sizeof(daynow), "%02d", daydraw);
  
  
  // Concatenate date
  //strcat(/*datenow,*/ convertday);
  
  
  //Battery
  int battery_level = battery_state_service_peek().charge_percent;
  char battperc[20];
  snprintf(battperc, sizeof(battperc), "%d", battery_level);
  strcat(battperc, "%");
 
  // Draw AM PM 24H
  char ampm[5];
  if (clock_is_24h_style()){
    snprintf(ampm, sizeof(ampm), " ");
  }
  else if (s_hours<12){
    snprintf(ampm, sizeof(ampm), "AM");    
  }
  else {
    snprintf(ampm, sizeof(ampm), "PM"); 
  };
 
  //dates band
  graphics_context_set_text_color(ctx, settings.Text2Color);
  graphics_draw_text(ctx, battperc, FontBattery, BatteryRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);
  
  graphics_context_set_text_color(ctx, settings.Text1Color);
  graphics_draw_text(ctx, ampm, FontDate2, ampmRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter), NULL);
  
  graphics_context_set_text_color(ctx, settings.Text3Color);
  graphics_draw_text(ctx, datenow, FontDate, DateRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
   
  graphics_context_set_text_color(ctx, settings.Text6Color);
  graphics_draw_text(ctx, daynow, FontDate2, DateRect2, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
  
  graphics_context_set_text_color(ctx, settings.Text5Color);
  graphics_draw_text(ctx, monthnow, FontDate, MonthRect, GTextOverflowModeWordWrap, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
}
  

static void layer_update_proc_bt(Layer * layer3, GContext * ctx3){
   // Create Rects
  GRect bounds = layer_get_bounds(layer3);

  #ifdef PBL_PLATFORM_EMERY
   GRect BTIconRect = 
      GRect(bounds.size.w * 100/144 + 1, bounds.size.h * 118/168 + 3, bounds.size.w * 40/144, bounds.size.h * 20/168);
  #else
  GRect BTIconRect = 
    (PBL_IF_ROUND_ELSE(
      GRect(6,40,40,20),
      GRect(bounds.size.w * 100/144, bounds.size.h * 118/168, bounds.size.w * 40/144, bounds.size.h * 20/168)
      // GRect(100,118,40,20)
    ));
  #endif

 bluetooth_callback(connection_service_peek_pebble_app_connection());
  
 graphics_context_set_text_color(ctx3, settings.Text4Color);
 graphics_draw_text(ctx3, "z", FontIcon2, BTIconRect, GTextOverflowModeFill,PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
  }

static void layer_update_proc_qt(Layer * layer4, GContext * ctx4){
   // Create Rects

 GRect bounds = layer_get_bounds(layer4);
 
 #ifdef PBL_PLATFORM_EMERY
   GRect QTIconRect = 
      GRect(bounds.size.w * 100/144 + 1, bounds.size.h * 136/168 - 1, bounds.size.w * 40/144, bounds.size.h * 20/168);
 #else

 GRect QTIconRect = 
    (PBL_IF_ROUND_ELSE(
      GRect(8,136,40,20),
      GRect(bounds.size.w * 100/144, bounds.size.h * 136/168, bounds.size.w * 40/144, bounds.size.h * 20/168)
      // GRect(100,136,40,20)
    ));
 #endif
  
 quiet_time_icon();
    
 graphics_context_set_text_color(ctx4, settings.Text4Color);
 graphics_draw_text(ctx4, "\U0000E061", FontIcon2, QTIconRect, GTextOverflowModeFill,PBL_IF_ROUND_ELSE(GTextAlignmentCenter,GTextAlignmentCenter), NULL);
  
}


/////////////////////////////////////////
////Init: Handle Settings////
/////////////////////////////////////////
// Read settings from persistent storage
static void prv_load_settings(){
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, & settings, sizeof(settings));
}
// Save the settings to persistent storage
static void prv_save_settings(){
  persist_write_data(SETTINGS_KEY, & settings, sizeof(settings));
 }

// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator * iter, void * context){

  // Background Color
  Tuple * bg1_color_t = dict_find(iter, MESSAGE_KEY_Back1Color);
  if (bg1_color_t){
    settings.Back1Color = GColorFromHEX(bg1_color_t-> value -> int32);
  } 
  Tuple * fr1_color_t = dict_find(iter, MESSAGE_KEY_FrameColor1);
  if (fr1_color_t){
    settings.FrameColor1 = GColorFromHEX(fr1_color_t-> value -> int32);
  }

  Tuple * tx1_color_t = dict_find(iter, MESSAGE_KEY_Text1Color);
  if (tx1_color_t){
    settings.Text1Color = GColorFromHEX(tx1_color_t-> value -> int32);
  }
  
  ///////////////////////////////
  Tuple * hr_color_t = dict_find(iter, MESSAGE_KEY_HourColor);
  if (hr_color_t){
    settings.HourColor = GColorFromHEX(hr_color_t-> value -> int32);
  }
 
  Tuple * min_color_t = dict_find(iter, MESSAGE_KEY_MinColor);
  if (min_color_t){
    settings.MinColor = GColorFromHEX(min_color_t-> value -> int32);
  }
 ///////////////////////////////
  Tuple * tx2_color_t = dict_find(iter, MESSAGE_KEY_Text2Color);
  if (tx2_color_t){
    settings.Text2Color = GColorFromHEX(tx2_color_t-> value -> int32);
  }
 
   Tuple * tx3_color_t = dict_find(iter, MESSAGE_KEY_Text3Color);
  if (tx3_color_t){
    settings.Text3Color = GColorFromHEX(tx3_color_t-> value -> int32);
  }
  
  Tuple * tx4_color_t = dict_find(iter,MESSAGE_KEY_Text4Color);
  if (tx4_color_t){
    settings.Text4Color = GColorFromHEX(tx4_color_t-> value -> int32);
    }
 
  Tuple * tx5_color_t = dict_find(iter,MESSAGE_KEY_Text5Color);
  if (tx5_color_t){
    settings.Text5Color = GColorFromHEX(tx5_color_t-> value -> int32);
    }
 
   Tuple * tx6_color_t = dict_find(iter,MESSAGE_KEY_Text6Color);
  if (tx6_color_t){
    settings.Text6Color = GColorFromHEX(tx6_color_t-> value -> int32);
    }

  Tuple * addzero12_t = dict_find(iter, MESSAGE_KEY_AddZero12h);
  Tuple * remzero24_t = dict_find(iter, MESSAGE_KEY_RemoveZero24h);
  Tuple * vibe_t = dict_find(iter, MESSAGE_KEY_BTVibeOn);

    if (addzero12_t) {
      settings.AddZero12h = addzero12_t->value->int32 == 1;
    //  layer_mark_dirty(g_layer);
    }

    if (remzero24_t) {
      settings.RemoveZero24h = remzero24_t->value->int32 == 1;
    //  layer_mark_dirty(g_layer);
    }

    if (vibe_t) {
      settings.BTVibeOn = vibe_t->value->int32 == 1;
    //  layer_mark_dirty(s_canvas_bt_icon);
    }
  
  //End data gathered
  // Get display handlers

/*  Tuple * disntheme_t = dict_find(iter, MESSAGE_KEY_NightTheme);
  if (disntheme_t){
    if (disntheme_t -> value -> int32 == 0){
      settings.NightTheme = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "NTHeme off");
    } else settings.NightTheme = true;
  } */
  
  
  //Update colors & hour settings
  layer_mark_dirty(s_canvas);
  layer_mark_dirty(s_canvas_bt_icon);
  layer_mark_dirty(s_canvas_qt_icon);
  layer_mark_dirty(hour_area_layer);
  layer_mark_dirty(minute_area_layer);
  // Save the new settings to persistent storage


  prv_save_settings();
}



//Load main layer
static void window_load(Window * window){
  Layer * window_layer = window_get_root_layer(window);
  GRect bounds4 = layer_get_bounds(window_layer);
  
  s_canvas = layer_create(bounds4);
    layer_set_update_proc(s_canvas, layer_update_proc);
    layer_add_child(window_layer, s_canvas);
  
  s_canvas_bt_icon = layer_create(bounds4);
    layer_set_update_proc (s_canvas_bt_icon, layer_update_proc_bt);
    layer_add_child(window_layer, s_canvas_bt_icon);
  
  s_canvas_qt_icon = layer_create(bounds4);
    layer_set_update_proc (s_canvas_qt_icon, layer_update_proc_qt);
    layer_add_child(window_layer, s_canvas_qt_icon);
    
  hour_area_layer = layer_create(bounds4);
    layer_add_child(window_get_root_layer(s_window), hour_area_layer);
    layer_set_update_proc(hour_area_layer, update_hour_area_layer);
  
  minute_area_layer = layer_create(bounds4);
    layer_add_child(window_get_root_layer(s_window), minute_area_layer);
    layer_set_update_proc(minute_area_layer, update_minute_area_layer);

}


static void window_unload(Window * window){
  layer_destroy(s_canvas);
  layer_destroy(hour_area_layer);
  layer_destroy(minute_area_layer);
  layer_destroy(s_canvas_bt_icon);
  layer_destroy(s_canvas_qt_icon);
  window_destroy(s_window);
//  fonts_unload_custom_font(FontIcon);
  fonts_unload_custom_font(FontIcon2);
  fonts_unload_custom_font(FontIcon3);
  ffont_destroy(time_font);
}

void main_window_push(){
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
  }

void main_window_update(int hours, int minutes, int weekday, int day, int month){
  s_hours = hours;
  s_minutes = minutes;
  s_day = day;
  s_weekday = weekday;
  s_month = month;
  
  layer_mark_dirty(s_canvas);
  layer_mark_dirty(s_canvas_bt_icon);
  layer_mark_dirty(s_canvas_qt_icon);
  layer_mark_dirty(hour_area_layer);
  layer_mark_dirty(minute_area_layer);
 
 /////// update_rotation(); ///use this one
 
}

static void tick_handler(struct tm * time_now, TimeUnits changed){

  main_window_update(time_now -> tm_hour, time_now -> tm_min, time_now -> tm_wday, time_now -> tm_mday, time_now -> tm_mon);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick at %d", time_now -> tm_min);
    
  }


static void init(){
  
  prv_load_settings();
  // Listen for AppMessages
   
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  s_hours=t->tm_hour;
  s_minutes=t->tm_min;
  s_day=t->tm_mday;
  s_weekday=t->tm_wday;
  s_month=t->tm_mon;
  //Register and open
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);
  // Load Fonts
 
  time_font =  ffont_create_from_resource(RESOURCE_ID_OSWALD_FFONT);
  
 #ifdef PBL_PLATFORM_EMERY
  FontDate = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  FontDate2 = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  FontBattery= fonts_get_system_font(PBL_IF_ROUND_ELSE(FONT_KEY_GOTHIC_14,FONT_KEY_GOTHIC_28_BOLD));
  FontIcon2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_18));
  FontIcon3 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_20));
 #else
  FontDate = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  FontDate2 = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  FontBattery= fonts_get_system_font(PBL_IF_ROUND_ELSE(FONT_KEY_GOTHIC_14,FONT_KEY_GOTHIC_18_BOLD));
  FontIcon2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_16));
  FontIcon3 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DRIPICONS_18));
#endif
 
  main_window_push();
  // Register with Event Services
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
 
  connection_service_subscribe((ConnectionHandlers){
    .pebble_app_connection_handler = bluetooth_vibe_icon
  });
  bluetooth_vibe_icon(connection_service_peek_pebble_app_connection());
 // handle_battery(battery_state_service_peek());
}
static void deinit(){
  tick_timer_service_unsubscribe();
  app_message_deregister_callbacks(); //Destroy the callbacks for clean up
//   tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
 
}
int main(){
  init();
  app_event_loop();
  deinit();
}