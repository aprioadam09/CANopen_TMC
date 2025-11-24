#include <lely/co/sdev.h>

#define CO_SDEV_STRING(s)	s

const struct co_sdev slave_sdev = {
	.id = 0x02,
	.name = NULL,
	.vendor_name = CO_SDEV_STRING("CANopen_TMC_Driver"),
	.vendor_id = 0x00000360,
	.product_name = CO_SDEV_STRING("TMC5160 Slave"),
	.product_code = 0x00000000,
	.revision = 0x00000000,
	.order_code = NULL,
	.baud = 0
		| CO_BAUD_125,
	.rate = 125,
	.lss = 0,
	.dummy = 0x000000fe,
	.nobj = 30,
	.objs = (const struct co_sobj[]){{
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Device type"),
#endif
		.idx = 0x1000,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Device type"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RO,
			.pdo_mapping = 0,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Error register"),
#endif
		.idx = 0x1001,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Error register"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = CO_UNSIGNED8_MIN },
			.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = CO_UNSIGNED8_MIN },
#endif
			.val = { .u8 = CO_UNSIGNED8_MIN },
			.access = CO_ACCESS_RO,
			.pdo_mapping = 0,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("COB-ID time stamp object"),
#endif
		.idx = 0x1012,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("COB-ID time stamp object"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = 0x80000100lu },
#endif
			.val = { .u32 = 0x80000100lu },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Producer heartbeat time"),
#endif
		.idx = 0x1017,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Producer heartbeat time"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED16,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u16 = CO_UNSIGNED16_MIN },
			.max = { .u16 = CO_UNSIGNED16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u16 = 0x03e8u },
#endif
			.val = { .u16 = 0x03e8u },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Identity object"),
#endif
		.idx = 0x1018,
		.code = CO_OBJECT_RECORD,
		.nsub = 5,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Highest sub-index supported"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = CO_UNSIGNED8_MIN },
			.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = 0x04 },
#endif
			.val = { .u8 = 0x04 },
			.access = CO_ACCESS_CONST,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Vendor-ID"),
#endif
			.subidx = 0x01,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = 0x00000360lu },
#endif
			.val = { .u32 = 0x00000360lu },
			.access = CO_ACCESS_RO,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Product code"),
#endif
			.subidx = 0x02,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RO,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Revision number"),
#endif
			.subidx = 0x03,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RO,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Serial number"),
#endif
			.subidx = 0x04,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RO,
			.pdo_mapping = 0,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Receive PDO 1 Communication Parameter"),
#endif
		.idx = 0x1400,
		.code = CO_OBJECT_RECORD,
		.nsub = 3,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Number of Entries"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = 0x02 },
			.max = { .u8 = 0x02 },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = 0x02 },
#endif
			.val = { .u8 = 0x02 },
			.access = CO_ACCESS_RO,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("COB-ID use by RPDO 1"),
#endif
			.subidx = 0x01,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = 0x00000202lu },
#endif
			.val = { .u32 = 0x00000202lu },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
				| CO_OBJ_FLAGS_DEF_NODEID
				| CO_OBJ_FLAGS_VAL_NODEID
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Transmission type RPDO 1"),
#endif
			.subidx = 0x02,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = CO_UNSIGNED8_MIN },
			.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = CO_UNSIGNED8_MAX },
#endif
			.val = { .u8 = CO_UNSIGNED8_MAX },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Receive PDO 2 Communication Parameter"),
#endif
		.idx = 0x1401,
		.code = CO_OBJECT_RECORD,
		.nsub = 3,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Number of Entries"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = 0x02 },
			.max = { .u8 = 0x02 },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = 0x02 },
#endif
			.val = { .u8 = 0x02 },
			.access = CO_ACCESS_RO,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("COB-ID use by RPDO 2"),
#endif
			.subidx = 0x01,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = 0x00000302lu },
#endif
			.val = { .u32 = 0x00000302lu },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
				| CO_OBJ_FLAGS_DEF_NODEID
				| CO_OBJ_FLAGS_VAL_NODEID
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Transmission type RPDO 2"),
#endif
			.subidx = 0x02,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = CO_UNSIGNED8_MIN },
			.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = CO_UNSIGNED8_MAX },
#endif
			.val = { .u8 = CO_UNSIGNED8_MAX },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Receive PDO 3 Communication Parameter"),
#endif
		.idx = 0x1402,
		.code = CO_OBJECT_RECORD,
		.nsub = 3,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Number of Entries"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = 0x02 },
			.max = { .u8 = 0x02 },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = 0x02 },
#endif
			.val = { .u8 = 0x02 },
			.access = CO_ACCESS_RO,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("COB-ID use by RPDO 3"),
#endif
			.subidx = 0x01,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = 0x00000402lu },
#endif
			.val = { .u32 = 0x00000402lu },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
				| CO_OBJ_FLAGS_DEF_NODEID
				| CO_OBJ_FLAGS_VAL_NODEID
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Transmission type RPDO 3"),
#endif
			.subidx = 0x02,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = CO_UNSIGNED8_MIN },
			.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = CO_UNSIGNED8_MAX },
#endif
			.val = { .u8 = CO_UNSIGNED8_MAX },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Receive PDO 1 mapping parameter"),
#endif
		.idx = 0x1600,
		.code = CO_OBJECT_RECORD,
		.nsub = 9,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Number of mapped objects RPDO 1"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = CO_UNSIGNED8_MIN },
			.max = { .u8 = 0x08 },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = 0x01 },
#endif
			.val = { .u8 = 0x01 },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 1 mapping information 1"),
#endif
			.subidx = 0x01,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = 0x60400010lu },
#endif
			.val = { .u32 = 0x60400010lu },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 1 mapping information 2"),
#endif
			.subidx = 0x02,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 1 mapping information 3"),
#endif
			.subidx = 0x03,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 1 mapping information 4"),
#endif
			.subidx = 0x04,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 1 mapping information 5"),
#endif
			.subidx = 0x05,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 1 mapping information 6"),
#endif
			.subidx = 0x06,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 1 mapping information 7"),
#endif
			.subidx = 0x07,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 1 mapping information 8"),
#endif
			.subidx = 0x08,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Receive PDO 2 mapping parameter"),
#endif
		.idx = 0x1601,
		.code = CO_OBJECT_RECORD,
		.nsub = 9,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Number of mapped objects RPDO 2"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = CO_UNSIGNED8_MIN },
			.max = { .u8 = 0x08 },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = 0x02 },
#endif
			.val = { .u8 = 0x02 },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 2 mapping information 1"),
#endif
			.subidx = 0x01,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = 0x60400010lu },
#endif
			.val = { .u32 = 0x60400010lu },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 2 mapping information 2"),
#endif
			.subidx = 0x02,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = 0x60600008lu },
#endif
			.val = { .u32 = 0x60600008lu },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 2 mapping information 3"),
#endif
			.subidx = 0x03,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 2 mapping information 4"),
#endif
			.subidx = 0x04,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 2 mapping information 5"),
#endif
			.subidx = 0x05,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 2 mapping information 6"),
#endif
			.subidx = 0x06,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 2 mapping information 7"),
#endif
			.subidx = 0x07,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 2 mapping information 8"),
#endif
			.subidx = 0x08,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Receive PDO 3 mapping parameter"),
#endif
		.idx = 0x1602,
		.code = CO_OBJECT_RECORD,
		.nsub = 9,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Number of mapped objects RPDO 3"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = CO_UNSIGNED8_MIN },
			.max = { .u8 = 0x08 },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = 0x02 },
#endif
			.val = { .u8 = 0x02 },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 3 mapping information 1"),
#endif
			.subidx = 0x01,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = 0x60400010lu },
#endif
			.val = { .u32 = 0x60400010lu },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 3 mapping information 2"),
#endif
			.subidx = 0x02,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = 0x607a0020lu },
#endif
			.val = { .u32 = 0x607a0020lu },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 3 mapping information 3"),
#endif
			.subidx = 0x03,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 3 mapping information 4"),
#endif
			.subidx = 0x04,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 3 mapping information 5"),
#endif
			.subidx = 0x05,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 3 mapping information 6"),
#endif
			.subidx = 0x06,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 3 mapping information 7"),
#endif
			.subidx = 0x07,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("RPDO 3 mapping information 8"),
#endif
			.subidx = 0x08,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Transmit PDO 1 communication parameters"),
#endif
		.idx = 0x1800,
		.code = CO_OBJECT_RECORD,
		.nsub = 3,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Number of Entries"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = 0x02 },
			.max = { .u8 = 0x02 },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = 0x02 },
#endif
			.val = { .u8 = 0x02 },
			.access = CO_ACCESS_RO,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("COB-ID use by TPDO 1"),
#endif
			.subidx = 0x01,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = 0x00000182lu },
#endif
			.val = { .u32 = 0x00000182lu },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
				| CO_OBJ_FLAGS_DEF_NODEID
				| CO_OBJ_FLAGS_VAL_NODEID
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Transmission type TPDO 1"),
#endif
			.subidx = 0x02,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = CO_UNSIGNED8_MIN },
			.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = CO_UNSIGNED8_MAX },
#endif
			.val = { .u8 = CO_UNSIGNED8_MAX },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Transmit PDO 2 communication parameters"),
#endif
		.idx = 0x1801,
		.code = CO_OBJECT_RECORD,
		.nsub = 3,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Number of Entries"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = 0x02 },
			.max = { .u8 = 0x02 },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = 0x02 },
#endif
			.val = { .u8 = 0x02 },
			.access = CO_ACCESS_RO,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("COB-ID use by TPDO 2"),
#endif
			.subidx = 0x01,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = 0x00000282lu },
#endif
			.val = { .u32 = 0x00000282lu },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
				| CO_OBJ_FLAGS_DEF_NODEID
				| CO_OBJ_FLAGS_VAL_NODEID
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Transmission type TPDO 2"),
#endif
			.subidx = 0x02,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = CO_UNSIGNED8_MIN },
			.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = CO_UNSIGNED8_MAX },
#endif
			.val = { .u8 = CO_UNSIGNED8_MAX },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Transmit PDO 1 mapping parameter"),
#endif
		.idx = 0x1a00,
		.code = CO_OBJECT_RECORD,
		.nsub = 9,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Number of mapped objects TPDO 1"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = CO_UNSIGNED8_MIN },
			.max = { .u8 = 0x08 },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = 0x01 },
#endif
			.val = { .u8 = 0x01 },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 1 mapping information 1"),
#endif
			.subidx = 0x01,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = 0x60410010lu },
#endif
			.val = { .u32 = 0x60410010lu },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 1 mapping information 2"),
#endif
			.subidx = 0x02,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 1 mapping information 3"),
#endif
			.subidx = 0x03,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 1 mapping information 4"),
#endif
			.subidx = 0x04,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 1 mapping information 5"),
#endif
			.subidx = 0x05,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 1 mapping information 6"),
#endif
			.subidx = 0x06,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 1 mapping information 7"),
#endif
			.subidx = 0x07,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 1 mapping information 8"),
#endif
			.subidx = 0x08,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Transmit PDO 2 mapping parameter"),
#endif
		.idx = 0x1a01,
		.code = CO_OBJECT_RECORD,
		.nsub = 9,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Number of mapped objects TPDO 2"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = CO_UNSIGNED8_MIN },
			.max = { .u8 = 0x08 },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = 0x02 },
#endif
			.val = { .u8 = 0x02 },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 2 mapping information 1"),
#endif
			.subidx = 0x01,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = 0x60410010lu },
#endif
			.val = { .u32 = 0x60410010lu },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 3 mapping information 2"),
#endif
			.subidx = 0x02,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = 0x60640020lu },
#endif
			.val = { .u32 = 0x60640020lu },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 2 mapping information 3"),
#endif
			.subidx = 0x03,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 2 mapping information 4"),
#endif
			.subidx = 0x04,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 2 mapping information 5"),
#endif
			.subidx = 0x05,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 2 mapping information 6"),
#endif
			.subidx = 0x06,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 2 mapping information 7"),
#endif
			.subidx = 0x07,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("TPDO 2 mapping information 8"),
#endif
			.subidx = 0x08,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("NMT startup"),
#endif
		.idx = 0x1f80,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("NMT startup"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = 0x00000004lu },
			.access = CO_ACCESS_RW,
			.pdo_mapping = 0,
			.flags = 0
				| CO_OBJ_FLAGS_PARAMETER_VALUE
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Control word"),
#endif
		.idx = 0x6040,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Control word"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED16,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u16 = CO_UNSIGNED16_MIN },
			.max = { .u16 = CO_UNSIGNED16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u16 = CO_UNSIGNED16_MIN },
#endif
			.val = { .u16 = CO_UNSIGNED16_MIN },
			.access = CO_ACCESS_RWW,
			.pdo_mapping = 1,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Status word"),
#endif
		.idx = 0x6041,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Status word"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED16,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u16 = CO_UNSIGNED16_MIN },
			.max = { .u16 = CO_UNSIGNED16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u16 = CO_UNSIGNED16_MIN },
#endif
			.val = { .u16 = CO_UNSIGNED16_MIN },
			.access = CO_ACCESS_RO,
			.pdo_mapping = 1,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Halt option code"),
#endif
		.idx = 0x605d,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Halt option code"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_INTEGER16,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .i16 = CO_INTEGER16_MIN },
			.max = { .i16 = CO_INTEGER16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .i16 = 0 },
#endif
			.val = { .i16 = 0 },
			.access = CO_ACCESS_RWW,
			.pdo_mapping = 1,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Modes of operation"),
#endif
		.idx = 0x6060,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Modes of operation"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = CO_UNSIGNED8_MIN },
			.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = CO_UNSIGNED8_MIN },
#endif
			.val = { .u8 = CO_UNSIGNED8_MIN },
			.access = CO_ACCESS_RWW,
			.pdo_mapping = 1,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Commanded position"),
#endif
		.idx = 0x6062,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Commanded position"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .i32 = CO_INTEGER32_MIN },
			.max = { .i32 = CO_INTEGER32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .i32 = 0l },
#endif
			.val = { .i32 = 0l },
			.access = CO_ACCESS_RO,
			.pdo_mapping = 1,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Actual motor position"),
#endif
		.idx = 0x6064,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Actual motor position"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .i32 = CO_INTEGER32_MIN },
			.max = { .i32 = CO_INTEGER32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .i32 = 0l },
#endif
			.val = { .i32 = 0l },
			.access = CO_ACCESS_RWR,
			.pdo_mapping = 1,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Profile target position"),
#endif
		.idx = 0x607a,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Profile target position"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .i32 = CO_INTEGER32_MIN },
			.max = { .i32 = CO_INTEGER32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .i32 = 0l },
#endif
			.val = { .i32 = 0l },
			.access = CO_ACCESS_RWW,
			.pdo_mapping = 1,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Profile target velocity"),
#endif
		.idx = 0x6081,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Profile target velocity"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .i32 = 0l },
			.max = { .i32 = 500000000l },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .i32 = 0l },
#endif
			.val = { .i32 = 0l },
			.access = CO_ACCESS_RWW,
			.pdo_mapping = 1,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Profile target acceleration"),
#endif
		.idx = 0x6083,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Profile target acceleration"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RWW,
			.pdo_mapping = 1,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Profile target deceleration"),
#endif
		.idx = 0x6084,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Profile target deceleration"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RWW,
			.pdo_mapping = 1,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Motion profile type"),
#endif
		.idx = 0x6086,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Motion profile type"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_INTEGER16,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .i16 = CO_INTEGER16_MIN },
			.max = { .i16 = CO_INTEGER16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .i16 = 0 },
#endif
			.val = { .i16 = 0 },
			.access = CO_ACCESS_RWW,
			.pdo_mapping = 1,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Homing method"),
#endif
		.idx = 0x6098,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Homing method"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_INTEGER8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .i8 = CO_INTEGER8_MIN },
			.max = { .i8 = CO_INTEGER8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .i8 = 0 },
#endif
			.val = { .i8 = 0 },
			.access = CO_ACCESS_RWW,
			.pdo_mapping = 1,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Homing speeds"),
#endif
		.idx = 0x6099,
		.code = CO_OBJECT_ARRAY,
		.nsub = 3,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Number of Entries"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u8 = 0x02 },
			.max = { .u8 = 0x02 },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u8 = 0x02 },
#endif
			.val = { .u8 = 0x02 },
			.access = CO_ACCESS_RO,
			.pdo_mapping = 0,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Homing velocity (fast)"),
#endif
			.subidx = 0x01,
			.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .i32 = 0l },
			.max = { .i32 = 500000000l },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .i32 = 0l },
#endif
			.val = { .i32 = 0l },
			.access = CO_ACCESS_RWW,
			.pdo_mapping = 1,
			.flags = 0
		}, {
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Homing velocity (slow)"),
#endif
			.subidx = 0x02,
			.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .i32 = 0l },
			.max = { .i32 = 500000000l },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .i32 = 0l },
#endif
			.val = { .i32 = 0l },
			.access = CO_ACCESS_RWW,
			.pdo_mapping = 1,
			.flags = 0
		}}
	}, {
#if !LELY_NO_CO_OBJ_NAME
		.name = CO_SDEV_STRING("Homing acceleration"),
#endif
		.idx = 0x609a,
		.code = CO_OBJECT_VAR,
		.nsub = 1,
		.subs = (const struct co_ssub[]){{
#if !LELY_NO_CO_OBJ_NAME
			.name = CO_SDEV_STRING("Homing acceleration"),
#endif
			.subidx = 0x00,
			.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_LIMITS
			.min = { .u32 = CO_UNSIGNED32_MIN },
			.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
			.def = { .u32 = CO_UNSIGNED32_MIN },
#endif
			.val = { .u32 = CO_UNSIGNED32_MIN },
			.access = CO_ACCESS_RWW,
			.pdo_mapping = 1,
			.flags = 0
		}}
	}}
};

