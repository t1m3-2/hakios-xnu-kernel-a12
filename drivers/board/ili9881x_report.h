/*
 * ILITEK ILI9881x demo-report decoder for the Galaxy A12.
 *
 * This is a bus-independent parser. The A12 vendor input tree describes a
 * 43-byte demo report with ten 4-byte contact records in its first 40 bytes.
 * Bytes 1-3 of each record carry packed 12-bit X/Y values; an all-0xff
 * coordinate triplet means the slot is empty. The controller's extended-axis report has
 * a separate 3-byte header which the caller must strip before decoding.
 * The report is scaled from the controller's 2048 x 2048 coordinate space to
 * the board's display size. The hardware SPI path and controller startup are
 * separate work and are not implemented here.
 *
 * Protocol references:
 *   Samsung android_kernel_samsung_a12, ili9881x/ili9881x_touch.c
 *   Samsung android_kernel_samsung_a12, ili9881x/ili9881x.h
 */
#ifndef HAKIOS_ILI9881X_REPORT_H
#define HAKIOS_ILI9881X_REPORT_H

#include <stdbool.h>
#include <stdint.h>

#define ILI9881X_CONTACTS       10u
#define ILI9881X_SLOT_BYTES     4u
#define ILI9881X_DEMO_BYTES     43u
#define ILI9881X_COORD_SCALE    2048u

struct ili9881x_frame {
	uint16_t active;                 /* one bit per contact slot */
	uint16_t x[ILI9881X_CONTACTS];
	uint16_t y[ILI9881X_CONTACTS];
};

/* Decode the 43-byte demo payload; leave `frame` untouched if the report is invalid. */
static inline bool
ili9881x_decode_demo(const uint8_t *packet, unsigned length, unsigned panel_x, unsigned panel_y,
	struct ili9881x_frame *frame)
{
	struct ili9881x_frame result = { 0 };

	if (packet == NULL || frame == NULL || length < ILI9881X_DEMO_BYTES || panel_x == 0 || panel_y == 0 ||
	    panel_x > UINT16_MAX || panel_y > UINT16_MAX) {
		return false;
	}

	for (unsigned id = 0; id < ILI9881X_CONTACTS; id++) {
		const uint8_t *slot = packet + id * ILI9881X_SLOT_BYTES;
		uint16_t raw_x, raw_y;

		if (slot[1] == 0xff && slot[2] == 0xff && slot[3] == 0xff) {
			continue;
		}

		raw_x = (uint16_t)(((slot[1] & 0xf0u) << 4) | slot[2]);
		raw_y = (uint16_t)(((slot[1] & 0x0fu) << 8) | slot[3]);
		if (raw_x >= ILI9881X_COORD_SCALE || raw_y >= ILI9881X_COORD_SCALE) {
			return false;
		}

		result.x[id] = (uint16_t)(((uint32_t)raw_x * panel_x) / ILI9881X_COORD_SCALE);
		result.y[id] = (uint16_t)(((uint32_t)raw_y * panel_y) / ILI9881X_COORD_SCALE);
		result.active |= (uint16_t)(1u << id);
	}

	*frame = result;
	return true;
}

#endif /* HAKIOS_ILI9881X_REPORT_H */
