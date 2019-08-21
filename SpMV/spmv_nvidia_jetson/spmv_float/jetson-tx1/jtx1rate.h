/**
 * @file jtx1rate.h
 * @author cs
 * @brief This header file contains the functions for setting and reading
 * Jetson TX1's GPU and EMC frequncies.
 */
#ifndef TX1RATE_H
#define TX1RATE_H

/**
 * @brief Units with adjustable operating frequency
 */
typedef enum jtx1_units {
    EMC_RATE = 0, ///< external memory controller (EMC)
    GPU_RATE, ///< graphics processing unit (GPU)
    CPU0_RATE, ///< first core of central processing unit (CPU)
    CPU1_RATE, ///< second core of CPU
    CPU2_RATE, ///< third core of CPU
    CPU3_RATE, ///< fourth core of CPU
} jtx1_unit;

/**
 * @brief Read operating frequency
 * @param unit See ::jtx1_unit
 * @param *rate Output's reference
 */
void jtx1_get_rate(const jtx1_unit unit, unsigned long *rate);

/**
 * @brief Set operating frequency
 * @param unit See ::jtx1_unit
 * @param rate Operating frequency 
 */
void jtx1_set_rate(const jtx1_unit unit, const unsigned long rate);

#endif
