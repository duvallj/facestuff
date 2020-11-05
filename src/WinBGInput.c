#include "WinBGInput.h"
#include "Debug.h"
#include <processthreadsapi.h>
#include <psapi.h>
#include <winuser.h>


static void key_handler_default(DWORD vkey) {
  dbg_printf("%lu\n", vkey);
  return;
}

static HHOOK WINBGINPUT_HOOK_HANDLE = NULL;
static key_handler_fp WINBGINPUT_KEY_HANDLER = &key_handler_default;

/**
 * @brief Returns a pointer to the main module as read by EnumProcessModules
 *
 * @returns NULL on any error
 * @returns a valid HMODULE otherwise
 */
static HMODULE get_main_module_handle() {
  HMODULE module_handle_arr[1024];
  DWORD cbs_needed = 0;

  HANDLE process_handle = GetCurrentProcess();
  if (process_handle == NULL) {
    return NULL;
  }

  BOOL enum_proc_success = EnumProcessModules(
      process_handle,
      module_handle_arr,
      sizeof(module_handle_arr),
      &cbs_needed
  );

  if (!enum_proc_success || cbs_needed <= 0) {
    return NULL;
  }

  return module_handle_arr[0];
}

/**
 * @brief Main entrypoint for the Windows keyboard hook
 */
static LRESULT hook_fn(int code, WPARAM wparam, LPARAM lparam) {
  if (code >= 0 && (wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN)) {
    KBDLLHOOKSTRUCT *kb_data = (KBDLLHOOKSTRUCT*)lparam;
    WINBGINPUT_KEY_HANDLER(kb_data->vkCode);
  }

  return CallNextHookEx(WINBGINPUT_HOOK_HANDLE, code, wparam, lparam);
}

/**
 * @brief Initializes the bg input variables
 *
 * This is a no-op if we are already initialized
 * @post WINBGINPUT_HOOK_HANDLE != NULL
 * */
BOOL bg_input_init() {
  if (WINBGINPUT_HOOK_HANDLE != NULL) {
    return TRUE;
  }

  HMODULE main_module_handle = get_main_module_handle();
  if (main_module_handle == NULL) {
    return FALSE;
  }

  WINBGINPUT_HOOK_HANDLE = SetWindowsHookExA(
      WH_KEYBOARD_LL,
      &hook_fn,
      main_module_handle,
      0
  );

  return TRUE;
}

/**
 * @brief Deinitializes the bg input variables
 *
 * This is a no-op if we are not initalized
 * @post WINBGINPUT_HOOK_HANDLE == NULL
 */
void bg_input_deinit() {
  if (WINBGINPUT_HOOK_HANDLE == NULL) {
    return;
  }

  UnhookWindowsHookEx(WINBGINPUT_HOOK_HANDLE);
  WINBGINPUT_HOOK_HANDLE = NULL;
}

/**
 * @brief Sets a key handler function as the one to call
 *
 * There can only be one key handler at a time, so if one was already set, this
 * replaces the previous one.
 *
 * @pre kh is a valid function pointer
 */
void bg_input_register_callback(key_handler_fp kh) {
  WINBGINPUT_KEY_HANDLER = kh;
}
