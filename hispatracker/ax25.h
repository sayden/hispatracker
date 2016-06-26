#ifndef __AX25_H__
#define __AX25_H__

struct s_address {
	char callsign[7];
	unsigned char ssid;
};

void ax25_send_header(const struct s_address *addresses, int num_addresses);
void ax25_send_byte(unsigned char byte);
void ax25_send_string(const char *string);
void ax25_send_footer();
void ax25_flush_frame();

#endif
