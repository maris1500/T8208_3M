#ifndef AAA_24G_APP_H_
#define AAA_24G_APP_H_

#if (MODULE_VOLICE_ADJUST_ENABLE)
	typedef struct {
	unsigned char v_wheel_en : 1;
	unsigned char v_wheel_sta : 1;
	
	unsigned char reserve:6;
}volice_wheel_t;

extern volice_wheel_t  volice_wheel_status;

#endif

#endif /* AAA_24G_APP_H_*/
