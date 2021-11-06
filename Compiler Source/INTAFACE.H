/***************************************************************************/
/* Prototype and Error codes retruned by the Intaface subsystem.           */
/***************************************************************************/

#define   DllExport  __declspec(dllexport)

DllExport short AquireCmdLineArgs (char * title, ...);

# define  BAD_FIELD_TYPE_NAME       1000
# define  FIELD_SYNTAX_ERROR        1001
# define  DEFINITION_TOO_LONG       1002
# define  DUP_FIELD_NAME            1003
# define  BAD_MIN_MAX               1004
# define  DUP_CMD_LINE_TOKEN        1005
# define  BAD_CYCLE_VALUE           1006
# define  CHAR_VAL_TOO_LONG         1007
# define  STRING_VAL_TOO_LONG       1008
# define  NUMBER_TOO_LOW            1009
# define  NUMBER_TOO_BIG            1010
# define  NUMBER_NOT_SHORT          1011
# define  INVALID_DATA_FOR_NUMERIC  1012
# define  BAD_USE_OF_LENGTH         1013
# define  MISSING_VALUE             1014
# define  BAD_ARG_PREFIX            1015
# define  UNKNOWN_SWITCH            1016
# define  INTERNAL_ERROR            1017
# define  INVALID_END               1018
# define  COMMAND_CANCELLED         1019
# define  INVALID_COLOR             1020
# define  INVALID_HIDDEN            1021

/************************** end of header **********************************/
