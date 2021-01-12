#pragma once

/**
* Return type for device probing routines.
*/
typedef enum {
	probe_ok, /*!< Probe finished successfully. */
	probe_next /*!< Probing device failed. Try to use other driver. */
} probe_ret_t;
