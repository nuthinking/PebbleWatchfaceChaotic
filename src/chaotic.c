#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x29, 0x4C, 0x43, 0x00, 0xB5, 0xF9, 0x45, 0xA2, 0x98, 0x2D, 0x3E, 0xDA, 0xB6, 0x8C, 0xE0, 0xBF }
PBL_APP_INFO(MY_UUID,
             "Chaotic", "Nuthinking",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);
			 
#define H 168

Window window;

Layer graphicLayer;

TextLayer timeLayer;
PblTm currentTime;

int bandHeight = 0;

HeapBitmap greyTexture;

void update_graphic_layer(Layer *me, GContext* ctx)
{
	GRect destination = greyTexture.bmp.bounds;
	
	graphics_draw_bitmap_in_rect(ctx, &greyTexture.bmp, destination);
	
	destination.size.h = bandHeight;
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, destination, 0, GCornerNone);
}

void handle_tick(AppContextRef ctx, PebbleTickEvent *t)
{
	
	static char timeText[] = "00"; // Needs to be static because it's used by the system later.

	get_time(&currentTime);

	string_format_time(timeText, sizeof(timeText), "%T", &currentTime);

	text_layer_set_text(&timeLayer, timeText);
	
	bandHeight = (currentTime.tm_min * H) / 60;

	layer_mark_dirty(&graphicLayer);
}


void handle_init(AppContextRef ctx)
{
	resource_init_current_app(&APP_RESOURCES);
	heap_bitmap_init(&greyTexture, RESOURCE_ID_GREY_PATTERN);

	window_init(&window, "Chaotic Watch");
	window_stack_push(&window, true /* Animated */);

	window_set_background_color(&window, GColorBlack);
	
	layer_init(&graphicLayer, GRect(0,0, window.layer.frame.size.w, window.layer.frame.size.h));
	graphicLayer.update_proc = update_graphic_layer;
	layer_add_child(&window.layer, &graphicLayer);
  
	text_layer_init(&timeLayer, GRect(0, 56, 144 /* width */, 50 /* height */));
	text_layer_set_background_color(&timeLayer, GColorClear);
	text_layer_set_text_color(&timeLayer, GColorWhite);
	text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_alignment(&timeLayer, GTextAlignmentCenter);
	
	handle_tick(ctx, NULL);

	layer_add_child(&window.layer, &timeLayer.layer);
}

void handle_deinit(AppContextRef ctx)
{
  heap_bitmap_deinit(&greyTexture);
}



void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
	.deinit_handler = &handle_deinit,
    .tick_info = {
      .tick_handler = &handle_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
