#include <stdio.h>
#include <string>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "heapless_json_stream_parser.h"

const char* json_str = R"(
{
    "kind": "identitytoolkit#VerifyPasswordResponse",
    "localId": "ZKPNYTdrY0gwQvHL6xC6CW7F3Hx2",
    "email": "tharusha.udana529@gmail.com",
    "displayName": "",
    "idToken": "eyJhbGciOiJSUzI1NiIsImtpZCI6IjAzZDA3YmJjM2Q3NWM2OTQyNzUxMGY2MTc0ZWIyZjE2NTQ3ZDRhN2QiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJodHRwczovL3NlY3VyZXRva2VuLmdvb2dsZS5jb20vZ3JlZW5ob3VzZS1wcm9qZWN0LWJlYzFlIiwiYXVkIjoiZ3JlZW5ob3VzZS1wcm9qZWN0LWJlYzFlIiwiYXV0aF90aW1lIjoxNjk4MjI4NzIxLCJ1c2VyX2lkIjoiWktQTllUZHJZMGd3UXZITDZ4QzZDVzdGM0h4MiIsInN1YiI6IlpLUE5ZVGRyWTBnd1F2SEw2eEM2Q1c3RjNIeDIiLCJpYXQiOjE2OTgyMjg3MjEsImV4cCI6MTY5ODIzMjMyMSwiZW1haWwiOiJ0aGFydXNoYS51ZGFuYTUyOUBnbWFpbC5jb20iLCJlbWFpbF92ZXJpZmllZCI6ZmFsc2UsImZpcmViYXNlIjp7ImlkZW50aXRpZXMiOnsiZW1haWwiOlsidGhhcnVzaGEudWRhbmE1MjlAZ21haWwuY29tIl19LCJzaWduX2luX3Byb3ZpZGVyIjoicGFzc3dvcmQifX0.n7UrtMqawGG0yyn17IYqnWoX7IITCkj7pTZtS_B03NZpVcPDn7tVDUFrHIYmGqaksZvv9Q3gnwO2u24719cbSkqkSGCcoZyZ4YYMbX1eAaFc6_sOa_dAE7qVRPilKVCl_t-QS5IwOVwYYLhiqR2ek73VZch8BbpQeR-M0oXnqH-4fCbJKbCBYP__N61oPUwRir5C1nkMbVfZmEPcQoiQ148PoQADfzrqkXKH9lenwf4jnDVHZqaBlOJFJE8iUpsW5eeTO9_f-TWUKwKGFGBPtJHsIcWDEx3dDFnTpMokswh56J4aD7PStENHzRoGqpA8mEXiijnLeCGY7pmQMcsUuQ",
    "registered": true,
    "refreshToken": "AMf-vBwS9cj1jKbsV2NR__m54dnTXsZsatZ1qMoekSIXmIic-ZipMLTSsRMWi5UempRLfxpCgJNDmIaSFfPL1qlZ-L9KF51vEdxLuOTjKJOOLZcwyl0P_W_oVgHhdFKlGQBtBK-NFLJi4spfAgSMERTh4IY3gf_41A8en1ONNp7pKtWQu1ltx3Bk93NHqetuMATm5ShHkExWU-wu_HhCNodVCJLosXaHfXlLfplIXD8Y2q0OBSlM7oA5WRfPdP37A7gmIM5zgeSj",
    "expiresIn": "3600k"
}
)";

json_stream_parser jparser([](std::string path, json_val_t value) {
    ESP_LOGI("TAG", "%s >>> %s", path.c_str(), value.val.c_str());
    if (path == "idToken") {
        std::string e;
        value.get_value(e);
        ESP_LOGI("TAGG", "expiresIn --> %s", e.c_str());
    }
});

extern "C" void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    size_t s = strlen(json_str);
    int i = 0;

    while (i < s)
    {
        jparser.parse(json_str[i]);
        i++;
    }

    /*for (int i = 0; i < sizeof(json_str); i++)
    {
        
        //jparser.parse(json_str[i]);
    }*/
    
}