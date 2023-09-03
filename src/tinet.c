#include "tinet.h"

const system_info_t *systemInfo;
char calcidStr[sizeof(systemInfo->calcid) * 2 + 1];

bool init_success = false;

char *tinet_username;
char *tinet_authkey;
uint8_t tinet_netkey_appvar;

bool USB_connected = false;
bool prev_USB_connected = false;
bool USB_connecting = false;
bool bridge_connected = false;
bool internet_connected = false;
bool has_unread_data = false;
bool is_esp8266 = false;
bool has_srl_device = false;
bool serial_init_data_sent = false;

srl_device_t srl;
uint8_t tinet_srl_buf[2048];

char tinet_in_buffer[32768];

usb_error_t usb_error;
const usb_standard_descriptors_t *usb_desc;

usb_error_t tinet_handle_usb_event(usb_event_t event, void *event_data, usb_callback_data_t *callback_data)
{
    usb_error_t err;
    if ((err = srl_UsbEventCallback(event, event_data, callback_data)) != USB_SUCCESS)
        return err;
    if (event == USB_DEVICE_CONNECTED_EVENT && !(usb_GetRole() & USB_ROLE_DEVICE))
    {
        usb_device_t device = event_data;
        USB_connected = true;
        usb_ResetDevice(device);
    }

    if (event == USB_HOST_CONFIGURE_EVENT || (event == USB_DEVICE_ENABLED_EVENT && !(usb_GetRole() & USB_ROLE_DEVICE)))
    {

        if (has_srl_device)
            return USB_SUCCESS;

        usb_device_t device;
        if (event == USB_HOST_CONFIGURE_EVENT)
        {
            device = usb_FindDevice(NULL, NULL, USB_SKIP_HUBS);
            if (device == NULL)
                return USB_SUCCESS;
        }
        else
        {
            device = event_data;
        }

        srl_error_t error = srl_Open(&srl, device, tinet_srl_buf, sizeof tinet_srl_buf, SRL_INTERFACE_ANY, 9600);
        if (error)
        {
            gfx_End();
            os_ClrHome();
            printf("Error %d initting serial\n", error);
            while (os_GetCSC())
                ;
            return 0;
            return USB_SUCCESS;
        }
        has_srl_device = true;
    }

    if (event == USB_DEVICE_DISCONNECTED_EVENT)
    {
        usb_device_t device = event_data;
        if (device == srl.dev)
        {
            USB_connected = false;
            srl_Close(&srl);
            has_srl_device = false;
        }
    }

    return USB_SUCCESS;
}

size_t tinet_write_serial(const char *message)
{
    size_t totalBytesWritten = 0;
    size_t messageLength = strlen(message);

    while (totalBytesWritten < messageLength)
    {
        int bytesWritten = srl_Write(&srl, message + totalBytesWritten, messageLength - totalBytesWritten);

        if (bytesWritten < 0)
        {
            printf("SRL W ERR");
        }

        totalBytesWritten += bytesWritten;
    }
    return totalBytesWritten;
}

enum TINETInitStatus tinet_init()
{
    for (unsigned int i = 0; i < sizeof(systemInfo->calcid); i++)
    {
        sprintf(calcidStr + i * 2, "%02X", systemInfo->calcid[i]);
    }

    const usb_standard_descriptors_t *usb_desc = srl_GetCDCStandardDescriptors();
    usb_error_t usb_error = usb_Init(tinet_handle_usb_event, NULL, usb_desc, USB_DEFAULT_INIT_FLAGS);
    if (usb_error)
    {
        printf("usb init error\n%u\n", usb_error);
        msleep(1000);
        return TINET_INIT_FAIL;
    }

    init_success = true;
    return TINET_INIT_SUCCESS;
}

enum TINETLoginStatus tinet_login()
{
    if (!USB_connected && !USB_connecting && bridge_connected)
    {
        USB_connecting = true;

        tinet_write_serial("SERIAL_CONNECTED");
        while (!internet_connected)
        {
            tinet_handle_srl();
        }

        tinet_netkey_appvar = ti_Open("NETKEY", "r");

        if (tinet_netkey_appvar == 0)
        {
            return TINET_LOGIN_NO_KEYFILE;
        }
        else
        {
            uint8_t *data_ptr = (uint8_t *)ti_GetDataPtr(tinet_netkey_appvar);
            ti_Close(tinet_netkey_appvar);

            char *read_tinet_username = (char *)data_ptr;
            tinet_username = read_tinet_username;

            size_t read_un_len = strlen(read_tinet_username) + 1;
            data_ptr += (read_un_len + 1);

            char *read_key = (char *)data_ptr - 1;
            tinet_authkey = read_key;

            char login_msg[93];
            snprintf(login_msg, sizeof(login_msg), "LOGIN:%s:%s:%s", calcidStr, tinet_username, tinet_authkey);
            tinet_write_serial(login_msg);

            return TINET_LOGIN_SUCCESS;
        }
    }
    else
    {
        return TINET_LOGIN_NO_BRIDGE;
    }
}

void tinet_handle_server_command()
{
    if (strcmp(tinet_in_buffer, "SERIAL_CONNECTED_CONFIRMED_BY_SERVER") == 0)
    {
        bridge_connected = true;
        internet_connected = true;
    }

    if (strcmp(tinet_in_buffer, "MAIL_NOT_VERIFIED") == 0)
    {
        printf("Mail not\nverified!\n");
    }

    if (strcmp(tinet_in_buffer, "ALREADY_CONNECTED") == 0)
    {
        printf("Already\nConnected\n");
    }

    if (strcmp(tinet_in_buffer, "USER_NOT_FOUND") == 0)
    {
        printf("User not\nfound\n");
    }

    if (strcmp(tinet_in_buffer, "INVALID_CALC_KEY") == 0)
    {
        printf("Invalid\ncalc key\n");
    }

    if (strcmp(tinet_in_buffer, "DIFFERENT_CALC_ID") == 0)
    {
        printf("Different\ncalc ID\n");
    }

    if (strcmp(tinet_in_buffer, "CALC_BANNED") == 0)
    {
        printf("You're\nbanned.\n");
    }

    if (strcmp(tinet_in_buffer, "CALC_ID_UPDATE_NEEDED") == 0)
    {
        printf("CalcID\nupdate\nneeded\n");
    }

    if (strcmp(tinet_in_buffer, "ESP8266") == 0)
    {
        is_esp8266 = true;
        printf("ESP8266\n");
    }
}

char tinet_handle_srl()
{
    usb_HandleEvents();

    size_t bytes_read = srl_Read(&srl, tinet_in_buffer, sizeof tinet_in_buffer);
    if (bytes_read < 0)
    {
        printf("SRL ERR");
    }
    else if (bytes_read > 0)
    {
        has_unread_data = true;
        tinet_in_buffer[bytes_read] = '\0';
        tinet_handle_server_command();
        has_unread_data = false;
    }
    
    return tinet_in_buffer;
}

bool tinetIsBridgeConnected()
{
    return bridge_connected;
}

bool tinetIsInternetConnected()
{
    return internet_connected;
}

char tinetGettinet_username()
{
    return *tinet_username;
}