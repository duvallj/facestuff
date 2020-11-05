#ifndef WINBGINPUT_H
#define WINBGINPUT_H

#include <Windows.h>

/** @brief The type of function pointer key handler callbacks should be.
 *
 * Key handler callbacks get called every time a key goes from up to down,
 * with the corresponding VK_KEY code as its argument
 */
typedef void (*key_handler_fp)(DWORD);

/**
 * @brief Initializes the bg input variables
 *
 * This is a no-op if we are already initialized
 * */
extern BOOL bg_input_init();

/**
 * @brief Deinitializes the bg input variables
 *
 * This is a no-op if we are not initalized
 */
extern void bg_input_deinit();

/**
 * @brief Sets a key handler function as the one to call
 *
 * There can only be one key handler at a time, so if one was already set, this
 * replaces the previous one.
 *
 * @pre kh is a valid function pointer
 */
extern void bg_input_register_callback(key_handler_fp kh);

#endif /* WINBGINPUT_H */
