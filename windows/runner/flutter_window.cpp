#include "flutter_window.h"
#include <flutter/event_channel.h>
#include <flutter/event_sink.h>
#include <flutter/event_stream_handler_functions.h>
#include <flutter/method_channel.h>
#include <flutter/standard_method_codec.h>
#include <Windows.h>

#include <optional>

#include "flutter/generated_plugin_registrant.h"

FlutterWindow::FlutterWindow(const flutter::DartProject& project)
    : project_(project) {}

FlutterWindow::~FlutterWindow() {}

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>

#include <stdio.h>
#include <stdlib.h>

// Need to link with Wlanapi.lib and Ole32.lib
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")


int listAllNetworks()
{

    // Declare and initialize variables.

    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2;      //    
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;
    DWORD dwRetVal = 0;
    int iRet = 0;

    WCHAR GuidString[39] = { 0 };

    unsigned int i;

    /* variables used for WlanEnumInterfaces  */

    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    PWLAN_INTERFACE_INFO pIfInfo = NULL;

    PWLAN_AVAILABLE_NETWORK_LIST pBssList = NULL;
    //PWLAN_AVAILABLE_NETWORK pBssEntry = NULL;

    

    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
        return 1;
        // You can use FormatMessage here to find out why the function failed
    }

    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
        return 1;
        // You can use FormatMessage here to find out why the function failed
    }
    else {
        wprintf(L"Num Entries: %lu\n", pIfList->dwNumberOfItems);
        wprintf(L"Current Index: %lu\n", pIfList->dwIndex);
        for (i = 0; i < (int)pIfList->dwNumberOfItems; i++) {
            pIfInfo = (WLAN_INTERFACE_INFO*)&pIfList->InterfaceInfo[i];
            wprintf(L"  Interface Index[%u]:\t %lu\n", i, i);
            iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString,
                sizeof(GuidString) / sizeof(*GuidString));
            // For c rather than C++ source code, the above line needs to be
            // iRet = StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 
            //     sizeof(GuidString)/sizeof(*GuidString)); 
            if (iRet == 0)
                wprintf(L"StringFromGUID2 failed\n");
            else {
                wprintf(L"  InterfaceGUID[%d]: %ws\n", i, GuidString);
            }
            wprintf(L"  Interface Description[%d]: %ws", i,
                pIfInfo->strInterfaceDescription);
            wprintf(L"\n");
            wprintf(L"  Interface State[%d]:\t ", i);
            switch (pIfInfo->isState) {
            case wlan_interface_state_not_ready:
                wprintf(L"Not ready\n");
                break;
            case wlan_interface_state_connected:
                wprintf(L"Connected\n");
                break;
            case wlan_interface_state_ad_hoc_network_formed:
                wprintf(L"First node in a ad hoc network\n");
                break;
            case wlan_interface_state_disconnecting:
                wprintf(L"Disconnecting\n");
                break;
            case wlan_interface_state_disconnected:
                wprintf(L"Not connected\n");
                break;
            case wlan_interface_state_associating:
                wprintf(L"Attempting to associate with a network\n");
                break;
            case wlan_interface_state_discovering:
                wprintf(L"Auto configuration is discovering settings for the network\n");
                break;
            case wlan_interface_state_authenticating:
                wprintf(L"In process of authenticating\n");
                break;
            default:
                wprintf(L"Unknown state %ld\n", pIfInfo->isState);
                break;
            }
            wprintf(L"\n");

            dwResult = WlanGetAvailableNetworkList(hClient,
                &pIfInfo->InterfaceGuid,
                0,
                NULL,
                &pBssList);

            if (dwResult != ERROR_SUCCESS) {
                wprintf(L"WlanGetAvailableNetworkList failed with error: %u\n",
                    dwResult);
                dwRetVal = 1;
                // You can use FormatMessage to find out why the function failed
            }
            else {
                wprintf(L"WLAN_AVAILABLE_NETWORK_LIST for this interface\n");
            }
            wchar_t strXml[2524];
            
            wchar_t szProfileStringFmt[] =
                L"	<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">\n"
                L"	<name>%S</name>\n"
                L"	<SSIDConfig>\n"
                L"		<SSID>\n"
                L"		<name>%S</name>\n"
                L"		</SSID>\n"
                L"	</SSIDConfig>\n"
                L"	<connectionType>ESS</connectionType>\n"
                L"	<connectionMode>auto</connectionMode>\n"
                L"  <autoSwitch >false</autoSwitch>\n"
                L"	<MSM>\n"
                L"		<security>\n"
                L"			<authEncryption>\n"
                L"				<authentication>WPA2</authentication>\n"
                L"				<encryption>AES</encryption>\n"
                L"				<useOneX>true</useOneX>\n"
                L"			</authEncryption>\n"
                L"         <OneX xmlns=\"http://www.microsoft.com/networking/OneX/v1\">\n"
                L"            <EAPConfig>\n"
                L"               <EapHostConfig xmlns=\"http://www.microsoft.com/provisioning/EapHostConfig\" xmlns:baseEap=\"http://www.microsoft.com/provisioning/BaseEapMethodConfig\" xmlns:eapCommon=\"http://www.microsoft.com/provisioning/EapCommon\">\n"
                L"                  <EapMethod>\n"
                L"                     <eapCommon:Type>13</eapCommon:Type>\n"
                L"                     <eapCommon:AuthorId>0</eapCommon:AuthorId>\n"
                L"                  </EapMethod>\n"
                L"                  <Config xmlns:baseEap=\"http://www.microsoft.com/provisioning/BaseEapConnectionPropertiesV1\" xmlns:eapTls=\"http://www.microsoft.com/provisioning/EapTlsConnectionPropertiesV1\">\n"
                L"                     <baseEap:Eap>\n"
                L"                        <baseEap:Type>13</baseEap:Type>\n"
                L"                        <eapTls:EapType>\n"
                L"                           <eapTls:CredentialsSource>\n"
                L"                              <eapTls:CertificateStore />\n"
                L"                           </eapTls:CredentialsSource>\n"
                L"                           <eapTls:ServerValidation>\n"
                L"                              <eapTls:DisableUserPromptForServerValidation>false</eapTls:DisableUserPromptForServerValidation>\n"
                L"                              <eapTls:ServerNames />\n"
                L"                              <eapTls:TrustedRootCA />\n"
                L"                           </eapTls:ServerValidation>\n"
                L"                           <eapTls:DifferentUsername>false</eapTls:DifferentUsername>\n"
                L"                        </eapTls:EapType>\n"
                L"                     </baseEap:Eap>\n"
                L"                  </Config>\n"
                L"               </EapHostConfig>\n"
                L"            </EAPConfig>\n"
                L"         </OneX>\n"
                L"		</security>\n"
                L"	</MSM>\n"
                L"</WLANProfile>\n"
                ;

            DWORD dwReason;
            _snwprintf_s(strXml, _countof(strXml), szProfileStringFmt, "TheWalnut", "TheWalnut");


            // set profile
            DWORD dwError;
            dwError = WlanSetProfile(hClient, &pIfInfo->InterfaceGuid, 0,          // no flags for the profile 
                strXml,
                NULL,       // use the default ACL
                TRUE,		// overwrite a profile if it already exists
                NULL,       // reserved
                &dwReason
            );
            
            switch (dwError)
            {
            case ERROR_INVALID_PARAMETER:
                puts("The profile has invalid params.");
                break;
            case ERROR_BAD_PROFILE:
                puts("The profile is bad.");
                break;
            case ERROR_ALREADY_EXISTS:
                puts("The profile is already exists.");
            }
            
            //if (dwResult != ERROR_SUCCESS) {
            //    wprintf(L"WlanGetAvailableNetworkList failed with error: %u\n",
            //        dwResult);
            //    dwRetVal = 1;
            //    // You can use FormatMessage to find out why the function failed
            //}
            //else {
            //    wprintf(L"WLAN_AVAILABLE_NETWORK_LIST for this interface\n");

            //    wprintf(L"  Num Entries: %lu\n\n", pBssList->dwNumberOfItems);

            //    for (j = 0; j < pBssList->dwNumberOfItems; j++) {
            //        pBssEntry =
            //            (WLAN_AVAILABLE_NETWORK*)&pBssList->Network[j];

            //        wprintf(L"  Profile Name[%u]:  %ws\n", j, pBssEntry->strProfileName);

            //        wprintf(L"  SSID[%u]:\t\t ", j);
            //        if (pBssEntry->dot11Ssid.uSSIDLength == 0)
            //            wprintf(L"\n");
            //        else {
            //            for (k = 0; k < pBssEntry->dot11Ssid.uSSIDLength; k++) {
            //                wprintf(L"%c", (int)pBssEntry->dot11Ssid.ucSSID[k]);
            //            }
            //            wprintf(L"\n");
            //        }

            //        wprintf(L"  BSS Network type[%u]:\t ", j);
            //        switch (pBssEntry->dot11BssType) {
            //        case dot11_BSS_type_infrastructure:
            //            wprintf(L"Infrastructure (%u)\n", pBssEntry->dot11BssType);
            //            break;
            //        case dot11_BSS_type_independent:
            //            wprintf(L"Infrastructure (%u)\n", pBssEntry->dot11BssType);
            //            break;
            //        default:
            //            wprintf(L"Other (%lu)\n", pBssEntry->dot11BssType);
            //            break;
            //        }

            //        wprintf(L"  Number of BSSIDs[%u]:\t %u\n", j, pBssEntry->uNumberOfBssids);

            //        wprintf(L"  Connectable[%u]:\t ", j);
            //        if (pBssEntry->bNetworkConnectable)
            //            wprintf(L"Yes\n");
            //        else {
            //            wprintf(L"No\n");
            //            wprintf(L"  Not connectable WLAN_REASON_CODE value[%u]:\t %u\n", j,
            //                pBssEntry->wlanNotConnectableReason);
            //        }

            //        wprintf(L"  Number of PHY types supported[%u]:\t %u\n", j, pBssEntry->uNumberOfPhyTypes);

            //        if (pBssEntry->wlanSignalQuality == 0)
            //            iRSSI = -100;
            //        else if (pBssEntry->wlanSignalQuality == 100)
            //            iRSSI = -50;
            //        else
            //            iRSSI = -100 + (pBssEntry->wlanSignalQuality / 2);

            //        wprintf(L"  Signal Quality[%u]:\t %u (RSSI: %i dBm)\n", j,
            //            pBssEntry->wlanSignalQuality, iRSSI);

            //        wprintf(L"  Security Enabled[%u]:\t ", j);
            //        if (pBssEntry->bSecurityEnabled)
            //            wprintf(L"Yes\n");
            //        else
            //            wprintf(L"No\n");

            //        wprintf(L"  Default AuthAlgorithm[%u]: ", j);
            //        switch (pBssEntry->dot11DefaultAuthAlgorithm) {
            //        case DOT11_AUTH_ALGO_80211_OPEN:
            //            wprintf(L"802.11 Open (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            //            break;
            //        case DOT11_AUTH_ALGO_80211_SHARED_KEY:
            //            wprintf(L"802.11 Shared (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            //            break;
            //        case DOT11_AUTH_ALGO_WPA:
            //            wprintf(L"WPA (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            //            break;
            //        case DOT11_AUTH_ALGO_WPA_PSK:
            //            wprintf(L"WPA-PSK (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            //            break;
            //        case DOT11_AUTH_ALGO_WPA_NONE:
            //            wprintf(L"WPA-None (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            //            break;
            //        case DOT11_AUTH_ALGO_RSNA:
            //            wprintf(L"RSNA (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            //            break;
            //        case DOT11_AUTH_ALGO_RSNA_PSK:
            //            wprintf(L"RSNA with PSK(%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            //            break;
            //        default:
            //            wprintf(L"Other (%lu)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            //            break;
            //        }

            //        wprintf(L"  Default CipherAlgorithm[%u]: ", j);
            //        switch (pBssEntry->dot11DefaultCipherAlgorithm) {
            //        case DOT11_CIPHER_ALGO_NONE:
            //            wprintf(L"None (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
            //            break;
            //        case DOT11_CIPHER_ALGO_WEP40:
            //            wprintf(L"WEP-40 (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
            //            break;
            //        case DOT11_CIPHER_ALGO_TKIP:
            //            wprintf(L"TKIP (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
            //            break;
            //        case DOT11_CIPHER_ALGO_CCMP:
            //            wprintf(L"CCMP (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
            //            break;
            //        case DOT11_CIPHER_ALGO_WEP104:
            //            wprintf(L"WEP-104 (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
            //            break;
            //        case DOT11_CIPHER_ALGO_WEP:
            //            wprintf(L"WEP (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
            //            break;
            //        default:
            //            wprintf(L"Other (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
            //            break;
            //        }

            //        wprintf(L"  Flags[%u]:\t 0x%x", j, pBssEntry->dwFlags);
            //        if (pBssEntry->dwFlags) {
            //            if (pBssEntry->dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED)
            //                wprintf(L" - Currently connected");
            //            if (pBssEntry->dwFlags & WLAN_AVAILABLE_NETWORK_HAS_PROFILE)
            //                wprintf(L" - Has profile");
            //        }
            //        wprintf(L"\n");

            //        wprintf(L"\n");
            //    }
            //}
        }

    }
    if (pBssList != NULL) {
        WlanFreeMemory(pBssList);
        pBssList = NULL;
    }

    if (pIfList != NULL) {
        WlanFreeMemory(pIfList);
        pIfList = NULL;
    }

    return dwRetVal;
}



bool FlutterWindow::OnCreate() {
  if (!Win32Window::OnCreate()) {
    return false;
  }

  //RegisterPlugins(flutter_controller_->engine());

 


  RECT frame = GetClientArea();

  // The size here must match the window dimensions to avoid unnecessary surface
  // creation / destruction in the startup path.
  flutter_controller_ = std::make_unique<flutter::FlutterViewController>(
      frame.right - frame.left, frame.bottom - frame.top, project_);
  // Ensure that basic setup of the controller was successful.
  if (!flutter_controller_->engine() || !flutter_controller_->view()) {
    return false;
  }

  RegisterPlugins(flutter_controller_->engine());
  
  flutter::MethodChannel<> channel(
      flutter_controller_->engine()->messenger(), "samples.flutter.dev/battery",
      &flutter::StandardMethodCodec::GetInstance());
  channel.SetMethodCallHandler(
      [](const flutter::MethodCall<>& call,
          std::unique_ptr<flutter::MethodResult<>> result) {
              //TODO
              if (call.method_name() == "getBatteryLevel") {
                  int batteryLevel = listAllNetworks();
                  if (batteryLevel != -1) {
                      result->Success(batteryLevel);
                  }
                  else {
                      result->Error("Unavailable", "Battery level not available.");
                  }
              }
              else {
                  result->NotImplemented();
              }
      });
  
  SetChildContent(flutter_controller_->view()->GetNativeWindow());

  flutter_controller_->engine()->SetNextFrameCallback([&]() {
    this->Show();
  });

  return true;
}


void FlutterWindow::OnDestroy() {
  if (flutter_controller_) {
    flutter_controller_ = nullptr;
  }

  Win32Window::OnDestroy();
}

LRESULT
FlutterWindow::MessageHandler(HWND hwnd, UINT const message,
                              WPARAM const wparam,
                              LPARAM const lparam) noexcept {
  // Give Flutter, including plugins, an opportunity to handle window messages.
  if (flutter_controller_) {
    std::optional<LRESULT> result =
        flutter_controller_->HandleTopLevelWindowProc(hwnd, message, wparam,
                                                      lparam);
    if (result) {
      return *result;
    }
  }

  switch (message) {
    case WM_FONTCHANGE:
      flutter_controller_->engine()->ReloadSystemFonts();
      break;
  }

  return Win32Window::MessageHandler(hwnd, message, wparam, lparam);
}
