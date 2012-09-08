#ifndef NIT_H
#define NIT_H 1

#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "ts_ctl.h"

typedef struct _NIThead {
	char table_id;
	int  section_syntax_indicator;
	int  reserved_future_use1;
	int  reserved1;
	int  section_length;
	int  network_id;
	int  reserved2;
	int  version_number;
	int  current_next_indicator;
	int  section_number;
	int  last_section_number;
	int  reserved_future_use2;
	int  network_descriptors_length;
	char network_descriptor[MAXSECLEN];
	int  reserved_future_use3;
	int  transport_stream_loop_length;

} NIThead;

typedef struct _NITbody {
	int  transport_stream_id;
	int  original_network_id;
	int  reserved_future_use;
	int  transport_descriptors_loop_length;
	char transport_descriptor[MAXSECLEN];
} NITbody;

typedef struct _ServiceListDescriptor {
	short	service_id;
	char	service_type;
}ServiceListDescriptor;

typedef struct _ServiceList {
	int   length;
	ServiceListDescriptor sld[256];
}ServiceList;

typedef struct _SatelliteDeliverySystemDescriptor {
	int		frequency;
	int		orbital_position;
	unsigned char	west_east_flag;
	unsigned char	polarisation;	
	unsigned char	modulation;
	int		symbol_rate;
	unsigned char	FEC_inner;
}SatelliteDeliverySystemDescriptor;

typedef struct _TerrestrialDeliverySystemDescriptor {
        short		area_code;
        unsigned char	guard_interval;
        unsigned char	transmission_mode;
        unsigned short  frequency;
}TerrestrialDeliverySystemDescriptor;

typedef struct _TSInformationTransmissionType {
        unsigned char	transmission_type_info;
        unsigned char	num_of_service;
        unsigned short  service_id[256];
}TSInformationTransmissionType;

typedef struct _TSInformationDescriptor {
        unsigned char	remote_control_key_id;
        unsigned char	length_of_ts_name;
        unsigned char	transmission_type_count;
        char            ts_name_char[MAXSECLEN];
        TSInformationTransmissionType    tstype[4];
}TSInformationDescriptor;

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

	int parseNIThead(unsigned char *data, NIThead *h);
	int parseNITbody(unsigned char *data, NITbody *b);
	void dumpNIT(unsigned char *data, SVT_CONTROL *top);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

