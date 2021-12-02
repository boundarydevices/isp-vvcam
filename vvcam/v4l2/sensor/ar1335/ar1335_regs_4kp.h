#ifndef _VVCAM_AR1335_REGS_4KP_H_
#define _VVCAM_AR1335_REGS_4KP_H_

#include "vvsensor.h"

/* 1080P30 RAW10 */
static struct vvsensor_ar1335_reg_value_t ar1335_init_setting_4kp[] = {
    {0x301A, 0x0219, 0, 100}, // RESET_REGISTER
    {0x3042, 0x1004, 0, 0}, // DARK_CONTROL2
    {0x30D2, 0x0120, 0, 0}, // CRM_CONTROL
    {0x30D4, 0x0000, 0, 0}, // COLUMN_CORRECTION
    {0x3090, 0x0000, 0, 0}, // RNF_CONTROL
    {0x30FC, 0x0060, 0, 0}, // MODULE_CLK_OFF
    {0x30FE, 0x0060, 0, 0}, // CTREE_OFF
    {0x31E0, 0x0781, 0, 0}, // PIX_DEF_ID
    {0x3180, 0x9434, 0, 0}, // FINE_DIG_CORRECTION_CONTROL
    {0x317C, 0xEFF4, 0, 0}, // ANALOG_CONTROL7
    {0x30EE, 0x613E, 0, 0}, // DARK_CONTROL3
    {0x3F2C, 0x4428, 0, 0}, // GTH_THRES_RTN
    {0x3D00, 0x0446, 0, 0}, // DYNAMIC_SEQRAM_00
    {0x3D02, 0x4C66, 0, 0}, // DYNAMIC_SEQRAM_02
    {0x3D04, 0xFFFF, 0, 0}, // DYNAMIC_SEQRAM_04
    {0x3D06, 0xFFFF, 0, 0}, // DYNAMIC_SEQRAM_06
    {0x3D08, 0x5E40, 0, 0}, // DYNAMIC_SEQRAM_08
    {0x3D0A, 0x1146, 0, 0}, // DYNAMIC_SEQRAM_0A
    {0x3D0C, 0x5D41, 0, 0}, // DYNAMIC_SEQRAM_0C
    {0x3D0E, 0x1088, 0, 0}, // DYNAMIC_SEQRAM_0E
    {0x3D10, 0x8342, 0, 0}, // DYNAMIC_SEQRAM_10
    {0x3D12, 0x00C0, 0, 0}, // DYNAMIC_SEQRAM_12
    {0x3D14, 0x5580, 0, 0}, // DYNAMIC_SEQRAM_14
    {0x3D16, 0x5B83, 0, 0}, // DYNAMIC_SEQRAM_16
    {0x3D18, 0x6084, 0, 0}, // DYNAMIC_SEQRAM_18
    {0x3D1A, 0x5A8D, 0, 0}, // DYNAMIC_SEQRAM_1A
    {0x3D1C, 0x00C0, 0, 0}, // DYNAMIC_SEQRAM_1C
    {0x3D1E, 0x8342, 0, 0}, // DYNAMIC_SEQRAM_1E
    {0x3D20, 0x925A, 0, 0}, // DYNAMIC_SEQRAM_20
    {0x3D22, 0x8664, 0, 0}, // DYNAMIC_SEQRAM_22
    {0x3D24, 0x1030, 0, 0}, // DYNAMIC_SEQRAM_24
    {0x3D26, 0x801C, 0, 0}, // DYNAMIC_SEQRAM_26
    {0x3D28, 0x00A0, 0, 0}, // DYNAMIC_SEQRAM_28
    {0x3D2A, 0x56B0, 0, 0}, // DYNAMIC_SEQRAM_2A
    {0x3D2C, 0x5788, 0, 0}, // DYNAMIC_SEQRAM_2C
    {0x3D2E, 0x5150, 0, 0}, // DYNAMIC_SEQRAM_2E
    {0x3D30, 0x824D, 0, 0}, // DYNAMIC_SEQRAM_30
    {0x3D32, 0x8D58, 0, 0}, // DYNAMIC_SEQRAM_32
    {0x3D34, 0x58D2, 0, 0}, // DYNAMIC_SEQRAM_34
    {0x3D36, 0x438A, 0, 0}, // DYNAMIC_SEQRAM_36
    {0x3D38, 0x4592, 0, 0}, // DYNAMIC_SEQRAM_38
    {0x3D3A, 0x458A, 0, 0}, // DYNAMIC_SEQRAM_3A
    {0x3D3C, 0x4389, 0, 0}, // DYNAMIC_SEQRAM_3C
    {0x3D3E, 0x51FF, 0, 0}, // DYNAMIC_SEQRAM_3E
    {0x3D40, 0x8451, 0, 0}, // DYNAMIC_SEQRAM_40
    {0x3D42, 0x8410, 0, 0}, // DYNAMIC_SEQRAM_42
    {0x3D44, 0x0C88, 0, 0}, // DYNAMIC_SEQRAM_44
    {0x3D46, 0x5959, 0, 0}, // DYNAMIC_SEQRAM_46
    {0x3D48, 0x8A5F, 0, 0}, // DYNAMIC_SEQRAM_48
    {0x3D4A, 0xDA42, 0, 0}, // DYNAMIC_SEQRAM_4A
    {0x3D4C, 0x9361, 0, 0}, // DYNAMIC_SEQRAM_4C
    {0x3D4E, 0x8262, 0, 0}, // DYNAMIC_SEQRAM_4E
    {0x3D50, 0x8342, 0, 0}, // DYNAMIC_SEQRAM_50
    {0x3D52, 0x8010, 0, 0}, // DYNAMIC_SEQRAM_52
    {0x3D54, 0xC041, 0, 0}, // DYNAMIC_SEQRAM_54
    {0x3D56, 0x64FF, 0, 0}, // DYNAMIC_SEQRAM_56
    {0x3D58, 0xFFB7, 0, 0}, // DYNAMIC_SEQRAM_58
    {0x3D5A, 0x4081, 0, 0}, // DYNAMIC_SEQRAM_5A
    {0x3D5C, 0x4080, 0, 0}, // DYNAMIC_SEQRAM_5C
    {0x3D5E, 0x4180, 0, 0}, // DYNAMIC_SEQRAM_5E
    {0x3D60, 0x4280, 0, 0}, // DYNAMIC_SEQRAM_60
    {0x3D62, 0x438D, 0, 0}, // DYNAMIC_SEQRAM_62
    {0x3D64, 0x44BA, 0, 0}, // DYNAMIC_SEQRAM_64
    {0x3D66, 0x4488, 0, 0}, // DYNAMIC_SEQRAM_66
    {0x3D68, 0x4380, 0, 0}, // DYNAMIC_SEQRAM_68
    {0x3D6A, 0x4241, 0, 0}, // DYNAMIC_SEQRAM_6A
    {0x3D6C, 0x8140, 0, 0}, // DYNAMIC_SEQRAM_6C
    {0x3D6E, 0x8240, 0, 0}, // DYNAMIC_SEQRAM_6E
    {0x3D70, 0x8041, 0, 0}, // DYNAMIC_SEQRAM_70
    {0x3D72, 0x8042, 0, 0}, // DYNAMIC_SEQRAM_72
    {0x3D74, 0x8043, 0, 0}, // DYNAMIC_SEQRAM_74
    {0x3D76, 0x8D44, 0, 0}, // DYNAMIC_SEQRAM_76
    {0x3D78, 0xBA44, 0, 0}, // DYNAMIC_SEQRAM_78
    {0x3D7A, 0x875E, 0, 0}, // DYNAMIC_SEQRAM_7A
    {0x3D7C, 0x4354, 0, 0}, // DYNAMIC_SEQRAM_7C
    {0x3D7E, 0x4241, 0, 0}, // DYNAMIC_SEQRAM_7E
    {0x3D80, 0x8140, 0, 0}, // DYNAMIC_SEQRAM_80
    {0x3D82, 0x8120, 0, 0}, // DYNAMIC_SEQRAM_82
    {0x3D84, 0x2881, 0, 0}, // DYNAMIC_SEQRAM_84
    {0x3D86, 0x6026, 0, 0}, // DYNAMIC_SEQRAM_86
    {0x3D88, 0x8055, 0, 0}, // DYNAMIC_SEQRAM_88
    {0x3D8A, 0x8070, 0, 0}, // DYNAMIC_SEQRAM_8A
    {0x3D8C, 0x8040, 0, 0}, // DYNAMIC_SEQRAM_8C
    {0x3D8E, 0x4C81, 0, 0}, // DYNAMIC_SEQRAM_8E
    {0x3D90, 0x45C3, 0, 0}, // DYNAMIC_SEQRAM_90
    {0x3D92, 0x4581, 0, 0}, // DYNAMIC_SEQRAM_92
    {0x3D94, 0x4C40, 0, 0}, // DYNAMIC_SEQRAM_94
    {0x3D96, 0x8070, 0, 0}, // DYNAMIC_SEQRAM_96
    {0x3D98, 0x8040, 0, 0}, // DYNAMIC_SEQRAM_98
    {0x3D9A, 0x4C85, 0, 0}, // DYNAMIC_SEQRAM_9A
    {0x3D9C, 0x6CA8, 0, 0}, // DYNAMIC_SEQRAM_9C
    {0x3D9E, 0x6C8C, 0, 0}, // DYNAMIC_SEQRAM_9E
    {0x3DA0, 0x000E, 0, 0}, // DYNAMIC_SEQRAM_A0
    {0x3DA2, 0xBE44, 0, 0}, // DYNAMIC_SEQRAM_A2
    {0x3DA4, 0x8844, 0, 0}, // DYNAMIC_SEQRAM_A4
    {0x3DA6, 0xBC78, 0, 0}, // DYNAMIC_SEQRAM_A6
    {0x3DA8, 0x0900, 0, 0}, // DYNAMIC_SEQRAM_A8
    {0x3DAA, 0x8904, 0, 0}, // DYNAMIC_SEQRAM_AA
    {0x3DAC, 0x8080, 0, 0}, // DYNAMIC_SEQRAM_AC
    {0x3DAE, 0x0240, 0, 0}, // DYNAMIC_SEQRAM_AE
    {0x3DB0, 0x8609, 0, 0}, // DYNAMIC_SEQRAM_B0
    {0x3DB2, 0x008E, 0, 0}, // DYNAMIC_SEQRAM_B2
    {0x3DB4, 0x0900, 0, 0}, // DYNAMIC_SEQRAM_B4
    {0x3DB6, 0x8002, 0, 0}, // DYNAMIC_SEQRAM_B6
    {0x3DB8, 0x4080, 0, 0}, // DYNAMIC_SEQRAM_B8
    {0x3DBA, 0x0480, 0, 0}, // DYNAMIC_SEQRAM_BA
    {0x3DBC, 0x887C, 0, 0}, // DYNAMIC_SEQRAM_BC
    {0x3DBE, 0xAA86, 0, 0}, // DYNAMIC_SEQRAM_BE
    {0x3DC0, 0x0900, 0, 0}, // DYNAMIC_SEQRAM_C0
    {0x3DC2, 0x877A, 0, 0}, // DYNAMIC_SEQRAM_C2
    {0x3DC4, 0x000E, 0, 0}, // DYNAMIC_SEQRAM_C4
    {0x3DC6, 0xC379, 0, 0}, // DYNAMIC_SEQRAM_C6
    {0x3DC8, 0x4C40, 0, 0}, // DYNAMIC_SEQRAM_C8
    {0x3DCA, 0xBF70, 0, 0}, // DYNAMIC_SEQRAM_CA
    {0x3DCC, 0x5E40, 0, 0}, // DYNAMIC_SEQRAM_CC
    {0x3DCE, 0x114E, 0, 0}, // DYNAMIC_SEQRAM_CE
    {0x3DD0, 0x5D41, 0, 0}, // DYNAMIC_SEQRAM_D0
    {0x3DD2, 0x5383, 0, 0}, // DYNAMIC_SEQRAM_D2
    {0x3DD4, 0x4200, 0, 0}, // DYNAMIC_SEQRAM_D4
    {0x3DD6, 0xC055, 0, 0}, // DYNAMIC_SEQRAM_D6
    {0x3DD8, 0xA400, 0, 0}, // DYNAMIC_SEQRAM_D8
    {0x3DDA, 0xC083, 0, 0}, // DYNAMIC_SEQRAM_DA
    {0x3DDC, 0x4288, 0, 0}, // DYNAMIC_SEQRAM_DC
    {0x3DDE, 0x6083, 0, 0}, // DYNAMIC_SEQRAM_DE
    {0x3DE0, 0x5B80, 0, 0}, // DYNAMIC_SEQRAM_E0
    {0x3DE2, 0x5A64, 0, 0}, // DYNAMIC_SEQRAM_E2
    {0x3DE4, 0x1030, 0, 0}, // DYNAMIC_SEQRAM_E4
    {0x3DE6, 0x801C, 0, 0}, // DYNAMIC_SEQRAM_E6
    {0x3DE8, 0x00A5, 0, 0}, // DYNAMIC_SEQRAM_E8
    {0x3DEA, 0x5697, 0, 0}, // DYNAMIC_SEQRAM_EA
    {0x3DEC, 0x57A5, 0, 0}, // DYNAMIC_SEQRAM_EC
    {0x3DEE, 0x5180, 0, 0}, // DYNAMIC_SEQRAM_EE
    {0x3DF0, 0x505A, 0, 0}, // DYNAMIC_SEQRAM_F0
    {0x3DF2, 0x814D, 0, 0}, // DYNAMIC_SEQRAM_F2
    {0x3DF4, 0x8358, 0, 0}, // DYNAMIC_SEQRAM_F4
    {0x3DF6, 0x8058, 0, 0}, // DYNAMIC_SEQRAM_F6
    {0x3DF8, 0xA943, 0, 0}, // DYNAMIC_SEQRAM_F8
    {0x3DFA, 0x8345, 0, 0}, // DYNAMIC_SEQRAM_FA
    {0x3DFC, 0xB045, 0, 0}, // DYNAMIC_SEQRAM_FC
    {0x3DFE, 0x8343, 0, 0}, // DYNAMIC_SEQRAM_FE
    {0x3E00, 0xA351, 0, 0}, // DYNAMIC_SEQRAM_100
    {0x3E02, 0xE251, 0, 0}, // DYNAMIC_SEQRAM_102
    {0x3E04, 0x8C59, 0, 0}, // DYNAMIC_SEQRAM_104
    {0x3E06, 0x8059, 0, 0}, // DYNAMIC_SEQRAM_106
    {0x3E08, 0x8A5F, 0, 0}, // DYNAMIC_SEQRAM_108
    {0x3E0A, 0xEC7C, 0, 0}, // DYNAMIC_SEQRAM_10A
    {0x3E0C, 0xCC84, 0, 0}, // DYNAMIC_SEQRAM_10C
    {0x3E0E, 0x6182, 0, 0}, // DYNAMIC_SEQRAM_10E
    {0x3E10, 0x6283, 0, 0}, // DYNAMIC_SEQRAM_110
    {0x3E12, 0x4283, 0, 0}, // DYNAMIC_SEQRAM_112
    {0x3E14, 0x10CC, 0, 0}, // DYNAMIC_SEQRAM_114
    {0x3E16, 0x6496, 0, 0}, // DYNAMIC_SEQRAM_116
    {0x3E18, 0x4281, 0, 0}, // DYNAMIC_SEQRAM_118
    {0x3E1A, 0x41BB, 0, 0}, // DYNAMIC_SEQRAM_11A
    {0x3E1C, 0x4082, 0, 0}, // DYNAMIC_SEQRAM_11C
    {0x3E1E, 0x407E, 0, 0}, // DYNAMIC_SEQRAM_11E
    {0x3E20, 0xCC41, 0, 0}, // DYNAMIC_SEQRAM_120
    {0x3E22, 0x8042, 0, 0}, // DYNAMIC_SEQRAM_122
    {0x3E24, 0x8043, 0, 0}, // DYNAMIC_SEQRAM_124
    {0x3E26, 0x8300, 0, 0}, // DYNAMIC_SEQRAM_126
    {0x3E28, 0xC088, 0, 0}, // DYNAMIC_SEQRAM_128
    {0x3E2A, 0x44BA, 0, 0}, // DYNAMIC_SEQRAM_12A
    {0x3E2C, 0x4488, 0, 0}, // DYNAMIC_SEQRAM_12C
    {0x3E2E, 0x00C8, 0, 0}, // DYNAMIC_SEQRAM_12E
    {0x3E30, 0x8042, 0, 0}, // DYNAMIC_SEQRAM_130
    {0x3E32, 0x4181, 0, 0}, // DYNAMIC_SEQRAM_132
    {0x3E34, 0x4082, 0, 0}, // DYNAMIC_SEQRAM_134
    {0x3E36, 0x4080, 0, 0}, // DYNAMIC_SEQRAM_136
    {0x3E38, 0x4180, 0, 0}, // DYNAMIC_SEQRAM_138
    {0x3E3A, 0x4280, 0, 0}, // DYNAMIC_SEQRAM_13A
    {0x3E3C, 0x4383, 0, 0}, // DYNAMIC_SEQRAM_13C
    {0x3E3E, 0x00C0, 0, 0}, // DYNAMIC_SEQRAM_13E
    {0x3E40, 0x8844, 0, 0}, // DYNAMIC_SEQRAM_140
    {0x3E42, 0xBA44, 0, 0}, // DYNAMIC_SEQRAM_142
    {0x3E44, 0x8800, 0, 0}, // DYNAMIC_SEQRAM_144
    {0x3E46, 0xC880, 0, 0}, // DYNAMIC_SEQRAM_146
    {0x3E48, 0x4241, 0, 0}, // DYNAMIC_SEQRAM_148
    {0x3E4A, 0x8240, 0, 0}, // DYNAMIC_SEQRAM_14A
    {0x3E4C, 0x8140, 0, 0}, // DYNAMIC_SEQRAM_14C
    {0x3E4E, 0x8041, 0, 0}, // DYNAMIC_SEQRAM_14E
    {0x3E50, 0x8042, 0, 0}, // DYNAMIC_SEQRAM_150
    {0x3E52, 0x8043, 0, 0}, // DYNAMIC_SEQRAM_152
    {0x3E54, 0x8300, 0, 0}, // DYNAMIC_SEQRAM_154
    {0x3E56, 0xC088, 0, 0}, // DYNAMIC_SEQRAM_156
    {0x3E58, 0x44BA, 0, 0}, // DYNAMIC_SEQRAM_158
    {0x3E5A, 0x4488, 0, 0}, // DYNAMIC_SEQRAM_15A
    {0x3E5C, 0x00C8, 0, 0}, // DYNAMIC_SEQRAM_15C
    {0x3E5E, 0x8042, 0, 0}, // DYNAMIC_SEQRAM_15E
    {0x3E60, 0x4181, 0, 0}, // DYNAMIC_SEQRAM_160
    {0x3E62, 0x4082, 0, 0}, // DYNAMIC_SEQRAM_162
    {0x3E64, 0x4080, 0, 0}, // DYNAMIC_SEQRAM_164
    {0x3E66, 0x4180, 0, 0}, // DYNAMIC_SEQRAM_166
    {0x3E68, 0x4280, 0, 0}, // DYNAMIC_SEQRAM_168
    {0x3E6A, 0x4383, 0, 0}, // DYNAMIC_SEQRAM_16A
    {0x3E6C, 0x00C0, 0, 0}, // DYNAMIC_SEQRAM_16C
    {0x3E6E, 0x8844, 0, 0}, // DYNAMIC_SEQRAM_16E
    {0x3E70, 0xBA44, 0, 0}, // DYNAMIC_SEQRAM_170
    {0x3E72, 0x8800, 0, 0}, // DYNAMIC_SEQRAM_172
    {0x3E74, 0xC880, 0, 0}, // DYNAMIC_SEQRAM_174
    {0x3E76, 0x4241, 0, 0}, // DYNAMIC_SEQRAM_176
    {0x3E78, 0x8140, 0, 0}, // DYNAMIC_SEQRAM_178
    {0x3E7A, 0x9F5E, 0, 0}, // DYNAMIC_SEQRAM_17A
    {0x3E7C, 0x8A54, 0, 0}, // DYNAMIC_SEQRAM_17C
    {0x3E7E, 0x8620, 0, 0}, // DYNAMIC_SEQRAM_17E
    {0x3E80, 0x2881, 0, 0}, // DYNAMIC_SEQRAM_180
    {0x3E82, 0x6026, 0, 0}, // DYNAMIC_SEQRAM_182
    {0x3E84, 0x8055, 0, 0}, // DYNAMIC_SEQRAM_184
    {0x3E86, 0x8070, 0, 0}, // DYNAMIC_SEQRAM_186
    {0x3E88, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_188
    {0x3E8A, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_18A
    {0x3E8C, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_18C
    {0x3E8E, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_18E
    {0x3E90, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_190
    {0x3E92, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_192
    {0x3E94, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_194
    {0x3E96, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_196
    {0x3E98, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_198
    {0x3E9A, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_19A
    {0x3E9C, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_19C
    {0x3E9E, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_19E
    {0x3EA0, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_1A0
    {0x3EA2, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_1A2
    {0x3EA4, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_1A4
    {0x3EA6, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_1A6
    {0x3EA8, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_1A8
    {0x3EAA, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_1AA
    {0x3EAC, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_1AC
    {0x3EAE, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_1AE
    {0x3EB0, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_1B0
    {0x3EB2, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_1B2
    {0x3EB4, 0x0000, 0, 0}, // DYNAMIC_SEQRAM_1B4
    {0x3EB6, 0x004D, 0, 0}, // DAC_LD_0_1
    {0x3EBC, 0xAA06, 0, 0}, // DAC_LD_6_7
    {0x3EC0, 0x2E02, 0, 0}, // DAC_LD_10_11
    {0x3EC2, 0x7700, 0, 0}, // DAC_LD_12_13
    {0x3EC4, 0x1C08, 0, 0}, // DAC_LD_14_15
    {0x3EC6, 0xEA44, 0, 0}, // DAC_LD_16_17
    {0x3EC8, 0x0F0F, 0, 0}, // DAC_LD_18_19
    {0x3ECA, 0x0F4A, 0, 0}, // DAC_LD_20_21
    {0x3ECC, 0x0706, 0, 0}, // DAC_LD_22_23
    {0x3ECE, 0x443B, 0, 0}, // DAC_LD_24_25
    {0x3ED0, 0x12F0, 0, 0}, // DAC_LD_26_27
    {0x3ED2, 0x0039, 0, 0}, // DAC_LD_28_29
    {0x3ED4, 0x862F, 0, 0}, // DAC_LD_30_31
    {0x3ED6, 0x4080, 0, 0}, // DAC_LD_32_33
    {0x3ED8, 0x0523, 0, 0}, // DAC_LD_34_35
    {0x3EDA, 0xF8AA, 0, 0}, // DAC_LD_36_37
    {0x3EDC, 0x5078, 0, 0}, // DAC_LD_38_39
    {0x3EDE, 0x5005, 0, 0}, // DAC_LD_40_41
    {0x316A, 0x8200, 0, 0}, // DAC_RSTLO
    {0x316E, 0x8200, 0, 0}, // DAC_ECL
    {0x316C, 0x8200, 0, 0}, // DAC_TXLO
    {0x3EF0, 0x414D, 0, 0}, // DAC_LD_ECL
    {0x3EF2, 0x0101, 0, 0}, // DAC_LD_FSC
    {0x3EF6, 0x0307, 0, 0}, // DAC_LD_RSTD
    {0x3EFA, 0x0F0F, 0, 0}, // DAC_LD_TXLO
    {0x3EFC, 0x0F0F, 0, 0}, // DAC_LD_TXLO1
    {0x3EFE, 0x0F0F, 0, 0}, // DAC_LD_TXLO2
    {0x31B0, 0x0058, 0, 0}, // FRAME_PREAMBLE
    {0x31B2, 0x002C, 0, 0}, // LINE_PREAMBLE
    {0x31B4, 0x23D0, 0, 0}, // MIPI_TIMING_0
    {0x31B6, 0x140A, 0, 0}, // MIPI_TIMING_1
    {0x31B8, 0x2413, 0, 0}, // MIPI_TIMING_2
    {0x31BA, 0x1C70, 0, 0}, // MIPI_TIMING_3
    {0x31BC, 0x860B, 0, 0}, // MIPI_TIMING_4
    {0x0304, 0x0101, 0, 0}, // PRE_PLL_CLK_DIV
    {0x0306, 0x2C2C, 0, 0}, // PLL_MULTIPLIER
    {0x0302, 0x0001, 0, 0}, // VT_SYS_CLK_DIV
    {0x030A, 0x0001, 0, 0}, // OP_SYS_CLK_DIV
    {0x0300, 0x0005, 0, 0}, // VT_PIX_CLK_DIV
    {0x0308, 0x000A, 0, 0}, // OP_PIX_CLK_DIV
    {0x0112, 0x0A0A, 0, 0}, // CCP_DATA_FORMAT
    {0x3016, 0x0101, 0, 0}, // ROW_SPEED
    {0x0344, 0x00C8, 0, 0}, // X_ADDR_START
    {0x0348, 0x0FC7, 0, 0}, // X_ADDR_END
    {0x0346, 0x01F0, 0, 0}, // Y_ADDR_START
    {0x034A, 0x0A5F, 0, 0}, // Y_ADDR_END
    {0x034C, 0x0F00, 0, 0}, // X_OUTPUT_SIZE
    {0x034E, 0x0870, 0, 0}, // Y_OUTPUT_SIZE
    {0x3040, 0x0041, 0, 0}, // READ_MODE
    {0x0112, 0x0A0A, 0, 0}, // CCP_DATA_FORMAT
    {0x0112, 0x0A0A, 0, 0}, // CCP_DATA_FORMAT
    {0x3172, 0x0206, 0, 0}, // ANALOG_CONTROL2
    {0x317A, 0x416E, 0, 0}, // ANALOG_CONTROL6
    {0x3F3C, 0x0003, 0, 0}, // ANALOG_CONTROL9
    {0x0342, 0x1230, 0, 0}, // LINE_LENGTH_PCK
    {0x0340, 0x0BCC, 0, 0}, // FRAME_LENGTH_LINES
    {0x0202, 0x0AA0, 0, 0}, // COARSE_INTEGRATION_TIME
    {0x0342, 0x1230, 0, 0}, // LINE_LENGTH_PCK
    {0x0340, 0x0BCC, 0, 0}, // FRAME_LENGTH_LINES
    {0x0202, 0x0AA0, 0, 0}, // COARSE_INTEGRATION_TIME
    {0x30FE, 0x0060, 0, 0}, // CTREE_OFF
    {0x31E0, 0x0781, 0, 0}, // PIX_DEF_ID
    {0x3F00, 0x004F, 0, 0}, // BM_T0
    {0x3F02, 0x0125, 0, 0}, // BM_T1
    {0x3F04, 0x0020, 0, 0}, // NOISE_GAIN_THRESHOLD0
    {0x3F06, 0x0040, 0, 0}, // NOISE_GAIN_THRESHOLD1
    {0x3F08, 0x0070, 0, 0}, // NOISE_GAIN_THRESHOLD2
    {0x3F0A, 0x0101, 0, 0}, // NOISE_FLOOR10
    {0x3F0C, 0x0302, 0, 0}, // NOISE_FLOOR32
    {0x3F1E, 0x0022, 0, 0}, // NOISE_COEF
    {0x3F1A, 0x01FF, 0, 0}, // CROSSFACTOR2
    {0x3F14, 0x0101, 0, 0}, // SINGLE_K_FACTOR2
    {0x3F44, 0x0707, 0, 0}, // COUPLE_K_FACTOR2
    {0x3F18, 0x011E, 0, 0}, // CROSSFACTOR1
    {0x3F12, 0x0303, 0, 0}, // SINGLE_K_FACTOR1
    {0x3F42, 0x1511, 0, 0}, // COUPLE_K_FACTOR1
    {0x3F16, 0x011E, 0, 0}, // CROSSFACTOR0
    {0x3F10, 0x0505, 0, 0}, // SINGLE_K_FACTOR0
    {0x3F40, 0x1511, 0, 0}, // COUPLE_K_FACTOR0
    {0x3F3C, 0x0003, 0, 0}, // ANALOG_CONTROL9
    {0x301A, 0x021C, 0, 0}, // RESET_REGISTER
};

#endif
