#include <pebble.h>

#define BALANCE 0
#define BILL_COUNT 1

Window* window;


TextLayer *day_layer;
TextLayer *current_balance_text;
TextLayer *total_balance_text;

static struct tm *tn;
TextLayer *text_time_layer;

Layer *bars_layer;
Layer *main_layer;
Layer *window_layer;

static double accounts_grab =0; //the amount in the account currently
bool js_updated = false;  

void bars_update_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  //Progress Bar
  graphics_draw_line(ctx, GPoint(5, 147),     GPoint(144-7, 147));  //long line, bottom
  graphics_draw_line(ctx, GPoint(5, 147+6),   GPoint(144-7, 147+6)); //short line, right
  graphics_draw_line(ctx, GPoint(4, 148),     GPoint(4, 148+4)); //short line, left
  graphics_draw_line(ctx, GPoint(144-6, 148), GPoint(144-6, 148+4)); //long line, top
  
}



void progress_update_callback(Layer *me, GContext* ctx) {
  (void)me;
  time_t now = time(NULL);
  tn = localtime(&now);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  

  //Calculate the percentage of alotted funds based on current spending amount
  double capital_one_grab = 50.00;
  int totalBalance = accounts_grab*100;
  int spentBalance = capital_one_grab*100;
  int bar_percent; 
  
  //TODO: Add something that waits for js to update
  
  //Manual limit
  double set_limit = 60; //TODO: set by settings  
  int budget_left = set_limit*100 - spentBalance;
  
  
   if(js_updated){
    //turns how much total left into currency form
    static char t_dollar[20];
    snprintf(t_dollar,sizeof(t_dollar),"%d",totalBalance/100);
    static char t_cent[10];
    snprintf(t_cent, sizeof(t_cent),"%02d",totalBalance%100);
    static char total_balance[20];
    snprintf(total_balance, sizeof(total_balance), "%s%s%s%s", "Balance: $", t_dollar, ".", t_cent);
    text_layer_set_text(total_balance_text, total_balance);
  }else{
    //TODO maybe make a smaller font for this part
    text_layer_set_text(total_balance_text, "Loading balance...");
  }
  
  //turns how much total left into currency form
  static char t_dollar[20];
  snprintf(t_dollar,sizeof(t_dollar),"%d",totalBalance/100);
  static char t_cent[10];
  snprintf(t_cent, sizeof(t_cent),"%02d",totalBalance%100);
  static char total_balance[20];
  snprintf(total_balance, sizeof(total_balance), "%s%s%s%s", "Total: $", t_dollar, ".", t_cent);
  text_layer_set_text(total_balance_text, total_balance);
  
  
  
  //sets percentage for progress bar
  double percentageLeft = ((double)capital_one_grab/set_limit); 
  percentageLeft = ((double)134*percentageLeft); //Percent of bar based on % of balance
  bar_percent = (int)percentageLeft;
  //fills the amount of progress for bar
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(5, 148, bar_percent, 5), 0, GCornerNone);

}



void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {

  static char time_text[] = "00:00";
  char *time_format;

  //Time
  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

 
  strftime(time_text, sizeof(time_text), time_format, tick_time);

  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(text_time_layer, time_text);

  
  //Redraw layer
  layer_mark_dirty(main_layer);
}


static void handle_init(void) {

  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);

  window_layer = window_get_root_layer(window);
  
  //Current Account Balance text
  current_balance_text = text_layer_create(GRect(8, 20, 144-16, 20+28)); //GRect(8, 49, 144-16, 49+28));   
  text_layer_set_text_color(current_balance_text, GColorWhite);
  text_layer_set_background_color(current_balance_text, GColorClear);
  text_layer_set_font(current_balance_text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(current_balance_text));
  
  //total account balance text
  total_balance_text = text_layer_create(GRect(8,95,144,95+22));  
  text_layer_set_text_color(total_balance_text,GColorWhite);
  text_layer_set_background_color(total_balance_text,GColorClear);
  text_layer_set_font(total_balance_text, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer,text_layer_get_layer(total_balance_text));


  //Clock text
  text_time_layer = text_layer_create(GRect(8, 60, 144, 60+20));  
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  
  text_layer_set_text(text_time_layer, "00:00");
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));


  bars_layer = layer_create(layer_get_frame(window_layer));
  layer_set_update_proc(bars_layer, bars_update_callback);
  layer_add_child(window_layer, bars_layer);

  main_layer = layer_create(layer_get_frame(window_layer));
  layer_set_update_proc(main_layer, progress_update_callback);
  layer_add_child(window_layer, main_layer);
  
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}


static void handle_destroy(void) {
  //Texts

  //Current Balance
  text_layer_destroy(current_balance_text);

  //Clock text
  text_layer_destroy(text_time_layer);
  
  layer_destroy(bars_layer);
  
  layer_destroy(main_layer);
  tick_timer_service_unsubscribe();
  layer_destroy(window_layer);
  
  window_destroy(window);
}


 typedef struct {
  char payee[32];
  int amount;
  int month;
  int day;
  int year;
} Bill;

Bill *bills;
int numBills = 0;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  static char balance_buffer[8];
  static char billing_length_buffer[8];
  static char payee[32];

  // start reading default items
  Tuple *tuple;
  
  // balance
  tuple = dict_find(iterator, 0);
  int accountBalance = (int) tuple->value->int32; 
  accounts_grab = accountBalance;
  printf("balance tuple value = %d", accountBalance);
  
  // num bills
  tuple = dict_find(iterator, 1);
  numBills = (int) tuple->value->int32;
  printf("bill count tuple value = %d", numBills);
  bills = (Bill *) calloc(numBills, sizeof(Bill));
  
  for (int i = 0; i < numBills; i++) {
    printf("Bill number: %d\n", i);
    
    int startingPoint = (i * 5) + 2;
    
    // payee
    tuple = dict_find(iterator, startingPoint);
    snprintf(bills[i].payee, sizeof(bills[i].payee), "%s", tuple->value->cstring);
    printf("Payee = %s\n", bills[i].payee);
    
    // payment amount
    tuple = dict_find(iterator, startingPoint + 1);
    bills[i].amount = (int) tuple->value->int32;
    printf("Amount = %d\n", bills[i].amount);

    // month
    tuple = dict_find(iterator, startingPoint + 2);
    bills[i].month = (int) tuple->value->int32;
    printf("Month = %d\n", bills[i].month);
    
    // day
    tuple = dict_find(iterator, startingPoint + 3);
    bills[i].day = (int) tuple->value->int32;
    printf("Day = %d\n", bills[i].day);
    
    // year 
    tuple = dict_find(iterator, startingPoint + 4);
    bills[i].year = (int) tuple->value->int32;
    printf("Year = %d\n", bills[i].year);
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

int main(void) {
   handle_init();

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  

  app_event_loop();
  
   handle_destroy();
}