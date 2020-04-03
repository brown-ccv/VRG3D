//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.

// Windows Specific Implementation

#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlctl.h>
#include <atlhost.h>
using namespace ATL;

#include "SpaceNavDevice.h"

#import "progid:TDxInput.Device.1" no_namespace

    CComPtr<ISensor> g3DSensor;
    MSG Message;

#include <atlstr.h>

   LPTSTR SPACENAV(_T("SpaceWareMessage00"));
   UINT SN_MESS;


namespace VRG3D {

using namespace G3D;

SpaceNavDevice::~SpaceNavDevice() {

}

void
SpaceNavDevice::setup()
{
    HRESULT hr;
    //cout << "Win32 Setup";
    SN_MESS = RegisterWindowMessage(SPACENAV);
    CComPtr<IUnknown> _3DxDevice;


     hr=::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED );
   if (!SUCCEEDED(hr))
   {
      CString strError;
      strError.FormatMessage (_T("Error 0x%x"), hr);
      ::MessageBox (NULL, strError, _T("CoInitializeEx failed"), MB_ICONERROR|MB_OK);

   }


     // Create the device object
     hr = _3DxDevice.CoCreateInstance(__uuidof(Device));
     if (SUCCEEDED(hr))
     {
        CComPtr<ISimpleDevice> _3DxSimpleDevice;

        hr = _3DxDevice.QueryInterface(&_3DxSimpleDevice);
        if (SUCCEEDED(hr))
        {
           // Get the interfaces to the sensor and the keyboard;
           g3DSensor = _3DxSimpleDevice->Sensor;

           // Associate a configuration with this device
           //_3DxSimpleDevice->LoadPreferences(_T("Cube3DPolling"));
           // Connect to the driver
           _3DxSimpleDevice->Connect();

           // Create timer used to poll the 3dconnexion device
           //gTimerId = ::SetTimer(NULL, 0, 25, _3DxTimerProc);
        }
     }
}


void  SpaceNavDevice::pollForInput(Array<EventRef> &events)
{
   if(PeekMessage(&Message, 0, SN_MESS, SN_MESS, PM_REMOVE)){
         DispatchMessage(&Message);
   if (g3DSensor)
   {
      try {
         CComPtr<IAngleAxis> pRotation = g3DSensor->Rotation;
         CComPtr<IVector3D> pTranslation = g3DSensor->Translation;

         if (pRotation->Angle > 0. || pTranslation->Length > 0.)
         {
     //calibrating the same as mac, windows results are (-1, 1) for rot
     // and (-80, 80) for trans, mac are not so simple, also mac have some axis reversed, possibly
     // settings stuff?
     Vector3 trans(pTranslation->X / 80.0, pTranslation->Z / 80.0, -pTranslation->Y / 80.0);
       Vector3 rot(pRotation->X, pRotation->Z, -pRotation->Y);

        events.append(new VRG3D::Event("SpaceNav_Trans", trans));
        events.append(new VRG3D::Event("SpaceNav_Rot", rot));
        //cout << trans << " " << rot << endl;
     }else{
        Vector3 trans = Vector3::zero();
        Vector3 rot = Vector3::zero();


        events.append(new VRG3D::Event("SpaceNav_Trans", trans));
        events.append(new VRG3D::Event("SpaceNav_Rot", rot));
     }

      }
      catch (...)
      {
      }
   }
   }
}

} // end namespace
