/**
 * @file jtx1temp.h
 * @author cs
 * @brief This header file contains the functions for reading
 * Jetson TX1's values of thermal zones.
 */
#ifndef JTX1TEMP_H_
#define JTX1TEMP_H_

#define MAX_BUFF 128
#define SYSFS_TEMP_PATH "/sys/class/thermal"

/**
 * @brief Thermal zones index
 */
typedef enum jtx1_tzones {
    A0 = 0, ///< on-chip thermal zone (mC)
    CPU, ///< on-chip thermal zone (mC)
    GPU, ///< on-chip thermal zone (mC)
    PLL, ///< on-chip thermal zone (mC)
    PMIC, ///< on-chip thermal zone (mC)
    TDIODE, ///< on-module thermal zone (mC)
    TBOARD, ///< on-module thermal zone (mC)
    FAN ///< on-chip thermal zone (mC)
} jtx1_tzone;


/**
 * @brief Read on-chip and on-module temperatures.
 *
 * @param zone Indexed by ::jtx1_tzone
 * @param *temperature Output's reference
 */
void jtx1_get_temp(jtx1_tzone zone,
		   unsigned int *temperature);

#endif // JTX1TEMP_H_
