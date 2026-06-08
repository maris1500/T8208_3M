#ifndef AAA_24G_APP_H_
#define AAA_24G_APP_H_

typedef struct {
	unsigned char mouse_send_need : 2;
	unsigned char dongle_id_valid : 1;
	unsigned char g24_pair_success : 1;
	unsigned char dongle_id_need_save : 1;
	unsigned char g24_need_suspend : 1;
	
	unsigned char reserve:2;
}g24_status_t;

extern g24_status_t  g24_flags;

#if (MODULE_VOLICE_ADJUST_ENABLE)
	typedef struct {
	unsigned char v_wheel_en : 1;
	unsigned char v_wheel_sta : 1;
	
	unsigned char reserve:6;
}volice_wheel_t;

extern volice_wheel_t  volice_wheel_status;

#endif

#endif /* AAA_24G_APP_H_*/
